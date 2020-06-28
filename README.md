# Othello
I2P(II) Mini Project 3

Here provides some files that simulate "Computer" side when playing othello game.

Different versions represent different methods that "Computer" uses.

Note that this file should be driven by "main.cpp" file, but I did not uplaod it.

------------------

Version 1: player_random.cpp

"Computer" randomly takes a valid place and put the disc on it, which is the very initial version.

------------------

Version 2: player_moves.cpp

"Computer" calculates the next valid moves of opponent and itself. Then, it will try to make opponent's next valid moves less and make its next valid moves more.

However, there are still some problems that need to be solved.

-------------------

Version 3: player_board.cpp

"Computer" calculates the "score" of current state (or you can say "current board"), where the "score" is decided by whether it has occupied the corners and the "star places" of the board or not.

Similar to Version 2, there are still some problems that need to be solved.
