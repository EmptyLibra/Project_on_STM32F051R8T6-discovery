#include "tim_but_led_pwm.h"

//===========================Buttons and leds==============================
static GPIO_TypeDef *GPIOx;

void buttonPinsInit(){ 
    // init additional buttons
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
    GPIO_InitTypeDef GPIOA_ini;
    GPIOA_ini.GPIO_Pin = BUTTON_BACK | BUTTON_RIGHT | BUTTON_TOP | BUTTON_LEFT;
    GPIOA_ini.GPIO_Mode = GPIO_Mode_IN;
    GPIOA_ini.GPIO_Speed = GPIO_Speed_10MHz;   // 2, 10, 50 ÌÃö
    GPIOA_ini.GPIO_OType = GPIO_OType_PP;
    GPIOA_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOA, &GPIOA_ini);
    
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOFEN, ENABLE);
    GPIO_InitTypeDef GPIOF_ini;
    GPIOF_ini.GPIO_Pin = BUTTON_BOTOOM | BUTTON_SELECT;
    GPIOF_ini.GPIO_Mode = GPIO_Mode_IN;
    GPIOF_ini.GPIO_Speed = GPIO_Speed_10MHz;   // 2, 10, 50 ÌÃö
    GPIOF_ini.GPIO_OType = GPIO_OType_PP;
    GPIOF_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOF, &GPIOF_ini);
}


void ledsPinInit(){
    //init leds PC8 and PC9
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOCEN, ENABLE);
    GPIO_InitTypeDef GPIOC_ini;
    
    GPIOC_ini.GPIO_Pin = LD3_PIN | LD4_PIN;
    GPIOC_ini.GPIO_Mode = GPIO_Mode_OUT;
    GPIOC_ini.GPIO_Speed = GPIO_Speed_10MHz;   // 2, 10, 50 ÌÃö
    GPIOC_ini.GPIO_OType = GPIO_OType_PP;
    GPIOC_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOC, &GPIOC_ini);
}
uint8_t isButtonPressed(uint16_t GPIO_Pin){
    GPIOx = ((GPIO_Pin == BUTTON_BOTOOM) || (GPIO_Pin == BUTTON_SELECT) ? BUTTONS_BOTOOM_SEL_PORT : BUTTONS_BRTL_PORT);
    if(IR_curButton == GPIO_Pin){
        IR_curButton = 0;
        return 1;
    }
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1){
		delay(5);
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1){
			return 1;
		}
	}
	return 0;
}
//
//============================Timers=======================================
void timerInit(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM3_ini;
    
    TIM3_ini.TIM_Prescaler =  48000 - 1;
    TIM3_ini.TIM_CounterMode = TIM_CounterMode_Up;
    TIM3_ini.TIM_Period = 1000;
    TIM3_ini.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseInit(TIM3, &TIM3_ini);
}
