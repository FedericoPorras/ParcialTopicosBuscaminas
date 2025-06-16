SOME DATA:

The window max dim allowed is 1366x768
And lets use 2 extra spaces for padding
768 = MAX_DIM_CELD * ( MAX_DIM(cols or rows) + 2 )
MAX_DIM_CELD = 768 / ( MAX_DIM(cols or rows) + 2 )

(768 / MAX_DIM_CELD) - 2 = MAX_DIM

Lets use some square grids as standarts, and calc there MAX_DIM_CELD
grid  -> MAX_DIM_CELD
40x40 -> 17 (use 16)
32x32 -> 22
24x24 -> 29
18x18 -> 38
15x15 -> 48
10x10 -> 59
8x8   -> 76

The grid will be in the coords
(32;32) - (736;736)
with the padding, it finally are
(0;0) - (768;768)

So, the menu will be in the coords
(0;768) - (768;1366)  [space of 768x598]

TOFIX:
* ERROR WITH OPENING LOG: ALL REVEALED. DANGER: SECOND EXECUTION AND 
  BEFORE WORKS, ONLY THE FIRST DOES NOT
* SOMETIMES MINES PENDING SHOWS WRONG NEGATIVES

MAIN TODOS:
* CHEAT (REQ)

* DEATH AND PETARDO SOUND
* WHEN LOADS A BIN FILE TRY LOG FILE TOO
* SHOW GAMES AVAILABLE TO LOAD
* AUTOREVEAL BOMB END GAME

* HOVER LIGHT (BUTTONS)
* BETTER TEXTURES / VIEW -> PERSONALIZE


DEPENDENCIES:
gamestate -> none
graphics -> none
minesweeper -> gamestate
configs -> gamestate
main -> all
UPDATE THIS

REMEMBER:
If you do not end your game, the time is running

TO ASK
- files .h .a -> .a necessary, include them
- files with no only-type-register. TODO: Save the whole strucure
- time vars are too long, it's normal? -> YES, time() works with that, avoid compiling risks