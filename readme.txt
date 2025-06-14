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
* RELOAD CONFIG EACH TIME INITPLAY IS CALLED, OTHERWISE
  YOU CANNOT LOAD GAMES WITH OTHER DIMENTIONS THAT THOSE
  THE CONFIG FILE HAS. IDK IF THERE IS ANOTHER WAY TO SOLVE IT
* WHEN YOU RUN IT IN
* ERROR WITH OPENING LOG: ALL REVEALED. DANGER: SECOND EXECUTION AND 
  BEFORE WORKS, ONLY THE FIRST DOES NOT

MAIN TODOS:
* FILE SAVE STATS LAST 10 GAMES (REQ) ON SCREEN!!!
* CHEAT (REQ)
* AUTOREVEAL BOMB END GAME
* CHANGE DIM IN SDL (how?) (just mesh dim)
* SOME OPTIONS FROM SDL SELECTIONS
* WIN SIGN

* HOVER LIGHT
* BETTER TEXTURES / VIEW -> PERSONALIZE


DEPENDENCIES:
gamestate -> none
graphics -> none
minesweeper -> gamestate
configs -> gamestate
main -> all
UPDATE THIS

TO ASK
- files .h .a -> .a necessary, include them
- files with no only-type-register. TODO: Save the whole strucure
- time vars are too long, it's normal? -> YES, time() works with that, avoid compiling risks

