/**
  * @file    main_conf.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    06-April-2022
  * @brief   Файл с основными настройками для всего проекта.
  *          Содержит основные функции инициализации и макросы для кнопок, светодиодов, системного.
  *          Настройка пинов для кнопок и светодиодов задаются здесь. 
  */
#ifndef __MAIN_CONF_H
#define __MAIN_CONF_H

/*============================== Подключение библиотек ==============================*/
/* CMSIS и SPL */
#include "stm32f0xx.h"             /* Основной CMSIS файл, содержащий регистры перефирии */
#include "stm32f0xx_rcc.h"         /* SPL библиотека для настройки тактирования */
#include "stm32f0xx_gpio.h"        /* SPL библиотека для настройки портов ввода/вывода */
#include "stm32f0xx_tim.h"         /* SPL библиотека для настройки таймеров */
#include "stm32f0xx_usart.h"       /* SPL библиотека для настройки интерфейса USART */

/*============================== Макросы ==============================*/
/*------------------- Порт и пины кнопок -------------------*/
#define RCC_AHBENR_BUTTONS_BRTL   RCC_AHBENR_GPIOAEN /* Тактирование порта кнопок */
#define BUTTONS_BRTL_PORT         GPIOA              /* Порт кнопок */
#define BUTTON_BACK               GPIO_Pin_0         /* Кнопка назад  - PA0 */
#define BUTTON_RIGHT              GPIO_Pin_1         /* Кнопка вправа - PA1 */
#define BUTTON_TOP                GPIO_Pin_2         /* Кнопка вверх  - PA2 */
#define BUTTON_LEFT               GPIO_Pin_3         /* Кнопка влево  - PA3 */

#define RCC_AHBENR_BUTTONS_BS     RCC_AHBENR_GPIOFEN /* Тактирование порта кнопок */
#define BUTTONS_BOTOOM_SEL_PORT   GPIOF              /* Порт кнопок */
#define BUTTON_BOTOOM             GPIO_Pin_4         /* Кнопка вниз   - PF4 */
#define BUTTON_SELECT             GPIO_Pin_5         /* Кнопка выбора - PF5 */

#define USER_BUTTON               GPIO_Pin_0         /* Синяя пользовательская кнопка - PA0 */

/*------------------- Порт и пины светодиодов -------------------*/
#define RCC_AHBENR_LEDS           RCC_AHBENR_GPIOCEN /* Тактирование порта светодиодов */
#define LEDS_PORT                 GPIOC              /* Порт светодиодов */
#define LED_GREEN_PIN             GPIO_Pin_9         /* Зеленый светодиод - PC9 (LD3) */
#define LED_BLUE_PIN              GPIO_Pin_8         /* Синий светодиод   - PC8 (LD4) */

/* Включение или выключение соответствующего светодиода */
#define LED_GREEN(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOC, LED_GREEN_PIN, Bit_SET) : GPIO_WriteBit(GPIOC, LED_GREEN_PIN, Bit_RESET))
#define LED_BLUE(mode)  ((mode) == 1 ? GPIO_WriteBit(GPIOC, LED_BLUE_PIN,  Bit_SET) : GPIO_WriteBit(GPIOC, LED_BLUE_PIN,  Bit_RESET))

/*------------------- Системный таймер -------------------*/
/* Первый бит регистра SysTick->CTRL (CTRL[0]) отвечает за включение или выключение таймера */
#define SYSTICK_ENABLE_MSK    (0x1UL)
#define SYSTICK_DISABLE_MSK   (0xFFEUL)

// Максимальное число, которое может сгенерироваться
#define MAX_RAND_NUM 0x7fffffffU

/*============================== Стуктуры и переменные ==============================*/
extern RCC_ClocksTypeDef clockStruct;  /* Структура с информацией о частотах тактирования */          
extern uint32_t buttonDelayUs;         /* Величина задержки для кнопок в мкс */

/*============================== Функции ==============================*/
unsigned int randLcg(void);
int abs(int num);
void delayUs(uint32_t mcs);
uint8_t isButtonPressed(uint16_t pin);

void delayInit(void);
void ledPinsInit(void);
void buttonPinsInit(void);

#endif
