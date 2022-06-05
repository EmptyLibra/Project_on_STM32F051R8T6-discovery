#ifndef SNAKE_HEADER
#define SNAKE_HEADER

#include "lcd_lib.h"
#include <string.h> // for memset()
#include <stdio.h>  // for sprintf()
#include "stdlib.h" // for abs()
#include "speaker.h"

#define START_SNAKE_LENGTH 8
// real row and col one bigger
#define START_SNAKE_COL 5     // [1, 120]
#define START_SNAKE_ROW 5     // [1, 30]

#define EMPTY_PX 0
#define FOOD_PX 1
#define WALL_OR_BODY_PX 2

#define SNAKE_STOP 1
#define SNAKE_NOT_STOP 0

#define GAME_EASY_MODE 0
#define GAME_MID_MODE 1
#define GAME_HARD_MODE 2

//-----variables-------------------------------------------------------------------------------------
typedef enum {
    UP,
    LEFT, 
    RIGHT,
    DOWN,
} Direction;

typedef struct DirectPoint {
    int8_t row, col;
    Direction dir;
} DirectionPoint;

typedef struct Snake {
    DirectionPoint head;
    DirectionPoint tail;
    Direction newDir;
    uint16_t length;
    uint8_t growCount;
} Snake;

typedef struct Food{
    uint8_t row, col;
    uint8_t isBig;
} Food;
//
//--init function-------------------------------
void snakeInit(void);
void gameInit(void);
//
//------special functions-------------------------------------------------------------
void genFood(void);
Direction getNewTailDir(void);
uint8_t getNextHeadPx(void);
//
//---main functions--------------------
void moveSnakeTail(void);
uint8_t moveSnakeHead(void);
void snakeMove(void);

void startSnakeGame(void);
#endif
