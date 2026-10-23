#!/usr/bin/env python3
"""Play a fixed Sea Battle scenario using the real game's terminal prompts."""
import argparse
import errno
import os
import pty
import re
import select
import shutil
import socket
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FLEET = [('h', 'A 0'), ('h', 'C 0'), ('h', 'E 0'), ('h', 'G 0'), ('h', 'G 3'), ('h', 'G 6'), ('', 'I 0'), ('', 'I 3'), ('', 'I 6'), ('', 'I 9')]
# Client uses the transposed layout, so both orientations appear in the demo.
CLIENT_FLEET = [('v' if orientation else '', chr(65 + int(cell[2])) + ' ' + str(ord(cell[0]) - 65)) for orientation, cell in FLEET]
CLIENT_CELLS = ['D 0', 'C 0', 'B 0', 'A 0', 'C 2', 'B 2', 'A 2', 'C 4', 'B 4', 'A 4', 'B 6', 'A 6', 'E 6', 'D 6', 'H 6', 'G 6', 'A 8', 'D 8', 'G 8', 'J 8']
PROMPT = re.compile(r'Place \[H\].*? : |Please, input coordinates.*?: |Fleet setup completed! Proceed\? \[y/n\]: |Please, enter valid firing coordinates: ')

def inputs(side):
    result = []
    for orientation, cell in FLEET if side == 'host' else CLIENT_FLEET:
        if orientation:
            result.append(orientation)
        result.append(cell)
    result.append('y')
    # Host misses. Client hits twice and misses. Host then sinks the client fleet.
    result.extend(['J 9'] + CLIENT_CELLS if side == 'host' else ['A 0', 'A 1', 'J 9'])
    return result

def play(side, port, delay, log_path, ready_file=None, visible=True):
    master, slave = pty.openpty()
    args = ['--host', str(port)] if side == 'host' else ['--connect', '127.0.0.1', str(port)]
    process = subprocess.Popen([str(ROOT / 'SeaBattle'), *args], stdin=subprocess.PIPE, stdout=slave, stderr=slave)
    os.close(slave)
    pending = ''
    transcript = ''
    steps = iter(inputs(side))
    deadline = time.monotonic() + 120 + delay * len(inputs(side))
    try:
        while time.monotonic() < deadline:
            readable, _, _ = select.select([master], [], [], .2)
            if readable:
                try:
                    data = os.read(master, 65536)
                except OSError as error:
                    if error.errno == errno.EIO:
                        break
                    raise
                if not data:
                    break
                if visible:
                    sys.stdout.buffer.write(data)
                    sys.stdout.buffer.flush()
                text = data.decode('utf-8', errors='replace')
                transcript += text
                pending += text
                if ready_file and 'Hosting server on port:' in transcript:
                    Path(ready_file).touch()
                while True:
                    match = PROMPT.search(pending)
                    if not match:
                        break
                    pending = pending[match.end():]
                    answer = next(steps, None)
                    if answer is None:
                        raise RuntimeError(f'{side}: scenario ran out of inputs')
                    time.sleep(delay)
                    if visible:
                        print(answer, flush=True)
                    transcript += answer + '\n'
                    process.stdin.write((answer + '\n').encode())
                    process.stdin.flush()
            elif process.poll() is not None:
                break
        else:
            raise RuntimeError(f'{side}: scenario timed out')
        code = process.wait(timeout=5)
        expected = 'You won!' if side == 'host' else 'You lost!'
        if code or expected not in transcript:
            raise RuntimeError(f'{side}: expected {expected!r}, exit code {code}; see {log_path}')
        return 0
    finally:
        if process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
        process.stdin.close()
        os.close(master)
        Path(log_path).write_text(transcript)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--side', choices=['host', 'client'], help='Run one side in this terminal')
    parser.add_argument('--port', type=int, default=5000)
    parser.add_argument('--delay', type=float, default=.5, help='Seconds before each scripted input')
    parser.add_argument('--headless', action='store_true', help='Run both sides with logs and no windows')
    parser.add_argument('--log-dir', type=Path, default=ROOT / 'build-scenario')
    parser.add_argument('--ready-file', help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.delay < 0 or not 1 <= args.port <= 65535:
        parser.error('Use a nonnegative delay and port 1-65535')
    if not (ROOT / 'SeaBattle').is_file():
        parser.error('Build the game first with make')
    args.log_dir.mkdir(parents=True, exist_ok=True)
    if args.side:
        return play(args.side, args.port, args.delay, args.log_dir / (args.side + '.log'), args.ready_file)
    if not args.headless and not shutil.which('xterm'):
        parser.error('Install xterm, use --headless, or run --side host/client in two terminals')
    # Pick a free local port for the automatic two-player demo.
    with socket.socket() as reservation:
        reservation.bind(('127.0.0.1', 0))
        port = reservation.getsockname()[1]
    ready = args.log_dir / 'host-ready'
    ready.unlink(missing_ok=True)
    children = []
    try:
        for side in ('host', 'client'):
            command = [sys.executable, str(Path(__file__).resolve()), '--side', side, '--port', str(port), '--delay', str(args.delay), '--log-dir', str(args.log_dir)]
            if side == 'host':
                command += ['--ready-file', str(ready)]
            if not args.headless:
                command = ['xterm', '-hold', '-geometry', '100x40', '-T', 'Sea Battle — ' + side, '-e', *command]
            children.append(subprocess.Popen(command, stdout=subprocess.DEVNULL if args.headless else None))
            if side == 'host':
                deadline = time.monotonic() + 10
                while not ready.exists():
                    if children[0].poll() is not None or time.monotonic() > deadline:
                        raise RuntimeError('Host did not start; see host.log')
                    time.sleep(.05)
                time.sleep(.1)
        if args.headless:
            codes = [child.wait(timeout=150 + args.delay * 60) for child in children]
            if any(codes):
                raise RuntimeError('Scenario failed; inspect host.log and client.log')
            print('Scenario passed: host wins, client loses. Logs: ' + str(args.log_dir))
        else:
            print('Opened both players. The host will win; windows stay open for inspection.')
            print('Logs: ' + str(args.log_dir))
        return 0
    finally:
        if args.headless:
            for child in children:
                if child.poll() is None:
                    child.terminate()
                    child.wait()

if __name__ == '__main__':
    try:
        sys.exit(main())
    except (RuntimeError, OSError, subprocess.TimeoutExpired) as error:
        print(str(error), file=sys.stderr)
        sys.exit(1)
