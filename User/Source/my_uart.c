#include "my_uart.h"

/* Receive data from Bluetooth, using USART (as UART).
*  TX: PA9, RX: PA10
*  Data format: 0xCAA.
*  C - command byte, first A - always 0x0D, second A - always 0x0A.
*/

/*===========================Variables and Structs==================================*/
FIFO_CREATE(my_fifo);                                     // my FIFO for uart
uint32_t uart1_IT_RX_flag = 0, uart1_IT_TX_flag = 0;      // Successful reception and transmission flag

/* For BlueTooth */
static uint8_t commandByte = 0;   // BlueTooth command byte

/*===========================Functions==================================*/
void USART1_IRQHandler(){ // interrupts handing
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		FIFO_PUSH(my_fifo, UART_RECEIVE_DATA());
        
        if(FIFO_COUNT_ELEMENTS(my_fifo) > 2){
            commandByte = FIFO_POP(my_fifo);
            if(FIFO_POP(my_fifo) == 0x0D){
                if(FIFO_POP(my_fifo) == 0x0A){
                    if(commandByte == 0x89){
                        GPIO_WriteBit(GPIOC, LD4_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD4_PIN)));
                    }
                }
            }
            FIFO_FLUSH(my_fifo);
        }
	}
}

void uartPinsInit(void){
    // enable clocking for port A and USART1
    RCC_AHBPeriphClockCmd(RCC_AHBENR_UART1EN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
    
    // PA10 - RX, PA9 - TX in alternative function
    GPIO_InitTypeDef GPIOx_ini;
    GPIOx_ini.GPIO_Pin = UART1_TX_PIN | UART1_RX_PIN;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;
    GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(UART1_PORT, &GPIOx_ini);
    
    GPIO_PinAFConfig(UART1_PORT, UART1_RX_SOURCE, GPIO_AF_1);
    GPIO_PinAFConfig(UART1_PORT, UART1_TX_SOURCE, GPIO_AF_1);
    // usart settings
    NVIC_EnableIRQ(USART1_IRQn);   // enable interrupts
    
    USART_InitTypeDef USART_ini;
    USART_ini.USART_BaudRate = 115200;
    USART_ini.USART_WordLength = USART_WordLength_8b;
    USART_ini.USART_StopBits = USART_StopBits_1;
    USART_ini.USART_Parity = USART_Parity_No;
    USART_ini.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_ini.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    
    USART_Init(USART1, &USART_ini);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
    USART_Cmd(USART1, ENABLE);
}

void startUartDataGetting(){
    lcdStruct.clearOrFillDisplay(CLEAR);

    while(1){
        if(FIFO_COUNT_ELEMENTS(my_fifo) > 2){
            commandByte = FIFO_POP(my_fifo);
            if(FIFO_POP(my_fifo) == 0x0D){
                if(FIFO_POP(my_fifo) == 0x0A){
                    if(commandByte == 0x89){
                        GPIO_WriteBit(GPIOC, LD4_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD4_PIN)));
                    }
                }
            }
            FIFO_FLUSH(my_fifo);
        }
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                lcdStruct.clearOrFillDisplay(CLEAR);
                break;
            }
        #endif
    }
}
