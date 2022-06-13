/**
  * @file    my_uart.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    05-June-2022
  * @brief   Библиотека для настройки UART1 и отправка по нему сообщений (PA9 - TX, PA10 - RX). 
  * Этот USART1 используется как UART1 для общения с bluetooth модулем HC05.
  * Формат данных для общения с ним: 0xCCAA
  * C - два байта команды, первая A - всегда 0x0D, вторая A - всегда 0x0A.  */
  
#include "my_uart.h"

/*============================== Структуры и переменные ==============================*/
FIFO_CREATE(my_fifo);                                     // my FIFO for uart

/*--- Для работы с Bluetooth ---*/
static uint16_t commandByte = 0;   /* Тип команды по bluetooth */
extern uint16_t IR_curButton;      /* Текущая нажатая IR-кнопка */

/*============================== Функции ==============================*/
/* Обработчик прерываний по USART1 (Команды от HC-05) */
void USART1_IRQHandler()
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		/* Очистка флага и запись принятых данных в FIFO */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		FIFO_PUSH(my_fifo, UART_RECEIVE_DATA());
        
		/* Если пришла целая команда */
        if(FIFO_COUNT_ELEMENTS(my_fifo) >= 4) {
			/* Записываем тип команды */
            commandByte = ((uint16_t)FIFO_POP(my_fifo) << 8) + (uint16_t)FIFO_POP(my_fifo);
			
			/* Если это корректная команда от HC-05 */
            if(FIFO_POP(my_fifo) == 0x0D) {
                if(FIFO_POP(my_fifo) == 0x0A) {
					
					/* Выбор действий в зависимости от команды */
                    switch(commandByte){
                        case BT_COMMAND_BUTTON_TOP:
                            IR_curButton = BUTTON_TOP;
                            break;
                        case BT_COMMAND_BUTTON_BOTTOM:
                            IR_curButton = BUTTON_BOTOOM;
                            break;
                        case BT_COMMAND_BUTTON_RIGHT:
                            IR_curButton = BUTTON_RIGHT;
                            break;
                        case BT_COMMAND_BUTTON_LEFT:
                            IR_curButton = BUTTON_LEFT;
                            break;
                        case BT_COMMAND_BUTTON_OK:
                            IR_curButton = BUTTON_SELECT;
                            break;
                        case BT_COMMAND_BUTTON_BACK:
                            IR_curButton = BUTTON_BACK;
                            break;
                        
                        case BT_COMMAND_SONG_PAUSE:
                            musicSet.isSongPlay = 0;
                            break;
                        case BT_COMMAND_SONG_PLAY:
                            musicSet.isSongPlay = 1;
                            break;
                        
                        case BT_COMMAND_SONG_ELISE:
                            playBackgroundSong(SPEAKER_TYPE_BIG, FurElise, FurElise_Beates, sizeof(FurElise)/2, 90,0);
                            break;
                        case BT_COMMAND_SONG_SW_MT:
                            playBackgroundSong(SPEAKER_TYPE_BIG, StarWars_MainTheme, StarWars_MainTheme_Beats, sizeof(StarWars_MainTheme)/2, 108,0);
                            break;
                        case BT_COMMAND_SONG_TETRIS:
                            playBackgroundSong(SPEAKER_TYPE_BIG, TetrisGameSong, TetrisGameSong_Beats, sizeof(TetrisGameSong)/2, 140,1);
                            musicSet.isCyclickSong = 1;
                            break;
                        default:
							LED_BLUE(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_BLUE_PIN));
							LED_GREEN(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_GREEN_PIN));
                            //GPIO_WriteBit(GPIOC, LD4_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD4_PIN)));
                    }
                }
            }
			/* Очищаем FIFO */
            FIFO_FLUSH(my_fifo);
        }
	}
}

/* Инициализация пинов UART (TX - PA9, RX - PA10) */
void uartPinsInit(void)
{
    /* Включаем тактирование порта А и USART1 */
    RCC_AHBPeriphClockCmd(RCC_AHBENR_UART_PORT_EN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
    
    /* Настрока пинов PA10 - AF1 - RX, PA9 - AF1 - TX */
    GPIO_InitTypeDef GPIOx_ini;
    GPIOx_ini.GPIO_Pin = UART1_TX_PIN | UART1_RX_PIN;    /* Выбор пинов */
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;                  /* Выбор режима пина: альтернативная функция */
    GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;             /* Частота тактирования высокая -  50 МГц */
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;                /* Двухтактный (push-pull) выход */
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_UP;                  /* Подтягивание к питанию */
    GPIO_Init(UART1_PORT, &GPIOx_ini);                   /* Записываем данные в порт */
    
	/* Выбор альтернативных функций для пинов */
    GPIO_PinAFConfig(UART1_PORT, UART1_RX_SOURCE, GPIO_AF_1);
    GPIO_PinAFConfig(UART1_PORT, UART1_TX_SOURCE, GPIO_AF_1);
    
	/*----- Настройки USART1 -----*/
    NVIC_EnableIRQ(USART1_IRQn);   /* Включаем прерывания для USART1 */
    
    USART_InitTypeDef USART_ini;
    USART_ini.USART_BaudRate = 115200;                                     /* Скорость USART: 115200 бод */
    USART_ini.USART_WordLength = USART_WordLength_8b;                      /* Длина слова: 8 бит */
    USART_ini.USART_StopBits = USART_StopBits_1;                           /* Длина стоп бита: 1 бит */
    USART_ini.USART_Parity = USART_Parity_No;                              /* Добавление бита четности: нет */
    USART_ini.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                  /* Режимы работы USART: приемник и передатчик */
    USART_ini.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  /* Для RS232 (контакт разрешение на отправку) */
    
    USART_Init(USART1, &USART_ini);                 /* Инициалазация USART1 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  /* Разрешаем прерывания по приему */
    USART_Cmd(USART1, ENABLE);                      /* Включаем USART1 */
}
