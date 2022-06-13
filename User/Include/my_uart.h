/**
  * @file    my_uart.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    05-June-2022
  * @brief   Заголовочный файл библиотеки для работы с UART */
  
#ifndef __MY_UART_H
#define __MY_UART_H

/*============================== Подключение библиотек ==============================*/
#include "lcd_lib.h"             /* Библиотека дисплея LCD12864 */
#include "infrared_receiver.h"   /* Библиотека общения в ИК-диапазоне (для улавливания нажатия кнопок) */
#include "my_fifo.h"             /* Библиотека, в которой содержится логика работы FIFO */

/*============================== Макросы ==============================*/
/* Настройки порта UART1 */
#define RCC_AHBENR_UART_PORT_EN  RCC_AHBENR_GPIOAEN   /* Тактирование порта А */
#define UART1_PORT               GPIOA                /* Порт UART */
#define UART1_TX_PIN             GPIO_Pin_9           /* PA9 - TX (USART1) */
#define UART1_RX_PIN             GPIO_Pin_10          /* PA10 - RX (USART1) */
#define UART1_TX_SOURCE          GPIO_PinSource9      /* Источник альтернативной функции TX */
#define UART1_RX_SOURCE          GPIO_PinSource10     /* Источник альтернативной функции RX */

/* Отправка и прием данных по UART */
#define UART_SEND_DATA(data) USART_SendData(USART1, data)
#define UART_RECEIVE_DATA() ((uint8_t) USART_ReceiveData(USART1))

/* Bluetooth команды по UART */
#define BT_COMMAND_BUTTON_TOP     0x0100  /* Нажатие кнопки вверх */
#define BT_COMMAND_BUTTON_BOTTOM  0x8000  /* Нажатие кнопки вниз */
#define BT_COMMAND_BUTTON_RIGHT   0x0020  /* Нажатие кнопки вправо */
#define BT_COMMAND_BUTTON_LEFT    0x0400  /* Нажатие кнопки влево */
#define BT_COMMAND_BUTTON_OK      0x0200  /* Нажатие кнопки выбора */
#define BT_COMMAND_BUTTON_BACK    0x0800  /* Нажатие кнопки назад */

#define BT_COMMAND_SONG_PAUSE     0x8001  /* Остановка музыки */
#define BT_COMMAND_SONG_PLAY      0x8002  /* Воспроизведение музыки */
#define BT_COMMAND_SONG_ELISE     0x8003  /* Запуск песни К Элизе */
#define BT_COMMAND_SONG_SW_MT     0x8004  /* Запуск главной темы звездных войн */
#define BT_COMMAND_SONG_TETRIS    0x8005  /* Запуск мелодии для тетриса */

#define BT_COMMAND_POWER_ON_LEDD  0x800A  /* Мигание обоих светодиодов */

/*============================== Структуры и переменные ==============================*/
/* Структура FIFO для хранения данных с UART (размер 256 байт) */
FIFO_STRUCT_CREATE(256);
extern MYFIFO my_fifo;

/*============================== Функции ==============================*/
void uartPinsInit(void);

#endif
