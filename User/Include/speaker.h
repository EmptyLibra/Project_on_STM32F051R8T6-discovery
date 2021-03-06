/** Using UTF8 file encoding
  * @file    speaker.h
  * @author  Shalaev Egor
  * @version V0.0.2
  * @date    23-June-2022
  * @brief   Эта библиотека реализует проигрывание мелодий на динамиках, с помощью таймеров в режиме ШИМ.
  *          Здесь содежатся подключаемые библиотеки, ноты всех основных октав в виде макросов, прочие макросы,
  *          а также объявления структур, перерменных и функций. 
  */

#ifndef __SPEAKER_H
#define __SPEAKER_H

/*============================== Подключение библиотек ==============================*/
#include "lcd_lib.h"        // Библиотека дисплея LCD12864
#include <string.h>         // Библиотека для функции memset

/*============================== Макросы ==============================*/
/*------------ Все ноты (по научной нотации)---------------------*/
// Частоты нот разбиты по октавам и указаны в герцах
/*---Субконтроктава---*/
#define C0   16   // Do
#define С0_D 17   // Do#
#define D0_B 17   // Re_b
#define D0   18   // Re
#define D0_D 19   // Re#
#define E0_B 19   // Mi_b
#define E0   20   // Mi
#define F0   21   // Fa
#define F0_D 23   // Fa#
#define G0_B 23   // Sol_b
#define G0   24   // Sol
#define G0_D 25   // Sol#
#define A0_B 25   // La_b
#define A0   27   // La
#define A0_D 29   // La#
#define B0_B 29   // Si_b
#define B0   30   // Si

/*---Контроктава---*/
#define C1   32   // Do
#define С1_D 34   // Do#
#define D1_B 34   // Re_b
#define D1   36   // Re
#define D1_D 38   // Re#
#define E1_B 38   // Mi_b
#define E1   41   // Mi
#define F1   43   // Fa
#define F1_D 46   // Fa#
#define G1_B 46   // Sol_b
#define G1   49   // Sol
#define G1_D 51   // Sol#
#define A1_B 51   // La_b
#define A1   55   // La
#define A1_D 58   // La#
#define B1_B 58   // Si_b
#define B1   61   // Si

/*---Большая октава---*/
#define C2   65   // Do
#define С2_D 69   // Do#
#define D2_B 69   // Re_b
#define D2   73   // Re
#define D2_D 77   // Re#
#define E2_B 77   // Mi_b
#define E2   82   // Mi
#define F2   87   // Fa
#define F2_D 92   // Fa#
#define G2_B 92   // Sol_b
#define G2   98   // Sol
#define G2_D 103  // Sol#
#define A2_B 103  // La_b
#define A2   110  // La
#define A2_D 116  // La#
#define B2_B 116  // Si_b
#define B2   123  // Si

/*---Малая октава---*/
#define C3   130  // Do
#define С3_D 138  // Do#
#define D3_B 138  // Re_b
#define D3   146  // Re
#define D3_D 155  // Re#
#define E3_B 155  // Mi_b
#define E3   164  // Mi
#define F3   174  // Fa
#define F3_D 188  // Fa#
#define G3_B 188  // Sol_b
#define G3   196  // Sol
#define G3_D 207  // Sol#
#define A3_B 207  // La_b
#define A3   220  // La
#define A3_D 233  // La#
#define B3_B 233  // Si_b
#define B3   246  // Si

/*---Первая октава---*/
#define C4   261  // Do
#define С4_D 277  // Do#
#define D4_B 277  // Re_b
#define D4   293  // Re
#define D4_D 311  // Re#
#define E4_B 311  // Mi_b
#define E4   329  // Mi
#define F4   348  // Fa
#define F4_D 370  // Fa#
#define G4_B 370  // Sol_b
#define G4   392  // Sol
#define G4_D 415  // Sol#
#define A4_B 415  // La_b
#define A4   440  // La
#define A4_D 466  // La#
#define B4_B 466  // Si_b
#define B4   493  // Si

/*---Вторая октава---*/
#define C5   523  // Do
#define С5_D 554  // Do#
#define D5_B 554  // Re_b
#define D5   587  // Re
#define D5_D 622  // Re#
#define E5_B 622  // Mi_b
#define E5   659  // Mi
#define F5   698  // Fa
#define F5_D 740  // Fa#
#define G5_B 740  // Sol_b
#define G5   784  // Sol
#define G5_D 830  // Sol#
#define A5_B 830  // La_b
#define A5   880  // La
#define A5_D 932  // La#
#define B5_B 932  // Si_b
#define B5   987  // Si

/*---Третья октава---*/
#define C6   1046 // Do
#define С6_D 1108 // Do#
#define D6_B 1108 // Re_b
#define D6   1174 // Re
#define D6_D 1244 // Re#
#define E6_B 1244 // Mi_b
#define E6   1318 // Mi
#define F6   1397 // Fa
#define F6_D 1480 // Fa#
#define G6_B 1480 // Sol_b
#define G6   1568 // Sol
#define G6_D 1568 // Sol#
#define A6_B 1661 // La_b
#define A6   1760 // La
#define A6_D 1864 // La#
#define B6_B 1864 // Si_b
#define B6   1975 // Si

/*---Четвертая октава---*/
#define C7   2093 // Do
#define С7_D 2217 // Do#
#define D7_B 2217 // Re_b
#define D7   2393 // Re
#define D7_D 2489 // Re#
#define E7_B 2489 // Mi_b
#define E7   2637 // Mi
#define F7   2793 // Fa
#define F7_D 2960 // Fa#
#define G7_B 2960 // Sol_b
#define G7   3136 // Sol
#define G7_D 3322 // Sol#
#define A7_B 3322 // La_b
#define A7   3520 // La
#define A7_D 3729 // La#
#define B7_B 3729 // Si_b
#define B7   3951 // Si

/*---Пятая октава---*/
#define C8   4186 // Do
#define С8_D 4435 // Do#
#define D8_B 4435 // Re_b
#define D8   4698 // Re
#define D8_D 4978 // Re#
#define E8_B 4978 // Mi_b
#define E8   5274 // Mi
#define F8   5587 // Fa
#define F8_D 5919 // Fa#
#define G8_B 5919 // Sol_b
#define G8   6272 // Sol
#define G8_D 6645 // Sol#
#define A8_B 6645 // La_b
#define A8   7040 // La
#define A8_D 7458 // La#
#define B8_B 7458 // Si_b
#define B8   7902 // Si

#define P    10   // Пауза

//--------Прочие макросы----------------------
#define LOOPS_PAUSE 0xFFFF        // пауза между циклами, в мкс
#define NOTES_PAUSE 1000          // пауза между нотами, в мкс
#define SPEAKER_MENU_COUNT   sizeof(SPEAKER_MENU_ITEMS)/sizeof(SPEAKER_MENU_ITEMS[0])  // Кол-во элементов в меню

// Порт и пины динамиков
#define RCC_AHBENR_SPEAKEREN   RCC_AHBENR_GPIOBEN  // Тактирование нужного порта
#define SPEAKER_PORT           GPIOB               // Порт динамиков
#define SPEAKER_MINI_PIN       GPIO_Pin_11         // Пин маленького динамика
#define SPEAKER_BIG_PIN        GPIO_Pin_10         // Пин большого динамика
#define SPEAKER_MINI_SOURCE    GPIO_PinSource11    // Источник для альтернативной функции малого динамика
#define SPEAKER_BIG_SOURCE     GPIO_PinSource10    // Источник для альтернативной функции большого динамика

// Тип динамика
#define SPEAKER_TYPE_MINI      0  // Маленький динамик
#define SPEAKER_TYPE_BIG       1  // Большой динамик с усилителем

// Устанавливает счёт таймера TIM14 в 50 мс
#define TIM14_SET_ARR_50MS() do{ TIM14->CNT = 0; TIM14->PSC = SystemCoreClock/1000 - 1; TIM14->ARR = 50; }while(0)

/*============================== Структуры и переменные ==============================*/

// Структура с данными о текущей песне
typedef struct{
    uint8_t speakerType;            // Тип динамика: большой (SPEAKER_TYPE_BIG) или маленький (SPEAKER_TYPE_MINI)
	
	uint8_t isCyclickSong;          // Повторяется ли песня циклично
    uint8_t tempo;                  // Темп песни
    uint16_t wholeNoteLen;          // Длительность целой ноты в мкс
    uint16_t noteCount;             // Количество нот в песне
	
	uint16_t curNoteIndex;          // Индекс текущей играющей ноты
	uint8_t isSongPlay;             // Играет ли песня сейчас или нет
	
    const uint16_t *freq;           // Массив со всеми нотами
    const uint8_t *beats;           // Массив с относительными длительностями нот (длительность одной целой ноты 255 (256))
} Song;
extern Song song;

/*============================== Библиотека песен ==============================*/
extern const uint16_t StarWars_MainTheme[38];
extern const uint8_t StarWars_MainTheme_Beats[38];

extern const uint16_t StarWars_TheImperialMarch[36];
extern const uint8_t StarWars_TheImperialMarch_Beats[36];

extern const uint16_t FurElise[41];
extern const uint8_t FurElise_Beates[41];

extern const uint16_t pirates[68];
extern const uint8_t pirates_Beates[68];

extern const uint16_t Harry_Potter[31];
extern const uint8_t Harry_Potter_Beats[31];

extern const uint16_t TetrisGameSong[43];
extern const uint8_t TetrisGameSong_Beats[43];

extern const uint16_t SnakeGameSong[38];
extern const uint8_t SnakeGameSong_Beats[38];
	
/*============================== Функции ==============================*/
extern void irProtocolAnalyze(void);  // Внешняя функция, которая находится в ir_remote_control.c

void speakerInit(void);
void speakerSetNote(uint16_t freq, uint16_t relatDuration);

void playBackgroundSong(uint8_t speakerType, const uint16_t *curSong, const uint8_t* songBeats, 
	                    uint16_t curNoteCount, uint8_t curTempo, uint8_t isCyclick);
void songStop(void);
void songPause(void);

void speakerMenu(void);

#endif
