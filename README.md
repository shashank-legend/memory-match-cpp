# Memory Match 🃏

A terminal memory-matching card game written in modern C++17, built as an
object-oriented programming project.

Flip two cards per turn. If they match, you score a point **and keep your turn**.
If they don't, the cards are hidden again and the next player goes. The player
with the most pairs when the board is cleared wins.

```
  MEMORY MATCH  |  Pairs left: 6

  +------+------+------+------+
  |  🍩  |  02  |  🍩  |  04  |
  +------+------+------+------+
  |  05  |  🚀  |  07  |  08  |
  +------+------+------+------+
  |  09  |  10  |  11  |  🚀  |
  +------+------+------+------+
  |  13  |  14  |  15  |  16  |
  +------+------+------+------+

  Scores: Alice 2  |  Bob 0
  Turn:   Alice
```

## Features

- 1–8 players with turn rotation and a final scoreboard (pairs, attempts, accuracy)
- Three difficulty levels (`easy` 2×4, `medium` 4×4, `hard` 6×6) or custom board sizes
- Emoji cards, or `--ascii` letters for terminals without emoji support
- Colour-coded cards: hidden, face up and matched
- Robust input handling: invalid numbers, already-flipped cards and `q` to quit at any time
- Reproducible boards with `--seed`
- Unit and full-game tests with no external dependencies, run in GitHub Actions CI

## Build & run

Requires a C++17 compiler (g++ or clang++) and `make`.

```bash
make          # builds ./memory-match
./memory-match
make test     # runs the test suite
make clean
```

### Options

```
-l, --level <easy|medium|hard>  Board size: 2x4, 4x4 (default) or 6x6
-r, --rows <n>                  Custom number of rows
-c, --cols <n>                  Custom number of columns
-p, --player <name>             Add a player (repeatable; skips the prompt)
-s, --seed <n>                  Shuffle seed, for a reproducible board
    --ascii                     Use letters instead of emoji
    --no-color                  Disable ANSI colors
    --no-clear                  Don't clear the screen between turns
-h, --help                      Show help
```

Example: a two-player game on the hard board:

```bash
./memory-match --level hard -p Alice -p Bob
```

## Project structure

```
include/          Public headers
  Card.h          A single card: symbol, face-up and matched state
  Player.h        Name, score, attempts, accuracy
  Board.h         Shuffled grid of card pairs and its rendering
  Game.h          Turn loop, input validation, scoreboard
  InputUtils.h    Parsing helpers
src/              Implementations + main.cpp (command-line interface)
tests/            Self-contained test runner
```

### OOP design

| Class    | Responsibility |
|----------|----------------|
| `Card`   | Encapsulates its symbol and state; knows whether it matches another card |
| `Player` | Tracks its own score and attempts and computes accuracy |
| `Board`  | Owns the cards, shuffles pairs with a seeded RNG, validates positions, renders itself |
| `Game`   | Coordinates players and the board; reads from any `std::istream` and writes to any `std::ostream`, so full games can be tested with scripted input |

Everything lives in the `memory` namespace, with no global state and no raw `new`/`delete`.
