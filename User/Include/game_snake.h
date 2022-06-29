/** Using UTF8 file encoding
  * @file    game_snake.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    25-June-2022
  * @brief   Эта библиотека реализует игру Змейка. Данный файл подключает нужные библиотеки, 
  *          содержит необходимые макросы, а также объявления функций */

#ifndef __GAME_SNAKE_H
#define __GAME_SNAKE_H

/*============================== Подключение библиотек ==============================*/
#include "lcd_lib.h"            // Библиотека дисплея LCD12864
#include "ir_remote_control.h"  // Библиотека общения в ИК-диапазоне (приемник-передатчик)
#include "speaker.h"            // Библиотека динамика для проигрывания музыки
#include <string.h>             // Библиотека для функции memset
#include <stdio.h>              // Библиотека для функции sprinf

/*============================== Макросы ==============================*/
// Стартовая длина змейки (в пикселях)
#define START_SNAKE_LENGTH 8U

// Стартовая строка и столбец змейки (реальные координаты на 1 больше)
#define START_SNAKE_ROW 5U    // Строка: [1, 30]
#define START_SNAKE_COL 5U    // Столбец: [1, 120]

// Вид следующего пикселя перед головой змейки
#define EMPTY_PX 0U           // Пустой пиксель
#define FOOD_PX  1U           // Пиксель еды
#define WALL_OR_BODY_PX 2U    // Пиксель стены или тела змейки

// Состояние змейки
#define SNAKE_STOP     1U     // Змейка врезалась и остановилась
#define SNAKE_NOT_STOP 0U     // Змейка всё ещё движется

// Сложность игры
#define GAME_EASY_MODE 0U     // Простая: Без стен и с проходом за экран
#define GAME_MID_MODE  1U     // Средняя: со стенами по контуру и на поле
#define GAME_HARD_MODE 2U     // Сложная: С тунелями и маленькими проходами

/*============================== Структуры и переменные ==============================*/
// Перечисление с возможными направлениями движения змейки
typedef enum {
    UP,
    LEFT, 
    RIGHT,
    DOWN,
} Direction;

// Структура, отвеающая за один пиксель на поле, с его координатами и навправлением движения
typedef struct DirectPoint {
    int8_t row, col;  // Координаты пикселя
    Direction dir;    // Напрвление движения
} DirectionPoint;

// Структура всей змейки
typedef struct Snake {
    DirectionPoint head;  // Пиксель головы (его координаты и направление)
    DirectionPoint tail;  // Пиксель хвоста (его координаты и направление)
    Direction newDir;     // Следующее направление змейки
    uint16_t length;      // Длина змейки
    uint8_t growCount;    // Количество сегментов, на которые должна вырасти змейка
} Snake;

// Структура с информацией о еде
typedef struct Food{
    uint8_t row, col;     // Координаты еду
    uint8_t isBig;        // Большая ли еда (1) или маленькая (0)
} Food;

/*============================== Функции ==============================*/
// Функции инициализации
static void snakeInit(void);
static void gameInit(void);

// Специальные функции
static void genFood(void);
static Direction getNewTailDir(void);
static uint8_t getNextHeadPx(void);

// Основные функции движения
static void moveSnakeTail(void);
static uint8_t moveSnakeHead(void);
static void snakeMove(void);

// Запуск игры змейка
void startSnakeGame(void);

#endif
