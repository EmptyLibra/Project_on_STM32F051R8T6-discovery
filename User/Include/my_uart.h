#ifndef HEADER_GUARD_UART
#define HEADER_GUARD_UART

#include "lcd_lib.h"
#include "my_fifo.h"
#include <stdio.h>  // for sprintf()

FIFO_STRUCT_CREATE(256);

extern MYFIFO my_fifo;
extern uint32_t uart1_IT_TX_flag;
extern uint32_t uart1_IT_RX_flag;

extern void USART1_IRQHandler(void);                // interrupts handing
void uartPinsInit(void);
void startUartDataGetting(void);

#endif
