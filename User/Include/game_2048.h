#ifndef GAME2048_HEADER
#define GAME2048_HEADER

#include "lcd_lib.h"
#include <string.h>  // for memset
#include <stdio.h>  // for sprintf

//---------------------------------Defines--------------------------------------
#define FIELD_WIDTH  73
#define FIELD_HEIGHT 32
#define CELL_WIDTH 18   // real cell width = 21

// game event
#define GAME_LOSE 0
#define GAME_WIN 1
#define GAME_CONTINUE 2

// game settings
#define WIN_NUMBER 2048

//-------------------------------Game structers---------------------------------
typedef struct Matrix{
    uint16_t matrix[16];
    
    uint16_t maxNumber;
}Matrix;


//-------------------------------Functions--------------------------------------
static void gameInit(void);                                                      // clear all variables, draw start field

//----write functions----
static void writeScore(void);                                                    // write new score to buffer (not yet draw)
static void writeNumberToField(uint16_t number, uint8_t row, uint8_t col);       // splits number into digits and write to buffer
static void writeMatrixToField(void);                                            // write all matrix to display buffer

// functions for game mechanics
static uint8_t getAvailableIndex(void);                                          // get random free index from matrix
static void genNewNumber(void);

// shifts the desired col of data or check one col for game end
static uint8_t moveColAndEndCheck(uint8_t isGameEndCheck, uint16_t *value1, uint16_t *value2, uint16_t *value3, uint16_t *value4);
static uint8_t checkGameEnd(void);                                               // Win if maxNumber = WIN_NUMBER else lose or continue
static void moveAndDrawMatrix(uint8_t newDirection);                             // Move matrix, generate new number and draw all

void startGame2048(void);
#endif
