#ifndef IR_HEADER
#define IR_HEADER

/* Detected infrared signal from different pults on NEC protocol (using Extented NEC protocol).
*  Executes commands to control LED and buttons.
*  Using infrared receiver on the PB10 pin and channel 3 of the Timer2 in capture mode.

* Extented NEC protocol features:
* Carrier frequency of 38 kHz.
* 16 bit Adress and 8 bit command length.
* bit time of 1.125ms or 2.25ms
* PWM modulation use

* Modulation:
* <-------Logical 1-----><-Logical 0--> 
* |||||||                |||||||       |||||||
* |||||||                |||||||       |||||||
* |||||||________________|||||||_______|||||||
* <560us>                <560us><560us><560us>
* <-------2.25ms--------><--1.125ms--->

Protocol:
*  ___________         1  0 0 1         1
* |           |       |  | | |         |                                          |
* |           |       |  | | |         |                                          |
* |           |_______|__|_|_|__.......|__........................................|
* <----9ms----><-4ms-><--16 bit Address--><-8 bit Command-><-8 bit invers Command->

*  Scheme of infrared receiver:
      |----------|
      |    __    |
      |   /  \   |
      |   \__/   |
      |----------|
        |  |   |
        |  |   |
       V0 GND  5V
*/

#include "pins_and_settings.h"
#include "speaker.h"
#include "stm32f0xx_tim.h"
#include "stdlib.h" // for abs()

/*==============================Defines==========================================*/
// ir-receiver pins (PB10)
#define RCC_AHBENR_IREN          RCC_AHBENR_GPIOBEN
#define IR_PORT                  GPIOB
#define IR_RECEIV_PIN            GPIO_Pin_10
#define IR_RECEIV_PIN_SOURCE     GPIO_PinSource10

// ir-transmitter pin
#define IR_TRANS_PIN             GPIO_Pin_14
#define IR_TRANS_PIN_SOURCE      GPIO_PinSource14


// buttons on chinese pult
#define IR_BUTTON_TOP     0x00FFA857 // Button - Green arrow up
#define IR_BUTTON_BOTOOM  0x00FFB04F // Button - DIY2
#define IR_BUTTON_RIGHT   0x00FF48B7 // Button - Blue arrow down
#define IR_BUTTON_LEFT    0x00FF08F7 // Button - Red arrow down
#define IR_BUTTON_SELECT  0x00FF10EF // Button - DIY4
#define IR_BUTTON_BACK    0x00FF50AF // Button - DIY6
#define IR_BLUE_LED       0x00FF9A65 // Blue button
#define IR_GREEN_LED      0x00FFA25D // Green button

#define IR_SONG_PAUSE     0x00FFC837 // Button - QUICK
#define IR_SONG_PLAY      0x00FFE817 // Button - SLOW
#define IR_SONG_SW_MT     0x00FF20DF // Button - JUMP3
#define IR_SONG_PIRATES   0x00FFA05F // Button - JUMP7
#define IR_SONG_ELISE     0x00FF609F // Button FADE3
#define IR_SONG_TETRIS    0x00FFE01F // Button FADE7

// buttons on Funai pult
#define IR_FUNAI_BUTTON_OK     0x21070EF1
#define IF_FUNAI_BUTTON_TOP    0x2107CE31
#define IF_FUNAI_BUTTON_BOTTOM 0x21072ED1
#define IF_FUNAI_BUTTON_LEFT   0x21076E91
#define IF_FUNAI_BUTTON_RIGHT  0x2107AE51
#define IF_FUNAI_BUTTON_BACK   0x21071EE1  
#define IF_FUNAI_BLUE_LED      0x2107DE21
#define IF_FUNAI_GREEN_LED     0x21073EC1

//=================NEC=========================
#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

/*===========================Variables==================================*/
extern uint16_t IR_curButton;

/*===========================Functions==================================*/
void IR_init(void);

//NEC
void mark(unsigned int time);
void space(unsigned int time);
extern void sendNEC (uint32_t data);

#endif
