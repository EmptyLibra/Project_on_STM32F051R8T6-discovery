#ifndef TETRIS_HEADER
#define TETRIS_HEADER

#include "lcd_lib.h"
#include "speaker.h"
#include <string.h> // for memset() and memcpy()
#include <stdio.h>  // for sprintf()
#define FIGURE_COUNT sizeof(FIGURES)/8

// game event
#define GAME_LOSE 0
#define GAME_WIN 1
#define GAME_CONTINUE 2

#define TETRIS_FIELD_WIDTH 20
//=================Variables====================================================


//=================Functions====================================================
static void gameInit(void);

//---other functions----------------------
void genNextFigType(void);
void writeFigToField(void);
uint8_t isLost(void);
void shiftAllDown(uint16_t lineNumber);
void checkAndShiftLines(void);
void updateScoreAndFigure(void);

//---move functions----------------
void rotate(void);
void shiftLeft(void);
void shiftRight(void);
uint8_t shiftDown(void);

//---Main function----------------
void startTetrisGame(void);
#endif
