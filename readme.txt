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

MAIN TODOS:
LOG FILE (REQ-4)
AUTOFLAG
USER ENTRY (REQ)
SHOW MINES REST (REQ)
SAVE GAMESTATE TO PLAY LATER (REQ)
FILE SAVE STATS LAST 10 GAMES (REQ)
CHEAT (REQ)

TIME REC, SHOW AND SAVE
VIEW GAMES (REPLAY)
AUTOREVEAL BOMB END GAME
CHANGE DIM IN SDL

BETTER TEXTURES / VIEW -> PERSONALIZE