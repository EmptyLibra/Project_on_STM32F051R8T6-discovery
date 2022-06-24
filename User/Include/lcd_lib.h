/**
  * @file    lcd_lib.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    04-June-2022
  * @brief   Библиотека для работы с дисплеем LCD12864B V 2.0 на контроллере ST7920.
  * ==================================
  * Здесь содержатся основные настройки дисплея, такие как тип интерфейса, режим дисплея,
  * пины подключения и прочее, а также объявления основных функций и макросов.
  */
#ifndef __LCD_LIB_H
#define __LCD_LIB_H

/*============================== Подключение библиотек ==============================*/
#include "main_conf.h"     /* Библиотека с основными настройками для всего проекта */
#include "stm32f0xx_spi.h" /* SPL библиотека для работы с SPI */

/*============================== Макросы ==============================*/
/* Выбор интерфейса общения с дисплеем (выбрать только один из макросов) */
//#define INTERFACE_8BIT_PARALLEL   /* 8-ми битный параллельный интерфейс ### PSB - +5V ### */
//#define INTERFACE_4BIT_PARALLEL  /* 4-х битный параллельный интерфейс ### PSB - +5V ### */
#define INTERFACE_SPI            /* Последовательный SPI интерфейс ### PSB - GND ###*/

/* Режим работы дисплея (выбрать только один из макросов) */
#define LCD_GRAPHICAL_MODE  /* Графический режим работы */
//#define LCD_CHARACTER_MODE  /* Символьный режим работы */

/*----- Выбор пинов дисплея в соответствии с выбранным интерфейсом ------*/
#if defined INTERFACE_SPI
/* SPI интерфейс */
	/* Пины управления дисплея */
	#define LCD_RST_PIN   GPIO_Pin_14   /* RST: PB14 - (0 - сброс дисплея) */
	#define LCD_SCK_PIN   GPIO_Pin_13   /* E:   PB13 - SCK SPI  */	
	#define LCD_MOSI_PIN  GPIO_Pin_15   /* R/W: PB15 - MOSI SPI */
	#define LCD_NSS_PIN   GPIO_Pin_12   /* RS:  PB12 - Chip Select (CS, SS) или выбор ведомого */
	
	/* Для настройки альтернативной функции */
	#define LCD_SCK_SOURCE   GPIO_PinSource13
	#define LCD_MOSI_SOURCE  GPIO_PinSource15
	
	/* Установка или сброс пина дисплея */
	#define RST(mode) ((mode) == 1 ? (GPIOB->BSRR = LCD_RST_PIN,0) : (GPIOB->BRR = LCD_RST_PIN,0))
	#define CS(mode)  ((mode) == 1 ? (GPIOB->BSRR = LCD_NSS_PIN,0) : (GPIOB->BRR = LCD_NSS_PIN,0))
	

#elif defined INTERFACE_8BIT_PARALLEL || defined INTERFACE_4BIT_PARALLEL
/* 8-ми или 4-х битный интерфейс */
	/* Пины управления дисплея */
	#define RST_PIN  GPIO_Pin_14     /* RST: PB14  (0 - сброс дисплея) (На дисплее Миландер - RES)     */
	#define E_PIN    GPIO_Pin_13     /* E:   PB13  (включить стробирование)                            */
	#define RW_PIN   GPIO_Pin_15     /* R/W: PB15  (0 - запись, 1- чтение)                             */
	#define RS_PIN   GPIO_Pin_12     /* RS:  PB12  (0 - команда, 1 - данные)(На дисплее Миландер - A0) */

	/* Пины данных */
	#define DB0_PIN  GPIO_Pin_0  /* DB0: PB0 */
	#define DB1_PIN  GPIO_Pin_1  /* DB1: PB1 */
	#define DB2_PIN  GPIO_Pin_2  /* DB2: PB2 */
	#define DB3_PIN  GPIO_Pin_3  /* DB3: PB3  */
	
	#define DB4_PIN  GPIO_Pin_4      /* DB4: PB4  */
	#define DB5_PIN  GPIO_Pin_5      /* DB5: PB5  */
	#define DB6_PIN  GPIO_Pin_6      /* DB6: PB6  */
	#define DB7_PIN  GPIO_Pin_7      /* DB7: PB7  */

	/* Установка или сброс пинов дисплея */
	#define RS(mode)  ((mode) == 1 ? (GPIOB->BSRR = RS_PIN,0) : (GPIOB->BRR = RS_PIN,0))
	#define RW(mode)  ((mode) == 1 ? (GPIOB->BSRR = RW_PIN,0) : (GPIOB->BRR = RW_PIN,0))
	#define E(mode)   ((mode) == 1 ? (GPIOB->BSRR = E_PIN,0)  : (GPIOB->BRR = E_PIN,0))
	#define RST(mode) ((mode) == 1 ? (GPIOB->BSRR = RST_PIN,0)  : (GPIOB->BRR = RST_PIN,0))
	
	#define DB0(mode) ((mode) == 1 ? (GPIOB->BSRR = DB0_PIN,0)  : (GPIOB->BRR = DB0_PIN,0))
	#define DB1(mode) ((mode) == 1 ? (GPIOB->BSRR = DB1_PIN,0)  : (GPIOB->BRR = DB1_PIN,0))
	#define DB2(mode) ((mode) == 1 ? (GPIOB->BSRR = DB2_PIN,0)  : (GPIOB->BRR = DB2_PIN,0))
	#define DB3(mode) ((mode) == 1 ? (GPIOB->BSRR = DB3_PIN,0)  : (GPIOB->BRR = DB3_PIN,0))

	#define DB4(mode) ((mode) == 1 ? (GPIOB->BSRR = DB4_PIN,0)  : (GPIOB->BRR = DB4_PIN,0))
	#define DB5(mode) ((mode) == 1 ? (GPIOB->BSRR = DB5_PIN,0)  : (GPIOB->BRR = DB5_PIN,0))
	#define DB6(mode) ((mode) == 1 ? (GPIOB->BSRR = DB6_PIN,0)  : (GPIOB->BRR = DB6_PIN,0))
	#define DB7(mode) ((mode) == 1 ? (GPIOB->BSRR = DB7_PIN,0)  : (GPIOB->BRR = DB7_PIN,0))
#endif

/*----- Общие команды дисплея (8-ми битный и SPI интерфейс) ------*/
/* (Базовые команды) */
#define CLEAR_DISPLAY        0x01U    /* Очистка DDRAM памяти дисплея и установка счетчика адреса в ноль*/ 
#define FUNCTION_SET_BASE    0x30U    /* Базовый режим дисплея (8-битный интерфейс) */
#define FUNCTION_SET_EXT     0x34U    /* Переход к расширенным инструкциям (8-битный интерфейс) */
#define DISPLAY_ON           0x0CU    /* Включение дисплея (курсор и его мигание выключено) */
#define ENTRY_MODE_SET_RIGHT 0x06U    /* Сдвиг курсора и дисплея будет происходить вправа (для DRAM) */
#define ENTRY_MODE_SET_LEFT  0x07U    /* Сдвиг курсора и дисплея будет происходить влево (для DRAM) */

/* Расширенные команды графического режима */
#define EX_GRAPHIC_DISPLAY_ON      0x36U  /* Включаем графический режим и переходим к расширенным инструкциям */
#define EX_GR_GO_TO_BASIC_FUNC_SET 0x32U  /* Вовзращаемся к базовым инструкциям (в графическом режиме) */
#define EX_SET_GDRAM_ADDR          0x80U  /* Установка адреса графического ram (часть команды) */
#define EX_STAND_BY_MODE           0x01U  /* Установка режима ожидания (выход с помощью любой другой инструкцией) */

/* Базовые команды символьного режима */
#define CURSOR_ON           0x0AU  /* Включение курсора */
#define CURSOR_BLINK_ON     0x09U  /* Включение мигание курсора */
#define CURSOR_HOME         0x02U  /* Установка счетчика адреса (AC) в 0 и возвращение курсора в начало */
#define CURSOR_SHIFT_LEFT   0x10U  /* Курсор сдвигается влево на 1 */
#define CURSOR_SHIFT_RIGHT  0x14U  /* Курсор сдвигается вправо на 1 */
#define DISPLAY_SHIFT_LEFT  0x18U  /* Дисплей сдвигается влево на 1 вместе с курсором */
#define DISPLAY_SHIFT_RIGHT 0x1CU  /* Дисплей сдвигается вправо на 1 вместе с курсором */
#define CGRAM_ADDR_EN       0x02U  /* Набор адресов CGRAM включен */

/* Расширенные команды символьного режима */
#define EX_SY_GO_TO_BASIC_FUNC_SET 0x30U  /* Вовзращаемся к базовым инструкциям (в символьном режиме) */
#define IRAM_ADDR_EN               0x02U  /* Набор адресов IRAM включен */
#define VERTICAL_SCROLL_EN         0x03U  /* Включен набор адресов вертикальной прокрутки */
#define REVERSE_LINE1              0x04U  /* Первая строка инвертированная (первое применение команды) не инвертированная (второе применение) */
#define REVERSE_LINE2              0x05U  /* Вторая строка инвертированная (первое применение команды) не инвертированная (второе применение) */
#define REVERSE_LINE3              0x06U  /* Третья строка инвертированная (первое применение команды) не инвертированная (второе применение) */
#define REVERSE_LINE4              0x07U  /* Четвертая строка инвертированная (первое применение команды) не инвертированная (второе применение) */

/*------ Для 4-х битного интерфейса (графический режим) ------*/
#define I4_FUNCTION_SET_BASE    0x20U    /* Базовый режим дисплея */


/* Номера страниц дисплея */
#define PAGE_1 0x00U
#define PAGE_2 0x01U
#define PAGE_3 0x02U
#define PAGE_4 0x03U
#define PAGE_5 0x04U
#define PAGE_6 0x05U
#define PAGE_7 0x06U
#define PAGE_8 0x07U

/* Ширина и высота дисплея в пикселях */
#define DISPLAY_WIDTH  128U
#define DISPLAY_HEIGHT 64U

/* Размер буфера дисплея в байтах*/
#define LCD_BUFFER_LENGTH (DISPLAY_HEIGHT*DISPLAY_WIDTH)/8

/* Направление сдвига и цикличность сдвига для бегущей строки (для функции LCD_BufferShiftPage) */
#define RIGHT_SHIFT      0
#define LEFT_SHIFT       1
#define CYCLIC_SHIFT     0
#define NOT_CYCLIC_SHIFT 1

/* Очистить или заполнить дисплей (для функции LCD_ClearOrFillDisplay) */
#define CLEAR 0
#define FILL 1

/* Выбор команды или байта данных в функции LCD_SendByteData */
#define LCD_COMMAND 0
#define LCD_DATA    1

/*============================== Структуры и переменные ==============================*/
/* Структура с основными переменными и функциями для работы с дисплеем.
 * Необходима для абстракции от конкретных функций того или иного дисплея */
typedef struct LCD_STRUCT{
	uint16_t byteIndex;   /* Текущая позиция байта, который надо отрисовать */
	
	/* Функции инициализации дисплея */
	void (*portsInit) (void);                                         /* Инициализация всех пинов дисплея  */
	void (*init) (void);                                              /* Инициализация и включение дисплея */
	
	/* Функции для работы с буфером дисплея */
	void (*writeStringToBuffer) (const char* str);                    /* Запись любой строки в буфер дисплея */
	void (*displayFullUpdate)   (void);                               /* Перерисовка всего содержимого буфера на экран */
	
	/* Прочии функции */
	void (*sendByteData)        (uint8_t dataOrCmd, uint8_t byte);    /* Отправка команды или байта данных */
	void (*clearOrFillDisplay)  (uint8_t isFILL);                     /* Очистка или заполнение всего дисплея */
} LCD_STRUCT;
extern LCD_STRUCT lcdStruct;

extern uint8_t displayBuffer[LCD_BUFFER_LENGTH];  /* Буфер байтов всего дисплея */

/*============================== Функции ==============================*/
/*----- Функции отправки данных или команд -----------------*/
#ifdef INTERFACE_8BIT_PARALLEL
	uint8_t LCD_ReadBsyFlagAndAdressCounter(void)         
#endif
void LCD_SendByteData(uint8_t dataOrCmd, uint8_t byte);
void LCD_SetAddr(uint8_t vertAddr, uint8_t horizAddr);

/*----- Функции инициализации -----------------*/
void LCD_PortsInit(void);
void LCD_Init(void);

/*------------- Функции для работы с буфером дисплея -----------------*/
uint8_t LCD_GetHorizontalByte(uint8_t Row, uint8_t Col);
void LCD_SetBufferCursor(uint8_t xCoord, uint8_t page);
void LCD_WriteStringToBuffer(const char* str);
void LCD_writeHorStringToBuffer(const char* str);
void LCD_BufferShiftPage(uint8_t page, uint8_t direct, uint8_t isCyclic);
void LCD_ClearBufferPage(uint8_t page);
void LCD_DrawPageFromBuffer(uint8_t page);
void LCD_DisplayFullUpdate(void);

//----draw functions-----------------------------------------------------
void LCD_DrawAllLibSymbolls(void);
void LCD_ClearOrFillDisplay(uint8_t isClear);
void LCD_RunLine(const char *str, uint8_t charCount, uint8_t page, uint8_t direct, uint8_t isCyclic);

#endif
