#include "my_uart.h"

//### use asart as uart ###
// PA9  - TX - white wire
// PA10 - RX - green wire
uint32_t uart1_IT_RX_flag = 0;
FIFO_CREATE(my_fifo);              // my FIFO for uart
//uint8_t uartBuffer[LCD_BUFFER_LENGTH] = {0x00};

void USART1_IRQHandler(){ // interrupts handing
    if(USART_GetITStatus(UART1, USART_IT_RXNE) == SET){
		USART_ClearITPendingBit(UART1, USART_IT_RXNE);
		FIFO_PUSH(my_fifo, UART_RECEIVE_DATA());
        uart1_IT_RX_flag = SET;
	}
}

void uartPinsInit(void){
    // enable clocking for port A and USART1
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOAEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
    
    // PA10 - RX in alternative function
    GPIO_InitTypeDef GPIOA_ini;
    GPIOA_ini.GPIO_Pin = USART1_PIN_RX;
    GPIOA_ini.GPIO_Mode = GPIO_Mode_AF;
    GPIOA_ini.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOA_ini.GPIO_OType = GPIO_OType_PP;
    GPIOA_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIOA_ini);
    
    // PA9 - TX in alternative function
    GPIOA_ini.GPIO_Pin = USART1_PIN_TX;
    GPIOA_ini.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIOA_ini);
    
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
    
    uint8_t data;
    UART_SEND_DATA(0xAA);
    char receiveString[2];
    while(1){
        if(uart1_IT_RX_flag){
            data = FIFO_POP(my_fifo);
            UART_SEND_DATA(data);
            sprintf(receiveString, "%02u", data);
            LCD_SendByteData(data);
            //lcdStruct.writeStringToBuffer(uartBuffer, receiveString);
            //LCD_DrawPageFromBuffer(uartBuffer, PAGE_1);
        }
        
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                lcdStruct.clearOrFillDisplay(CLEAR);
                break;
            }
        #endif
    }
}
