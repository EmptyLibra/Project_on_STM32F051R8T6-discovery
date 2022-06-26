/** Using UTF8 file encoding
  * @file    game_tetris.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    26-June-2022
  * @brief   Эта библиотека реализует игру Тетрис. Данный файл подключает нужные библиотеки, 
  *          содержит необходимые макросы, а также объявления функций */
#ifndef __GAME_TETRIS_H
#define __GAME_TETRIS_H

/*============================== Подключение библиотек ==============================*/
#include "lcd_lib.h"            // Библиотека дисплея LCD12864
#include "ir_remote_control.h"  // Библиотека общения в ИК-диапазоне (приемник-передатчик)
#include "speaker.h"            // Библиотека динамика для проигрывания музыки
#include <string.h>             // Библиотека для функции memset
#include <stdio.h>              // Библиотека для функции sprinf

/*============================== Макросы ==============================*/
// Общее количество различных фигур
#define FIGURE_COUNT sizeof(FIGURES)/8

// Состояния игры
#define GAME_LOSE     0
#define GAME_WIN      1
#define GAME_CONTINUE 2

// Ширина поля для тетриса
#define TETRIS_FIELD_WIDTH 20

/*============================== Функции ==============================*/
static void gameInit(void);

static void genNextFigType(void);
static void writeFigToField(void);
static uint8_t isLost(void);
static void shiftAllDown(uint16_t lineNumber);
static void checkAndShiftLines(void);
static void updateScoreAndFigure(void);

//---Функции движения фигуры----------------
static void rotate(void);
static void shiftLeft(void);
static void shiftRight(void);
static uint8_t shiftDown(void);

void startTetrisGame(void);
#endif
