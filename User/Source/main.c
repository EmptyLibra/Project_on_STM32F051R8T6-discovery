#include "lcd_main_menu.h"

void HardFault_Handler(void){
    
}

void delay(long mcs){
    unsigned int tick = 0;
    while (mcs--)
    {
        while (tick < 6)
        {
            tick++;
        }
        tick = 0;
    }
}

int main(void)
{
    __enable_irq();
    //----initializing pins-------------------------------------------------
    buttonPinsInit();                      // init user buttons PA0
    LCD_PortsInit();                       // init LCD display pins
    timerInit();                           // init timer for games
    speakerInit();                         // init PB12, PB11 as PWM
    ledsPinInit();                         // init leds PC8 and PC9
    IR_init();
    
    //----init LCD-----------------------------------------------
    LCD_Init();                  // init Display
    
    //---main loop-----------------------------------------------
    TIM_Cmd(TIM3, ENABLE);
    LCD_StartMainMenu();
//    
//    while(1){
//        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET){
//            LD3(SET);
//        } else{
//            LD3(RESET);
//        }
//    }
    
}
