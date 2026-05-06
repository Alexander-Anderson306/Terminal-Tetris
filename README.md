# Terminal Tetris
Tetris written entirly in C for the terminal.

---

## Requirements:
- The user must be using a UNIX-based machine.
- Terminal Tetris will not work on Windows.
- NVIDIA GeForce RTX 5090  
  *JK JK 😂*
---

## How to play:
Clone the repository
Run make and play the version you wish to play! Its as easy as that! 

---

## Controles:
- a: move piece left.
- d: move piece right.
- s: lower piece to bottom of the board.
- q: rotate piece left.
- e: rotate piece right.
- esc: quit

---

## Score:
- 1 Row: 100 points
- 2 Rows: 250 points
- 3 Rows: 500 points
- Tetris: 1000 points

---

## Levels:
# Standard:
- 0-999 points: Fall rate of one block per 1.5 seconds.
- 1000-2499: Fall rate of one block per 1 seconds.
- 2500-5999: Fall rate of one block per 0.8 seconds.
- 6000-9999: Fall rate of one block per 0.6 seconds.
- 10000-19999: Fall rate of one block per 0.5 seconds.
- 20000 and beyond: Fall rate of one block per 0.4 seconds.

# Hard Mode:
- 0-999 points: Fall rate of one block per 0.8 seconds.
- 1000-2499: Fall rate of one block per 0.6 seconds.
- 2500-5999: Fall rate of one block per 0.5 seconds.
- 6000-9999: Fall rate of one block per 0.4 seconds.
- 10000-19999: Fall rate of one block per 0.3 seconds.
- 20000 and beyond: Fall rate of one block per 0.2 seconds (good luck).


## Don't like the scoring or the gravity?
Go ahead and change the source code!
The gravity frame rates are found at the top of game.c.
The level cut offs are found at the bottem of game.c in the update_fall_tick_rate function.
