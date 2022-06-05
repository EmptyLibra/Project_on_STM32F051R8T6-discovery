/**
  * @file    main_conf.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    06-April-2022
  * @brief   Файл с основными настройками для всего проекта.
  *          Содержит основные функции инициализации для кнопок, светодиодов, системного и игрового таймера.
  *          Настройка пинов для кнопок и светодиодов задаются в main_conf.h.
  *          Здесь задаётся их инициализация.
  *          Также здесь определяются величины задержек.
  */
#include "main_conf.h"

/*============================== Стуктуры и переменные ==============================*/
/* Текущая инфракрасная кнопка (для управления с пульта) (Определена в: infrared_receiver.c) */
extern uint16_t IR_curButton;  

RCC_ClocksTypeDef clockStruct;           /* Структура с информацией о частотах тактирования */

static volatile uint32_t delayUsDec = 0; /* Счетчик вниз для системного таймера */
uint32_t buttonDelayUs = 0;              /* Величина задержки для кнопок в мкс */

/*============================== Функции ==============================*/
/* Обработчик прерываний системного таймера для счета задержки в мкс*/
void SysTick_Handler(void)
{
	if(delayUsDec) delayUsDec--;
}

/* Функция задержки на указанное количесвто микросекунд с помощью системного таймера 
 * Для большей точности используется системный таймер, а не просто пустой цикл*/
void delayUs(uint32_t mcs)
{
	/* Включаем системный таймер (управляем в ручную, чтобы прерывания не вызывались постоянно) */
	SysTick->CTRL |= SYSTICK_ENABLE_MSK;
	
	/* Записываем значение в счетчик для системного таймера */
	delayUsDec = mcs;
	while(delayUsDec);
	
	/* Выключаем системный таймер и обнуляем его счетчик */
	SysTick->CTRL &= SYSTICK_DISABLE_MSK;
	SysTick->VAL = 0;
}
/* Функция проверяет, нажата ли кнопка (или кнопка на пульте)
* GPIO_Pin: кнопка (пин кнопки), которую надо проверить (возможные значения указаны в main_conf.h)
 * Возвращаемое значение: 1 - если кнопка нажата, 0 - если не нажата */
uint8_t isButtonPressed(uint16_t GPIO_Pin)
{
	/* Переменная для хранения порта текущей кнопки */
	GPIO_TypeDef *GPIOx;
	
	/* Выбор нужного порта */
    GPIOx = ((GPIO_Pin == BUTTON_BOTOOM) || (GPIO_Pin == BUTTON_SELECT) ? BUTTONS_BOTOOM_SEL_PORT : BUTTONS_BRTL_PORT);
    
	/* Проверка нажатия кнопки на ИК-пульте */
	if(IR_curButton == GPIO_Pin) {
        IR_curButton = 0;
        return 1;
    }
	
	/* Проверка нажатия обычной кнопки (+защита от дребезга контактов) */
    if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1) {
		delayUs(5);
		if(GPIO_ReadInputDataBit(GPIOx, GPIO_Pin) == 1) {
			return 1;
		}
	}
	return 0;
}

/* Инициализация системного таймера для задержек, а также задание значений для основных задержек */
void delayInit(void)
{
	/* Получение информации о тактировании */
	RCC_GetClocksFreq(&clockStruct);
	
	/*--- Настройка системного таймера для задержек в мкС ---
	* Т.к. за 1 сек происходит SystemCoreClock тактов, то, 
	* чтобы получить 1 мкс нужно отсчитать SystemCoreClock/1000000 тактов.
	* Обнуляем счетчик таймера.
	* Выбираем в качестве источника тактирования - процессор, разрешаем прерывания, таймер не включаем */
	SysTick->LOAD = SystemCoreClock/1000000 - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
	
	/* Задержка после нажатия на кнопку - 200 мс = 200 000 мкс */
	buttonDelayUs = 200000;
}

/* Инициализация пинов кнопок */
void buttonPinsInit(void)
{ 
    /* Инициализация дополнительных кнопок */
    RCC_AHBPeriphClockCmd(RCC_AHBENR_BUTTONS_BRTL, ENABLE);  /* Включаем тактирование */
    GPIO_InitTypeDef GPIOx_ini;
    GPIOx_ini.GPIO_Pin = BUTTON_BACK | BUTTON_RIGHT | BUTTON_TOP | BUTTON_LEFT;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_IN;                    /* Настраиваем ногу как вход */
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;               /* Частота тактирования средняя -  10 МГц */
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;                  /* Двухтактный (push-pull) выход */
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;                /* Без подтягивания */
    
	GPIO_Init(BUTTONS_BRTL_PORT, &GPIOx_ini);              /* Записываем данные в порт */
    
    RCC_AHBPeriphClockCmd(RCC_AHBENR_BUTTONS_BS, ENABLE);  /* Включаем тактирование */
    GPIOx_ini.GPIO_Pin = BUTTON_BOTOOM | BUTTON_SELECT;
    
	GPIO_Init(BUTTONS_BOTOOM_SEL_PORT, &GPIOx_ini);        /* Записываем данные в порт */
}

/* Инициализация пинов светодиодов */
void ledPinsInit(void)
{
    /* Инициализация светодиодов на плате */
    RCC_AHBPeriphClockCmd(RCC_AHBENR_LEDS, ENABLE);  /* Включаем тактирование */
    GPIO_InitTypeDef GPIOx_ini;
    
    GPIOx_ini.GPIO_Pin = LED_GREEN_PIN | LED_BLUE_PIN;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_OUT;       /* Настраиваем ногу как выход */
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;   /* Частота тактирования средняя -  10 МГц */
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;      /* Двухтактный (push-pull) выход */
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;    /* Без подтягивания */
    
    GPIO_Init(LEDS_PORT, &GPIOx_ini);          /* Записываем данные в порт */
}

/* Инициализация игрового таймера для генерации случайных чисел */
void gameTimerInit(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  /* Включаем тактирование */
    TIM_TimeBaseInitTypeDef TIM3_ini;
    
    TIM3_ini.TIM_Prescaler = SystemCoreClock/1000 - 1; /* Предделитель (считаем в мс) */
    TIM3_ini.TIM_CounterMode = TIM_CounterMode_Up;     /* Счёт вверх */
    TIM3_ini.TIM_Period = 1000;                        /* Счёт до 1 секунды */
    TIM3_ini.TIM_ClockDivision = TIM_CKD_DIV1;         /* Без делителя частоты */
    
    TIM_TimeBaseInit(TIM3, &TIM3_ini);                 /* Записываем данные в порт */
}
