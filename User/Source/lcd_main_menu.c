#include "lcd_main_menu.h"

static char MENU_ITEMS[][11] = {
    "2048     ",
    "Snake    ",
    "Tetris   ",
    "Speaker  ",
    "UART     "
};
static int currentItem = 0;
static uint8_t menuBuffer[LCD_BUFFER_LENGTH] = {0x00};
//
//----menu draw-----------------------------------------
void LCD_FillMenuBuffer(){
    memset(menuBuffer, 0, LCD_BUFFER_LENGTH);
    lcdStruct.byteIndex = 0x00;
    int i;
    for(i = 0; i < MENU_COUNT; i++){
        lcdStruct.writeSymbolToBuffer(menuBuffer, (i == currentItem) ? '>' : ' ');
        lcdStruct.writeStringToBuffer(menuBuffer, MENU_ITEMS[i]);
        if((i%2 == 0) && (DISPLAY_WIDTH == 128)){
            lcdStruct.writeSymbolToBuffer(menuBuffer, ' ');
        }
    }
}
//
//----start function------------------------------
void LCD_StartMainMenu(){
	lcdStruct.clearOrFillDisplay(FILL);
    delay(500000);
    lcdStruct.clearOrFillDisplay(CLEAR);
    
    LCD_FillMenuBuffer();
    lcdStruct.displayFullUpdate(menuBuffer);
    
    while(1){
        if(isButtonPressed(BUTTON_RIGHT))
        {
            delay(300000);
            currentItem += (currentItem == MENU_COUNT-1 ? 0 : 1);
            
            LCD_FillMenuBuffer();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        if(isButtonPressed(BUTTON_LEFT))
        {
            delay(300000);
            currentItem -= (currentItem == 0 ? 0 : 1);
            
            LCD_FillMenuBuffer();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        
        #ifdef MH046
            if(isButtonPressed(BUTTON_BOTOOM))
            {
                delay(500000);
                switch(currentItem){
                    case 0:
                        startGame2048();
                        break;
                    case 1:
                        startSnakeGame();
                        break;
                    default: ;
                        
                }
                LCD_FillMenuBuffer();
                lcdStruct.displayFullUpdate(menuBuffer);
            }
        #else
            if(isButtonPressed(BUTTON_TOP))
            {
                delay(300000);
                currentItem -= (currentItem < 2 ? 0 : 2);
                
                LCD_FillMenuBuffer();
                lcdStruct.displayFullUpdate(menuBuffer);
            }
            
            if(isButtonPressed(BUTTON_BOTOOM))
            {
                delay(300000);
                currentItem += (currentItem >= MENU_COUNT-2 ? 0 : 2);
                
                LCD_FillMenuBuffer();
                lcdStruct.displayFullUpdate(menuBuffer);
            }
            
            if(isButtonPressed(BUTTON_SELECT)){
                delay(500000);
                switch(currentItem){
                    case 0:
                        startGame2048();
//                        sendNEC(0x00FF02FD);
                        break;
                    case 1:
                        startSnakeGame();
                        break;
                    case 2:
                        startTetrisGame();
                        break;
                    case 3:
                        lcdStruct.clearOrFillDisplay(CLEAR);
                        speakerMenu();
                        break;
                    case 4:
                        startUartDataGetting();
                        break;
                    default: ;
                }
                LCD_FillMenuBuffer();
                lcdStruct.displayFullUpdate(menuBuffer);
            }
        #endif
    }
}

