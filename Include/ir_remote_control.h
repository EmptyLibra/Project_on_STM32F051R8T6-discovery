/** Using UTF8 file encoding
  * @file    ir_remote_control.h
  * @author  Shalaev Egor
  * @version V0.0.2
  * @date    17-June-2022
  * @brief   Эта библиотека реализует прием сигналов от ИК-пультов и их передачу по протоколам NEC и Panasonic.
  *          В этом файле перечислены подключаемые библиотеки, пины ИК-приемника и ИК-передатчика,
  *          Характеристики обоих протоколов, некоторые команды, а также объявления основных функций. */
  
#ifndef __IR_REMOTE_CONTROL_H
#define __IR_REMOTE_CONTROL_H

/*============================== Подключение библиотек ==============================*/
#include "main_conf.h"  /* Библиотека с основными настройками для всего проекта */
#include "speaker.h"    /* Библиотека для работы с динамиками */
#include "fast_fifo.h"  /* Библиотека с быстрым fifo */

/*============================== Макросы ==============================*/
/* Пины ИК-приемника и ИК-передатчика */
#define RCC_AHBENR_IR_RECEIVER_EN  RCC_AHBENR_GPIOAEN // Тактирование приемника
#define IR_RECEIVER_PORT           GPIOA              // ИК-порт приемника
#define RCC_AHBENR_IR_TRANSMIT_EN  RCC_AHBENR_GPIOCEN // Тактирование передатчика
#define IR_TRANSMIT_PORT           GPIOC              // ИК-порт передатчика
#define IR_TRANS_PIN               GPIO_Pin_7         // PС7 - ИК-передатчик
#define IR_RECEIV_PIN              GPIO_Pin_8         // PA8 - ИК-приемник
#define IR_RECEIV_PIN_SOURCE       GPIO_PinSource8    // Источник для альтернативной функции

// Включение или выключение ШИМ с частотой 38кГц для ИК-диода
#define IR_PWM_STATE(mode) ((mode) == 1 ? TIM_CCxCmd(TIM3, TIM_Channel_2, ENABLE) : TIM_CCxCmd(TIM3, TIM_Channel_2, DISABLE))

/*----- Характеристики протокола NEC -----
 * Всего бит в протоколе:   32
 * Кол-во адресных бит:     16
 * Кол-во бит кода команды: 16
 * ---------------------------
 * Основные временные интервалы по протоколу:
 * Длительность 1 преабулы:                9000  мкс
 * Длительность 0 преамбулы:               4500  мкс
 * Длительность всей преамбулы:            13500 мкс (в таймере: 1300-1400)
 * Длительность mark 1, mark 0, space 0:   560   мкс
 * Длительность 1 space:                   1690  мкс
 * Длительность лог. единицы:              2250  мкс (в таймере: 220-235)
 * Длительность лог. нуля:                 1120  мкс (в таймере: 105-125)  */
#define IR_PROTOCOL_TYPE_NEC  1     // Тип протокола: NEC
#define NEC_COUNT_BITS        32    // Кол-во битов протокола
#define NEC_HDR_MARK          9000  // Длительность 1 преамбулы (мкс)
#define NEC_HDR_SPACE         4500  // Длительность 0 преамбулы (мкс)
#define NEC_BIT_MARK          560   // Длительность импульса 1 и импульса 0 (мкс)
#define NEC_ONE_SPACE         1690  // Длительность паузы 1 (мкс)
#define NEC_ZERO_SPACE        560   // Длительность паузы нуля (мкс)
#define NEC_RPT_SPACE         2250  // Длительность преамбулы при повторной команде (мкс)

/* Коды кнопок китайского пульта (NEC) (управляющего подсветкой) */
#define IR_BUTTON_TOP      0x00FFA857  // Кнопка - Зеленая стрелка вверх
#define IR_BUTTON_BOTOOM   0x00FFB04F  // Кнопка - DIY2
#define IR_BUTTON_RIGHT    0x00FF48B7  // Кнопка - Синяя стрелка вверх
#define IR_BUTTON_LEFT     0x00FF08F7  // Кнопка - Красная стрелка вниз
#define IR_BUTTON_SELECT   0x00FF10EF  // Кнопка - DIY4
#define IR_BUTTON_BACK     0x00FF50AF  // Кнопка - DIY6
#define IR_BLUE_LED        0x00FFA25D  // Кнопка - Синяя кнопка B
#define IR_GREEN_LED       0x00FF9A65  // Кнопка - зеленая кнопка G

#define IR_SONG_PAUSE_PLAY 0x00FFC837  // Кнопка - QUICK
#define IR_SONG_SW_MT      0x00FF20DF  // Кнопка - JUMP3
#define IR_SONG_PIRATES    0x00FFA05F  // Кнопка - JUMP7
#define IR_SONG_ELISE      0x00FF609F  // Кнопка - FADE3
#define IR_SONG_TETRIS     0x00FFE01F  // Кнопка - FADE7


/*----- Характеристики протокола Panasonic (REC80, JAP) -----
 * Всего бит в протоколе:   48
 * Кол-во адресных бит:     24 (16)
 * Кол-во бит кода команды: 24 (32)
 * ---------------------------
 * Основные временные интервалы по протоколу:
 * Длительность 1 преабулы:                3600 мкс
 * Длительность 0 преамбулы:               1500 мкс
 * Длительность всей преамбулы:            5100 мкс (в таймере: 500-530)
 * Длительность mark 1, mark 0, space 0:   400  мкс
 * Длительность 1 space:                   800  мкс
 * Длительность лог. единицы:              1600 мкс (в таймере: 165-185)
 * Длительность лог. нуля:                 800  мкс (в таймере: 75-95)  */
#define IR_PROTOCOL_TYPE_PANASONIC  2     // Тип протокола: Panasonic
#define PANASONIC_COUNT_BITS        48    // Кол-во битов протокола
#define PANASONIC_HDR_MARK          3600  // Длительность 1 преамбулы (мкс)
#define PANASONIC_HDR_SPACE         1500  // Длительность 0 преамбулы (мкс)
#define PANASONIC_BIT_MARK          400   // Длительность импульса 1 и импульса 0 (мкс)
#define PANASONIC_ONE_SPACE         1200  // Длительность паузы 1 (мкс)
#define PANASONIC_ZERO_SPACE        400   // Длительность паузы нуля (мкс)

/* Коды кнопок с пульта от Panasonic */
#define IR_PANASONIC_BUTTON_TOP         0x400401005253
#define IR_PANASONIC_BUTTON_BOTOOM      0x40040100D2D3 
#define IR_PANASONIC_BUTTON_RIGHT       0x40040100F2F3 
#define IR_PANASONIC_BUTTON_LEFT        0x400401007273 
#define IR_PANASONIC_BUTTON_SELECT      0x400401009293 
#define IR_PANASONIC_BUTTON_BACK        0x400401002B2A 
#define IR_PANASONIC_GREEN_LED          0x400401008E8F
#define IR_PANASONIC_BLUE_LED           0x40040100CECF 

#define IR_PANASONIC_SONG_PLAY          0x400409005059
#define IR_PANASONIC_SONG_PAUSE         0x400409000009
#define IR_PANASONIC_SONG_SW_MT         0x400401008C8D 
#define IR_PANASONIC_SONG_PIRATES       0x400401908D1C 
#define IR_PANASONIC_SONG_ELISE         0x40040190E574 
#define IR_PANASONIC_SONG_TETRIS        0x400401909100


// Неизвестный тип протокола
#define IR_PROTOCOL_TYPE_UNKNOWN  4

// Максимальный временной интервал (в мкс) между двумя спадами
#define MAX_IR_SIGNAL_LEN 1450 // 14500 мкс = 1450 в таймере

// Режимы ИК-приема
#define IR_RECEIVE_TYPE_RUN_COMMAND  0  // Обычный режим с выполнением команд
#define IR_RECEIVE_TYPE_TEST_COMMAND 1  // Тестовый режим с отображением команд

// Отправляем преамбулу или информационные биты
#define IR_SEND_PREAMBULA 0
#define IR_SEND_INF_BITS  1

/*============================== Структуры и переменные ==============================*/
/* Текущий ИК-режим и текущее состояние приема (включено или выключено) */
extern uint8_t irReceiveType, irReceiveState;

// Стурктура для хранения информации о принятом протоколе
typedef struct{
	uint8_t type;             // Тип принятого протокола
	uint8_t bitsCount;        // Количество бит в протоколе
	uint16_t preambuleTime;   // Длина преамбулы в мкс
	uint16_t bitMarkLen;      // Длина единичного импульса (для отправки команды)
	uint8_t irCmdBitCounter;  // Счетчик текущего количества битов в команде
	uint64_t irCommand;       // Сама команда
	
	// Для реализации долгого нажатия на пульте NEC
	uint8_t repeatLastCmd;    // Надо ли повторять последнюю команду
	uint64_t lastNecCommand;  // Буфер для последней команды NEC
	
	uint8_t isFullReceive;    // Принят ли протокол полностью (для Ir-теста)
} IR_ReceiveProt;
extern IR_ReceiveProt irProtocol;

// Структура для отправки ИК-команды
typedef struct {
	uint8_t polatitySignalNow;  // Текущая полярность ИК-сигнала (импульс или его отсутствие)
	uint8_t nextProtocolPart;   // Какая часть протокола отправляется (преамбула или инфформационные биты)
	uint64_t curBitMask;        // Текущая маска для бита, который отправляется (какой бит команды отправляется)
	uint8_t flagTransmitWork;   // Флаг того, что передача ИК-сигнала всё ещё ведётся
} IR_TransmittConf;

/*============================== Функции ==============================*/
void irReseivTransInit(void);
void irProtocolAnalyze(void);

void sendReceiveIrCommad(void);
#endif
