# Sea Battle

A small two-player Battleship game for the Linux terminal, written in C++17.
One player hosts a TCP connection and the other connects. Both place their
fleets, then take turns firing at coordinates on a 10 × 10 board.

The networking code uses POSIX sockets and is included in the repository;
there are no external libraries or submodules.

## Build

You need Linux, a C++17 compiler, and GNU Make. On Ubuntu or Debian:

```sh
sudo apt install build-essential
make
```

Run `./SeaBattle` directly. Windows is not currently supported.

## Play

Open two terminals for a local game. Start the host first:

```sh
./SeaBattle --host 5000
```

In the second terminal:

```sh
./SeaBattle --connect 127.0.0.1 5000
```

For two computers on the same network, replace `127.0.0.1` with the host's
IPv4 address and allow the chosen TCP port through the host's firewall.
The host listens on all IPv4 interfaces. Both players should run the same
version. Running without arguments opens a menu with **Host a game**, **Join a game**,
and **Quit**. Hosting prompts for a port; joining prompts for an IPv4 address
and port. Short options `-h` and `-c` are also accepted.

Use a terminal at least 84 columns wide. Coordinates are **row column**: rows **A–J** and columns **0–9**,
for example `B 4`. Lowercase row letters work too. Place ships in descending size order: one length-4 ship, two
length-3 ships, three length-2 ships, and four single-cell ships. Enter `h` or
`v` for orientation, then the coordinates of the ship's top-left cell.
Ships must have at least one empty cell between them, including diagonally.
Declining the completed layout clears it and lets you start again.

The host fires first. A hit keeps your turn; a miss passes it to the other
player. Sinking a ship marks the surrounding water automatically. Destroy
all 20 opposing ship cells to win. Previously fired or automatically marked
coordinates cannot be selected again.

| Symbol | Meaning |
| --- | --- |
| `#` | Ship during setup; a hit on the opponent's board |
| `*` | A hit on your fleet |
| `D` | The final hit that sank an opposing ship |
| `X` | Miss or water surrounding a sunk ship |

Your ship layout is hidden after setup; incoming hits remain visible.
Press Ctrl+C to quit. Closing input also exits the game. If the opponent closes their connection,
the game displays a disconnect message, including while you are placing
ships or waiting to enter your next shot.

## Tests

Python 3 is needed only for tests:

```sh
make test
```

The tests run a complete host/client game over loopback and check input
validation, turn changes, edge ships, overlapping placement, fleet resets,
fragmented TCP messages, invalid peer messages, and disconnects.

## Scope

This is a small learning project for friendly local/network games. It has no
AI opponent, matchmaking, saved games, reconnection, or connection timeout.
Connections are blocking and unencrypted. The opponent reports the outcome
of your shots, so the protocol does not prevent cheating.

Licensed under the [MIT License](LICENSE).

## Watch a scripted game

```sh
python3 scripts/play_scenario.py
```

This opens two `xterm` windows and drives the real game through its prompts,
pausing half a second before each input. The host places horizontal ships;
the client places vertical ships. The host misses first, the client hits twice
and then misses, and the host sinks the client's entire fleet. The host wins.
Both windows stay open afterward so you can inspect the final boards.

You can change the pace with `--delay 1`, or run each side in your own terminals
(start the host first):

```sh
python3 scripts/play_scenario.py --side host --port 5000
python3 scripts/play_scenario.py --side client --port 5000
```

For an automatic run without windows:

```sh
python3 scripts/play_scenario.py --headless --delay 0
```

Automatic runs choose a free local port. Each side saves its transcript in
`build-scenario/host.log` or `build-scenario/client.log`. A headless run also
checks that the host wins and the client loses. The layouts and shots are
listed at the top of the script if you want to change the scenario.
