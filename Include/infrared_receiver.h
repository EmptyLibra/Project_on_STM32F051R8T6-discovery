#ifndef __IR_H
#define __IR_H

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

#include "main_conf.h"
#include "speaker.h"
#include "stdlib.h" // for abs()

/*==============================Defines==========================================*/
// ir-receiver pins (PC9)
#define RCC_AHBENR_IREN          RCC_AHBENR_GPIOCEN
#define IR_PORT                  GPIOC
#define IR_RECEIV_PIN            GPIO_Pin_6
#define IR_RECEIV_PIN_SOURCE     GPIO_PinSource6

// ir-transmitter pin (PC8)
#define IR_TRANS_PIN             GPIO_Pin_7
#define IR_TRANS_PIN_SOURCE      GPIO_PinSource7


// buttons on chinese pult
#define IR_BUTTON_TOP     0x00FFA857 // Button - Green arrow up
#define IR_BUTTON_BOTOOM  0x00FFB04F // Button - DIY2
#define IR_BUTTON_RIGHT   0x00FF48B7 // Button - Blue arrow down
#define IR_BUTTON_LEFT    0x00FF08F7 // Button - Red arrow down
#define IR_BUTTON_SELECT  0x00FF10EF // Button - DIY4
#define IR_BUTTON_BACK    0x00FF50AF // Button - DIY6
#define IR_BLUE_LED       0x00FFA25D // Blue button
#define IR_GREEN_LED      0x00FF9A65 // Green button

#define IR_SONG_PAUSE_PLAY 0x00FFC837 // Button - QUICK
#define IR_SONG_SW_MT     0x00FF20DF // Button - JUMP3
#define IR_SONG_PIRATES   0x00FFA05F // Button - JUMP7
#define IR_SONG_ELISE     0x00FF609F // Button FADE3
#define IR_SONG_TETRIS    0x00FFE01F // Button FADE7

// buttons on Panasonic pult
#define IR_PANASONIC_BUTTON_TOP         0x400401005253
#define IR_PANASONIC_BUTTON_BOTOOM      0x40040100D2D3 
#define IR_PANASONIC_BUTTON_RIGHT       0x40040100F2F3 
#define IR_PANASONIC_BUTTON_LEFT        0x400401007273 
#define IR_PANASONIC_BUTTON_SELECT      0x400401009293 
#define IR_PANASONIC_BUTTON_BACK        0x400401002B2A 
#define IR_PANASONIC_GREEN_LED          0x40040100CECF 
#define IR_PANASONIC_BLUE_LED           0x400401008E8F 

#define IR_PANASONIC_SONG_PLAY          0x400409005059
#define IR_PANASONIC_SONG_PAUSE         0x400409000009
#define IR_PANASONIC_SONG_SW_MT         0x400401008C8D 
#define IR_PANASONIC_SONG_PIRATES       0x400401908D1C 
#define IR_PANASONIC_SONG_ELISE         0x40040190E574 
#define IR_PANASONIC_SONG_TETRIS        0x400401909100

//=================NEC=========================
#define IR_PROTOCOL_TYPE_NEC 1
#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

/* NEC real len:
* Preambula mark:                   880-950 (9000 us)
* Preambula space:                  430-470 (4500 us)
* One mark, zero mark, zero spase:  40-70   (560 us)
* One space:                        150-180 (1690 us)
*/

//=================PANASONIC=========================
#define IR_PROTOCOL_TYPE_PANASONIC 2
#define PANASONIC_BITS          48
#define PANASONIC_HDR_MARK    3600
#define PANASONIC_HDR_SPACE   1500
#define PANASONIC_BIT_MARK     400
#define PANASONIC_ONE_SPACE   1200
#define PANASONIC_ZERO_SPACE   400

/*Protocol PANASONIC (REC80, JAP) (in my case):
* 16 bit address, 32 bit commands
*/

/* PANASONIC real len:
* Preambula mark:                   300-360 (3000 us)
* Preambula space:                  160-190 (1500 us)
* One mark, zero mark, zero spase:  30-60   (400 us)
* One space:                        110-150 (1200 us)
*/


#define IR_PROTOCOL_TYPE_UNKNOWN  4

//-----Receive types--------------
#define IR_RECEIVE_TYPE_NORMAL 0
#define IR_RECEIVE_TYPE_TEST   1

/*===========================Structs and Variables==================================*/
//extern uint16_t IR_curButton;
extern uint8_t isIrReceiveEn;
    
typedef struct{
    uint8_t dataReceiveCount;
    uint64_t receiveIRData1;
    uint64_t receiveIRData2;
    uint8_t currProtocolType;
    
    uint8_t receiveType;
    uint8_t isProtocolReceive;
    
    uint16_t preambula;
} IR_ReceiveProt;
extern IR_ReceiveProt irReceiveProt;

/*===========================Functions==================================*/
void irReseivTransInit(void);

//NEC
void mark(unsigned int time);
void space(unsigned int time);
extern void sendNEC(uint32_t data);
extern void sendPanasonic(uint64_t data);
#endif
