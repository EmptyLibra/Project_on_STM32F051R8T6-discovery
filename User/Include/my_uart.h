#ifndef HEADER_GUARD_UART
#define HEADER_GUARD_UART

/* Receive data from Bluetooth, using USART (as UART).
*  TX: PA9, RX: PA10
*  Data format: 0xCAA.
*  C - command byte, first A - always 0x0D, second A - always 0x0A.
*/

#include "lcd_lib.h"
#include "my_fifo.h"

/*==============================Defines==========================================*/
// USART1 port settings
#define RCC_AHBENR_UART1EN    RCC_AHBENR_GPIOAEN
#define UART1_PORT            GPIOA
#define UART1_TX_PIN          GPIO_Pin_9           // PA9 - TX (USART1)
#define UART1_RX_PIN          GPIO_Pin_10          // PA10 - RX (USART1)
#define UART1_TX_SOURCE       GPIO_PinSource9
#define UART1_RX_SOURCE       GPIO_PinSource10

#define UART_SEND_DATA(data) USART_SendData(USART1, data)
#define UART_RECEIVE_DATA() ((uint8_t) USART_ReceiveData(USART1))

/*===========================Structs==================================*/
FIFO_STRUCT_CREATE(256);
extern MYFIFO my_fifo;

/*===========================Variables==================================*/
extern uint32_t uart1_IT_TX_flag, uart1_IT_RX_flag;

/*===========================Functions==================================*/
void uartPinsInit(void);
void startUartDataGetting(void);

#endif
