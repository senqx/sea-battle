#!/usr/bin/env python3
"""Exercise real processes over loopback, including fragmented TCP messages."""
import os
import socket
import subprocess
import tempfile
import time
from contextlib import contextmanager
from pathlib import Path

APP = os.environ.get('SEA_BATTLE_TEST_APP', str(Path(__file__).resolve().parents[1] / 'SeaBattle'))
NUMERIC_FLEET = 'h\n0 0\nh\n2 0\nh\n4 0\nh\n6 0\nh\n6 3\nh\n6 6\n8 0\n8 3\n8 6\n8 9\ny\n'
def letter_rows(text):
    return '\n'.join(chr(65 + int(line.split()[0])) + ' ' + line.split()[1] if ' ' in line and line.split()[0].isdigit() else line for line in text.splitlines()) + '\n'
FLEET = letter_rows(NUMERIC_FLEET)

CELLS = [(0, c) for c in range(4)] + [(r, c) for r in (2, 4) for c in range(3)] + [(6, c) for c in (0, 1, 3, 4, 6, 7)] + [(8, c) for c in (0, 3, 6, 9)]

def port():
    with socket.socket() as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]

@contextmanager
def game(args, text):
    with tempfile.TemporaryFile(mode='w+') as log, tempfile.TemporaryFile(mode='w+') as source:
        source.write(text)
        source.seek(0)
        p = subprocess.Popen([APP, *args], stdin=source, stdout=log, stderr=log)
        try:
            yield p, log
        finally:
            if p.poll() is None:
                p.kill()
            p.wait()

def output(log):
    log.seek(0)
    return log.read()

def connect(port_number):
    deadline = time.monotonic() + 5
    while time.monotonic() < deadline:
        try:
            s = socket.create_connection(('127.0.0.1', port_number), timeout=2)
            return s
        except ConnectionRefusedError:
            time.sleep(.02)
    raise AssertionError('Host did not start')

def receive(s, size):
    data = b''
    while len(data) < size:
        part = s.recv(size - len(data))
        assert part, 'Unexpected disconnect'
        data += part
    return data

# Malformed arguments must fail promptly, rather than crash or wait for input.
for args in [['--connect', '127.0.0.1'], ['--host', '--host'], ['--host', '12junk'], ['--host', '0'], ['--connect', '1.2.3.4.5', '1234'], ['--connect', '127.0.0.1', '65536'], ['--what'], ['--host', '999999999999999999999'], ['--host', '1234', 'extra']]:
    result = subprocess.run([APP, *args], input='', text=True, capture_output=True, timeout=2)
    assert result.returncode == 1, (args, result)
assert subprocess.run([APP], input='', text=True, capture_output=True, timeout=2).returncode == 1

# Two real players: a miss passes the turn, a hit retains it, all ships can sink.
n = port()
shots = 'J 9\nJ 9\ninvalid 0\n' + ''.join(f'{chr(97+r)} {c}\n' for r, c in CELLS)
with game([], '1\n' + str(n) + '\n' + FLEET + shots) as (host, hostlog):
    # Wait for the listening message without consuming a connection.
    deadline = time.monotonic() + 5
    while 'Hosting server' not in output(hostlog):
        assert time.monotonic() < deadline
        time.sleep(.02)
    time.sleep(.05)
    with game([], '2\n127.0.0.1\n' + str(n) + '\n' + FLEET + 'J 9\n') as (client, clientlog):
        host.wait(timeout=10)
        client.wait(timeout=10)
        assert host.returncode == client.returncode == 0
        assert 'You won!' in output(hostlog), output(hostlog)
        assert 'You lost!' in output(clientlog), output(clientlog)
        assert 'Hitstatus: X' in output(hostlog)

# Controlled peer: fragmented readiness/coordinates, edge ship detection, disconnects.
for mode in ('edge', 'vertical', 'invalid', 'partial', 'ready', 'overlap', 'restart', 'repeat'):
    n = port()
    fleet = FLEET
    if mode == 'vertical':
        lines = NUMERIC_FLEET.splitlines()
        fleet = letter_rows('\n'.join('v' if line == 'h' else ' '.join(reversed(line.split())) if ' ' in line else line for line in lines) + '\n')
    if mode == 'overlap':
        fleet = fleet.replace('I 3\n', 'I 0\nI 3\n')
    if mode == 'restart':
        fleet = FLEET[:-2] + 'n\n' + FLEET
    with game(['--host', str(n)], fleet + 'J 9\n') as (host, log):
        with connect(n) as peer:
            assert receive(peer, 5) == b'Ready'
            if mode == 'ready':
                peer.sendall(b'Wrong')
            else:
                for byte in b'Ready':
                    peer.sendall(bytes([byte]))
                    time.sleep(.005)
                assert receive(peer, 2) == b'99'
                peer.sendall(b'X')
                if mode == 'partial':
                    peer.sendall(b'0')
                elif mode == 'invalid':
                    peer.sendall(b'zz')
                elif mode in ('edge', 'vertical', 'repeat'):
                    # Hit the right end first: remaining ship includes column zero.
                    for col, expected in [(3, b'#'), (2, b'#'), (1, b'#'), (0, b'D')]:
                        coordinate = (str(col) + '0' if mode == 'vertical' else '0' + str(col)).encode()
                        peer.sendall(coordinate[:1])
                        time.sleep(.005)
                        peer.sendall(coordinate[1:])
                        assert receive(peer, 1) == expected
                    if mode == 'repeat':
                        peer.sendall(b'00')
        host.wait(timeout=5)
        text = output(log)
        assert host.returncode in (0, 1), text
        if mode in ('partial', 'invalid', 'ready', 'repeat'): assert host.returncode == 1, text
        else: assert 'Opponent disconnected' in text, text
        if mode == 'overlap':
            assert 'Boats should have distance' in text
        if mode == 'restart':
            assert text.count('Fleet setup completed!') == 2, text
# Menu: invalid choices reprompt; quit needs no socket.
menu = subprocess.run([APP], input='wrong\n3\n', text=True, capture_output=True, timeout=2)
assert menu.returncode == 0 and '1. Host a game' in menu.stdout and 'Please choose' in menu.stdout

# Keep stdin open and idle: detect disconnect without another keystroke.
for phase in ('setup', 'turn'):
    n = port()
    with tempfile.TemporaryFile(mode='w+') as log:
        host = subprocess.Popen([APP, '--host', str(n)], stdin=subprocess.PIPE, stdout=log, stderr=log, text=True)
        try:
            with connect(n) as peer:
                if phase == 'turn':
                    host.stdin.write(FLEET)
                    host.stdin.flush()
                    assert receive(peer, 5) == b'Ready'
                    peer.sendall(b'Ready')
                    time.sleep(.05)
                else:
                    time.sleep(.05)
            host.wait(timeout=3)
            assert 'Opponent disconnected. Game ended.' in output(log), output(log)
        finally:
            if host.poll() is None: host.kill()
            host.wait()
            host.stdin.close()

print('Passed: input validation, complete game, turn changes, fragmented messages, edge sinking, overlap, restart, invalid peers and disconnects.')
