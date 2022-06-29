/** Using UTF8 file encoding
  * @file    game_2048.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    26-June-2022
  * @brief   Эта библиотека реализует игру 2048. Данный файл подключает нужные библиотеки, 
  *          содержит необходимые макросы, а также объявления функций */
  
#ifndef __GAME2048_H
#define __GAME2048_H

/*============================== Подключение библиотек ==============================*/
#include "lcd_lib.h"            // Библиотека дисплея LCD12864
#include "ir_remote_control.h"  // Библиотека общения в ИК-диапазоне (приемник-передатчик)
#include <string.h>             // Библиотека для функции memset
#include <stdio.h>              // Библиотека для функции sprinf

/*============================== Макросы ==============================*/
// Размеры поля в пикселях
#define FIELD_WIDTH  73  // Ширина поля
#define FIELD_HEIGHT 32  // Высота поля
#define CELL_WIDTH   18  // Размеры ячейки с цифрами (реальный размер на 3 больше)

// Состояния игры
#define GAME_LOSE     0
#define GAME_WIN      1
#define GAME_CONTINUE 2

// Счёт для победы
#define WIN_NUMBER 2048

/*============================== Структуры и переменные ==============================*/
// Структура для хранения всех чисел поля, а также максимальное число на поле
typedef struct Matrix {
    uint16_t matrix[16];
    
    uint16_t maxNumber;
}Matrix;


/*============================== Функции ==============================*/
static void gameInit(void);

//----Функции записи----
static void writeScore(void);
static void writeNumberToField(uint16_t number, uint8_t row, uint8_t col);
static void writeMatrixToField(void);

// Функции, реализующие механику игры
static uint8_t getAvailableIndex(void);
static void genNewNumber(void);
static uint8_t moveColAndEndCheck(uint8_t isGameEndCheck, uint16_t *value1, uint16_t *value2, uint16_t *value3, uint16_t *value4);
static uint8_t checkGameEnd(void);
static void moveAndDrawMatrix(uint8_t newDirection);

void startGame2048(void);
#endif
