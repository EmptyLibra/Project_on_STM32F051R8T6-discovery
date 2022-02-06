#include "my_uart.h"

//### use asart as uart ###
// PA9  - TX - white wire
// PA10 - RX - green wire
uint32_t uart1_IT_RX_flag = 0, uart1_IT_TX_flag = 0;
uint8_t commandByte = 0;
FIFO_CREATE(my_fifo);              // my FIFO for uart
//uint8_t uartBuffer[LCD_BUFFER_LENGTH] = {0x00};

void USART1_IRQHandler(){ // interrupts handing
    if(USART_GetITStatus(UART1, USART_IT_RXNE) == SET){
		USART_ClearITPendingBit(UART1, USART_IT_RXNE);
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
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
    
    // PA10 - RX, PA9 - TX in alternative function
    GPIO_InitTypeDef GPIOA_ini;
    GPIOA_ini.GPIO_Pin = USART1_PIN_RX | USART1_PIN_TX;
    GPIOA_ini.GPIO_Mode = GPIO_Mode_AF;
    GPIOA_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOA_ini.GPIO_OType = GPIO_OType_PP;
    GPIOA_ini.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIOA_ini);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
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
