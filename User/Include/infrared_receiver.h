#ifndef IR_HEADER
#define IR_HEADER

#include "pins_and_settings.h"
#include "stm32f0xx_tim.h"

/* detected infrared signal from different pults on NEC protocol */
/* scheme infrared receiver
  |---------|
  |  /--\   |
  |  \__/   |
  |---------|
    |  |  |
    |  |  |
   V0 GND 5V
*/
/* Use Extented NEC protocol:
*  
*/
// buttons on chinese pult
#define IR_BUTTON_TOP     0x00FFA857 // Button - Green arrow up
#define IR_BUTTON_BOTOOM  0x00FFB04F // Button - DIY2
#define IR_BUTTON_RIGHT   0x00FF48B7 // Button - Blue arrow down
#define IR_BUTTON_LEFT    0x00FF08F7 // Button - Red arrow down
#define IR_BUTTON_SELECT  0x00FF10EF // Button - DIY4
#define IR_BUTTON_BACK    0x00FF50AF // Button - DIY6
#define IR_BLUE_LED       0x00FF9A65 // Blue button
#define IR_GREEN_LED      0x00FFA25D // Green button

// buttons on Funai pult
#define IR_FUNAI_BUTTON_OK     0x21070EF1
#define IF_FUNAI_BUTTON_TOP    0x2107CE31
#define IF_FUNAI_BUTTON_BOTTOM 0x21072ED1
#define IF_FUNAI_BUTTON_LEFT   0x21076E91
#define IF_FUNAI_BUTTON_RIGHT  0x2107AE51
#define IF_FUNAI_BUTTON_BACK   0x21071EE1  
#define IF_FUNAI_BLUE_LED      0x2107DE21
#define IF_FUNAI_GREEN_LED     0x21073EC1

extern uint16_t curButton;

void IR_init(void);


#endif
