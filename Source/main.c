#include "lcd_main_menu.h"

/* Обработчик прерываний для кретических ошибок (для отладки) */
void HardFault_Handler(void)
{
    
}

/* Главная функция проекта */
int main(void)
{
	/*### Пины PA14 и PA13 отвечают за отладку (debug) и их лучше не трогать ###*/
	
	/*==================== Блок инициализации ====================*/
	__enable_irq();       /* Разрешаем прерывания */
	delayInit();          /* Инициализация системного таймера и значений задержек */
    buttonPinsInit();     /* Инициализация внешних кнопок    (Пины: PA0-PA3, PF4, PF5 ) */
	ledPinsInit();        /* Инициализация светодиодов       (Пины: PC8, PC9) */
    LCD_PortsInit();      /* Инициализация пинов LCD дисплея (Пины: PB12-PB15 (SPI2)) */
	//speakerInit();        /* Инициализация динамиков         (Пины: PB12, PB11 в режиме ШИМ (PWM)) */
    irReseivTransInit();  /*  */
	uartPinsInit();       /* Инициализация USART1: TX - PA9, RX - PA10 */
	gameTimerInit();      /* Инициализация игрового таймера (GAME_TIMER) для генерации случайных чисел */
	
    /*==================== Блок второстепенной инициализации ====================*/
	LCD_Init();           /* Включение и инициализация дисплея */
    
	/*==================== Блок главного цикла ====================*/
    TIM_Cmd(GAME_TIMER, ENABLE);

	LCD_StartMainMenu();
	
	/* TEST START */
//    while(1)
//	{
//        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET)
//		{
//            LED_GREEN(SET);
//        } else
//		{
//            LED_GREEN(RESET);
//        }
//    }
    /* TEST END */
}
