#include "lcd_main_menu.h"

static char MENU_ITEMS[][11] = {
    "2048     ",
    "Snake    ",
    "Tetris   ",
    "Speaker  ",
    "IR Test  ",
    "Dahatsu  "
};
static char DAHATSU_MENU_ITEMS[][11] = {
    "Power on ",
    "None     "
};
static int currentItem = 0, dahatsuCurItem = 0;
static uint8_t irMode = 0;
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

void drawIrMode(){
    // write hello string
    lcdStruct.byteIndex = DISPLAY_WIDTH*7;
    
    switch(irMode){
        case IR_MODE_TEST:
            lcdStruct.writeStringToBuffer(menuBuffer, ">test  capture  send");
            break;
        case IR_MODE_CAPTURE:
            lcdStruct.writeStringToBuffer(menuBuffer, " test >capture  send");
            break;
        case IR_MODE_SEND:
            lcdStruct.writeStringToBuffer(menuBuffer, " test  capture >send");
    }
    lcdStruct.displayFullUpdate(menuBuffer);
}
void irProtocolAnalyze(){
    irReceiveProt.receiveType = IR_RECEIVE_TYPE_TEST;
    lcdStruct.clearOrFillDisplay(CLEAR);
    memset(menuBuffer, 0, LCD_BUFFER_LENGTH);
    
    lcdStruct.byteIndex = 0;
    lcdStruct.writeStringToBuffer(menuBuffer, "IR test");
    LCD_DrawPageFromBuffer(menuBuffer, PAGE_1);
    drawIrMode();
    
    char receiveStr[128];
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delay(300000);
            irMode -= (irMode == IR_MODE_TEST ? 0 : 1);
            isIrReceiveEn = 1;
            
            drawIrMode();
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delay(300000);
            irMode += (irMode == IR_MODE_SEND ? 0 : 1);

            drawIrMode();
        }
        if(isButtonPressed(BUTTON_SELECT)){
            delay(300000);
            if(irMode == IR_MODE_SEND){
                isIrReceiveEn = 0;
                if(irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_NEC){
                    LD3(SET);
                    sendNEC((uint32_t)irReceiveProt.receiveIRData1);
                    LD3(RESET);
                } else if(irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_PANASONIC){
                    LD3(SET);
                    sendPanasonic(irReceiveProt.receiveIRData1);
                    LD3(RESET);
                } else if(irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_DAHATSU){
                    LD3(SET);
                    sendDahatsu(irReceiveProt.receiveIRData1, irReceiveProt.receiveIRData2);
                    LD3(RESET);
                }else {
                    LD4(SET);
                    delay(300000);
                    LD4(RESET);
                }
            } else {
                isIrReceiveEn = 1;
            }
        }
        
        if(irReceiveProt.isProtocolReceive && irMode != IR_MODE_SEND){
            irReceiveProt.isProtocolReceive = 0;
            memset(menuBuffer, 0, LCD_BUFFER_LENGTH);
            
            lcdStruct.clearOrFillDisplay(CLEAR);
            lcdStruct.byteIndex = 0;
            lcdStruct.writeStringToBuffer(menuBuffer, "IR test: ");
            
            lcdStruct.byteIndex = DISPLAY_WIDTH;
            lcdStruct.writeStringToBuffer(menuBuffer, "Preambula(us):");
            sprintf(receiveStr, "%05d", ((unsigned int)irReceiveProt.preambula)*10);
            LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 5);
            
            switch(irReceiveProt.currProtocolType){
                // NEC protocol
                case IR_PROTOCOL_TYPE_NEC:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer(menuBuffer, "NEC");
                
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    sprintf(receiveStr, "%08X", (unsigned int) irReceiveProt.receiveIRData1);
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    break;
                
                // PANASONIC protocol
                case IR_PROTOCOL_TYPE_PANASONIC:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer(menuBuffer, "Panasonic");
                    
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    sprintf(receiveStr, "%04X", (unsigned int)(irReceiveProt.receiveIRData1 >> 32));
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 4);
                    
                    sprintf(receiveStr, "%08X", (unsigned int)irReceiveProt.receiveIRData1);
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    break;
                
                case IR_PROTOCOL_TYPE_DAHATSU:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer(menuBuffer, "Dahatsu");
                    
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    sprintf(receiveStr, "%08X", (unsigned int)(irReceiveProt.receiveIRData1 >> 32));
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    
                    sprintf(receiveStr, "%08X", (unsigned int)irReceiveProt.receiveIRData1);
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    
                    lcdStruct.byteIndex = DISPLAY_WIDTH*3;
                    sprintf(receiveStr, "%08X", (unsigned int)(irReceiveProt.receiveIRData2 >> 32));
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                
                    sprintf(receiveStr, "%08X", (unsigned int)irReceiveProt.receiveIRData2);
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    break;
                
                // other protocols
                default:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer(menuBuffer, "Unknown");
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    
                    sprintf(receiveStr, "%08X", (unsigned int) (irReceiveProt.receiveIRData2 >> 32));
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    
                    sprintf(receiveStr, "%08X", (unsigned int) irReceiveProt.receiveIRData2);
                    LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                
                    lcdStruct.byteIndex = DISPLAY_WIDTH*3;
                    if(irReceiveProt.dataReceiveCount > 65/4){                        
                        sprintf(receiveStr, "%08X", (unsigned int) (irReceiveProt.receiveIRData1 >> 32));
                        LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                        
                        sprintf(receiveStr, "%08X", (unsigned int) irReceiveProt.receiveIRData1);
                        LCD_WritePartOfStringToBuffer(menuBuffer, receiveStr, 8);
                    }
                    break;
            }
            if(irMode == IR_MODE_CAPTURE){
                irMode = IR_MODE_SEND;
                isIrReceiveEn = 0;
            }
            drawIrMode();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        if(isButtonPressed(BUTTON_BACK)){
            isIrReceiveEn = 1;
            lcdStruct.clearOrFillDisplay(CLEAR);
            irReceiveProt.receiveType = IR_RECEIVE_TYPE_NORMAL;
            return;
        }
    }
}

void fillDahatsuMenu(){
    memset(menuBuffer, 0, LCD_BUFFER_LENGTH);
    lcdStruct.byteIndex = 0x00;
    int i;
    for(i = 0; i < DAHATSU_MENU_COUNT; i++){
        lcdStruct.writeSymbolToBuffer(menuBuffer, (i == dahatsuCurItem) ? '>' : ' ');
        lcdStruct.writeStringToBuffer(menuBuffer, DAHATSU_MENU_ITEMS[i]);
        if((i%2 == 0) && (DISPLAY_WIDTH == 128)){
            lcdStruct.writeSymbolToBuffer(menuBuffer, ' ');
        }
    }
}
void dahatsuMenu(){
    lcdStruct.clearOrFillDisplay(CLEAR);
    dahatsuCurItem = 0;
    fillDahatsuMenu();
    lcdStruct.displayFullUpdate(menuBuffer);
    
    while(1){
        if(isButtonPressed(BUTTON_RIGHT))
        {
            delay(300000);
            dahatsuCurItem += (dahatsuCurItem == DAHATSU_MENU_COUNT-1 ? 0 : 1);
            
            fillDahatsuMenu();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        if(isButtonPressed(BUTTON_LEFT))
        {
            delay(300000);
            dahatsuCurItem -= (dahatsuCurItem == 0 ? 0 : 1);
            
            fillDahatsuMenu();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        if(isButtonPressed(BUTTON_TOP)){
            delay(300000);
            dahatsuCurItem -= (dahatsuCurItem < 2 ? 0 : 2);
            
            fillDahatsuMenu();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        
        if(isButtonPressed(BUTTON_BOTOOM)){
            delay(300000);
            dahatsuCurItem += (dahatsuCurItem >= DAHATSU_MENU_COUNT-2 ? 0 : 2);
            
            fillDahatsuMenu();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        
        if(isButtonPressed(BUTTON_SELECT)){
            delay(500000);
            switch(dahatsuCurItem){
                case 0:
                    sendDahatsu(DAHATSU_COMMAND_POWER_ON1, DAHATSU_COMMAND_POWER_ON2);
                    break;
                case 1:
                    break;
                default: ;
            }
        }
        
        if(isButtonPressed(BUTTON_BACK)){
            lcdStruct.clearOrFillDisplay(CLEAR);
            return;
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
//                        for(uint8_t i = 0; i < 16; i++){
//                            sendPanasonic(0x40040100BCBD);
//                            delay(70000);
//                        }
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
                        irProtocolAnalyze();
                        break;
                    case 5:
                        dahatsuMenu();
                        break;
                    default: ;
                }
                LCD_FillMenuBuffer();
                lcdStruct.displayFullUpdate(menuBuffer);
            }
        #endif
    }
}

