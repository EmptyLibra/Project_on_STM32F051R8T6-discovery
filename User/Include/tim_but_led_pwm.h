#ifndef BUTT_HEADER
#define BUTT_HEADER

#include "pins_and_settings.h"
#include "infrared_receiver.h"
#include "stm32f0xx_tim.h"

//===========================Buttons and leds==============================
void ledsPinInit(void);
void buttonPinsInit(void);
void Systick_PWM(void);

void timerInit(void);


#endif
