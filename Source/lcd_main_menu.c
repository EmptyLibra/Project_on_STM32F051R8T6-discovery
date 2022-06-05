/**
  * @file    lcd_main_menu.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    05-June-2022
  * @brief   Эта библиотека реализует взаимодействие экрана и пользователя.
  *          В ней осуществляется отрисовка пунктов меню. */

#include "lcd_main_menu.h"

/*============================== Переменные ==============================*/
/* Список меню */
static char MENU_ITEMS[][11] = {
    "2048     ",
    "Snake    ",
    "Tetris   ",
    "Speaker  ",
    "IR Test  "
};

/* Индекс текущего элемента меню */
static int currentItem = 0;

/* Текущий ИК режим */
static uint8_t irMode = 0;
//
/*============================== Функции ==============================*/
/* Заполнение буфера экрана элементами меню */
static void LCD_FillMenuBuffer(void)
{
	/* Стираем буфер и обнуляем счетчик адреса */
    memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
    lcdStruct.byteIndex = 0x00;
    
    for(int i = 0; i < MENU_COUNT; i++) {
		/* Если элемент меню является текущим, то рисуем стрелку выбора */
        lcdStruct.writeStringToBuffer((i == currentItem) ? ">" : " ");
		
		/* Записываем элемент меню */
        lcdStruct.writeStringToBuffer(MENU_ITEMS[i]);
		
		/* Ставим дополнительный пробел у второго столбца меню */
        if((i%2 == 0) && (DISPLAY_WIDTH == 128)) {
            lcdStruct.writeStringToBuffer(" ");
        }
    }
}

/* Рисуем меню ИК-анализа-захвата */
static void drawIrMode(void)
{
    lcdStruct.byteIndex = DISPLAY_WIDTH*7;
    
    switch(irMode){
        case IR_MODE_TEST:
            lcdStruct.writeStringToBuffer(">test  capture  send");
            break;
        case IR_MODE_CAPTURE:
            lcdStruct.writeStringToBuffer(" test >capture  send");
            break;
        case IR_MODE_SEND:
            lcdStruct.writeStringToBuffer(" test  capture >send");
    }
    lcdStruct.displayFullUpdate();
}

/* Анализируем ИК-сигнал и отображаем результат ?????????????????????*/
static void irProtocolAnalyze(void)
{
    irReceiveProt.receiveType = IR_RECEIVE_TYPE_TEST;
    lcdStruct.clearOrFillDisplay(CLEAR);
    memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
    
    lcdStruct.byteIndex = 0;
    lcdStruct.writeStringToBuffer("IR test");
    LCD_DrawPageFromBuffer(PAGE_1);
    drawIrMode();
    
    char receiveStr[128];
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delayUs(buttonDelayUs);
            irMode -= (irMode == IR_MODE_TEST ? 0 : 1);
            isIrReceiveEn = 1;
            
            drawIrMode();
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delayUs(buttonDelayUs);
            irMode += (irMode == IR_MODE_SEND ? 0 : 1);

            drawIrMode();
        }
        if(isButtonPressed(BUTTON_SELECT)){
            delayUs(buttonDelayUs);
            if(irMode == IR_MODE_SEND){
                isIrReceiveEn = 0;
                if(irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_NEC){
                    LED_GREEN(SET);
                    sendNEC((uint32_t)irReceiveProt.receiveIRData1);
                    LED_GREEN(RESET);
                } else if(irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_PANASONIC){
                    LED_GREEN(SET);
                    sendPanasonic(irReceiveProt.receiveIRData1);
                    LED_GREEN(RESET);
                }else {
                    LED_BLUE(SET);
                    delayUs(buttonDelayUs);
                    LED_BLUE(RESET);
                }
            } else {
                isIrReceiveEn = 1;
            }
        }
        
        if(irReceiveProt.isProtocolReceive && irMode != IR_MODE_SEND){
            irReceiveProt.isProtocolReceive = 0;
            memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
            
            lcdStruct.clearOrFillDisplay(CLEAR);
            lcdStruct.byteIndex = 0;
            lcdStruct.writeStringToBuffer("IR test: ");
            
            lcdStruct.byteIndex = DISPLAY_WIDTH;
            lcdStruct.writeStringToBuffer("Preambula(us):");
			
			char receiveStr_5[5] = {0};
            sprintf(receiveStr_5, "%05d", ((unsigned int)irReceiveProt.preambula)*10);
            lcdStruct.writeStringToBuffer(receiveStr_5);
            
			char receiveStr_4[5] = {0};
			char receiveStr_8[9] = {0};
            switch(irReceiveProt.currProtocolType){
                // NEC protocol
                case IR_PROTOCOL_TYPE_NEC:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer("NEC");
                
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    sprintf(receiveStr_8, "%08X", (unsigned int) irReceiveProt.receiveIRData1);
				
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    break;
                
                // PANASONIC protocol
                case IR_PROTOCOL_TYPE_PANASONIC:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer("Panasonic");
                    
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    sprintf(receiveStr_4, "%04X", (unsigned int)(irReceiveProt.receiveIRData1 >> 32));
                    lcdStruct.writeStringToBuffer(receiveStr_4);
                    
                    sprintf(receiveStr_8, "%08X", (unsigned int)irReceiveProt.receiveIRData1);
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    break;
                
                // other protocols
                default:
                    lcdStruct.byteIndex = 54;
                    lcdStruct.writeStringToBuffer("Unknown");
                    lcdStruct.byteIndex = DISPLAY_WIDTH*2;
                    
                    sprintf(receiveStr_8, "%08X", (unsigned int) (irReceiveProt.receiveIRData2 >> 32));
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    
                    sprintf(receiveStr_8, "%08X", (unsigned int) irReceiveProt.receiveIRData2);
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                
                    lcdStruct.byteIndex = DISPLAY_WIDTH*3;
                    if(irReceiveProt.dataReceiveCount > 65/4){                        
                        sprintf(receiveStr_8, "%08X", (unsigned int) (irReceiveProt.receiveIRData1 >> 32));
                        lcdStruct.writeStringToBuffer(receiveStr_8);
                        
                        sprintf(receiveStr_8, "%08X", (unsigned int) irReceiveProt.receiveIRData1);
                        lcdStruct.writeStringToBuffer(receiveStr_8);
                    }
                    break;
            }
            if(irMode == IR_MODE_CAPTURE){
                irMode = IR_MODE_SEND;
                isIrReceiveEn = 0;
            }
            drawIrMode();
            lcdStruct.displayFullUpdate();
        }
        if(isButtonPressed(BUTTON_BACK)){
            isIrReceiveEn = 1;
            lcdStruct.clearOrFillDisplay(CLEAR);
            irReceiveProt.receiveType = IR_RECEIVE_TYPE_NORMAL;
            return;
        }
    }
}

//
/* Функция запуска основного меню */
void LCD_StartMainMenu()
{
	/* Мигание дисплея */
	lcdStruct.clearOrFillDisplay(FILL);
    delayUs(500000);
    lcdStruct.clearOrFillDisplay(CLEAR);
    
	/* Заполняем экран элементами меню и отрисовываем их */
    LCD_FillMenuBuffer();
    lcdStruct.displayFullUpdate();
    
	/* Основной цикл работы с меню */
    while(1){
		/* Движение вправо по списку */
        if(isButtonPressed(BUTTON_RIGHT))
        {
            delayUs(buttonDelayUs);
            currentItem += (currentItem == MENU_COUNT-1 ? 0 : 1);
            
            LCD_FillMenuBuffer();
            lcdStruct.displayFullUpdate();
        }
		
		/* Движение влево по списку */
        if(isButtonPressed(BUTTON_LEFT))
        {
            delayUs(buttonDelayUs);
            currentItem -= (currentItem == 0 ? 0 : 1);
            
            LCD_FillMenuBuffer();
            lcdStruct.displayFullUpdate();
        }
        
		/* Движение вверх по списку */
		if(isButtonPressed(BUTTON_TOP))
		{
			delayUs(buttonDelayUs);
			currentItem -= (currentItem < 2 ? 0 : 2);
			
			LCD_FillMenuBuffer();
			lcdStruct.displayFullUpdate();
		}
		
		/* Движение вниз по списку */
		if(isButtonPressed(BUTTON_BOTOOM))
		{
			delayUs(buttonDelayUs);
			currentItem += (currentItem >= MENU_COUNT-2 ? 0 : 2);
			
			LCD_FillMenuBuffer();
			lcdStruct.displayFullUpdate();
		}
		
		/* Выбор элемента списка */
		if(isButtonPressed(BUTTON_SELECT)) {
			/* Задержка, чтобы дождаться, когда пользователь отпустит кнопку */
			delayUs(400000);
			
			/* Запуск того или иного события */
			switch(currentItem) {
				case 0:
					startGame2048();
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
				default: ;
			}
			/* Отрисовка меню заново при выходе из пункта меню */
			LCD_FillMenuBuffer();
			lcdStruct.displayFullUpdate();
		}
    }
}
