/**
  * @file    lcd_main_menu.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    05-June-2022
  * @brief   Эта библиотека реализует взаимодействие экрана и пользователя.
  *          В ней осуществляется отрисовка пунктов меню. */

#include "lcd_main_menu.h"

/*============================== Переменные ==============================*/
/* Список меню (в строке 9 символ, но русский символ считается за 2 + '\0') */
static const char MENU_ITEMS[][19] = {
    "2048     ",
    "Змейка   ",
    "Тетрис   ",
    "Динамики ",
    "ИК-тест  "
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
	// Устанавливаем курсор на последнюю страницу
    LCD_SetBufferCursor(0, PAGE_8);
	
	// В зависимости от текущего режима рисуем меню
    switch(irMode) {
        case IR_MODE_TEST:
            lcdStruct.writeStringToBuffer(">тест  захват  отпр.");
            break;
        case IR_MODE_CAPTURE:
            lcdStruct.writeStringToBuffer(" тест >захват  отпр.");
            break;
        case IR_MODE_SEND:
            lcdStruct.writeStringToBuffer(" тест  захват >отпр.");
    }
	
	// Полностью отрисовываем дисплей
    lcdStruct.displayFullUpdate();
}

/* Анализируем ИК-сигнал и отображаем результат */
static void irProtocolTest(void)
{
	// Включаем тестовые режим приема ИК-команд (без их выполнения)
    irReceiveType = IR_RECEIVE_TYPE_TEST_COMMAND;
	
	// Очищаем буфер меню
    memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
    
	// Рисуем само меню
    LCD_SetBufferCursor(0,0);
    lcdStruct.writeStringToBuffer("ИК тест");
    LCD_DrawPageFromBuffer(PAGE_1);
    drawIrMode();
    
    while(1) 
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
		// Движение по меню ИК-теста влево
        if(isButtonPressed(BUTTON_LEFT)) {
			// Задержка для адекватной работы кнопок
            delayUs(buttonDelayUs);
			
			// Переключение режима меню и включение приема ИК-сообщений
            irMode -= (irMode == IR_MODE_TEST ? 0 : 1);
            irReceiveState = ENABLE;
            irProtocol.isFullReceive = 0;
			irProtocol.bitsCount = 0;
			irProtocol.irCmdBitCounter = 0;
			irProtocol.irCommand = 0;
			irProtocol.preambuleTime = 0;
			irProtocol.type = 0;
			irProtocol.repeatLastCmd = 0;
			
			// Рисуем ИК-меню
            drawIrMode();
        }
		
		// Движение по меню ИК-теста вправо
        if(isButtonPressed(BUTTON_RIGHT)) {
			// Задержка для адекватной работы кнопок
            delayUs(buttonDelayUs);
			
			// Переключение режима меню
            irMode += (irMode == IR_MODE_SEND ? 0 : 1);
			
			// Рисуем ИК-меню
            drawIrMode();
        }
		
		// Отправка принятой команды
        if(isButtonPressed(BUTTON_SELECT)) {
			// Задержка для адекватной работы кнопок
            delayUs(buttonDelayUs);
			
			// Если мы находимся в режиме отправки команд, то отправляем ИК-команду
            if(irMode == IR_MODE_SEND) {
				// Выключаем прием ИК-команд, чтобы не принять то, что отправили
                irReceiveState = DISABLE;
				
				// В зависимости от протокола вызываем ту или иную функцию
                if(irProtocol.type != IR_PROTOCOL_TYPE_UNKNOWN) {
					// Мигание зеленого светодиода говорит об успешной отправке команды
                    LED_GREEN(SET);
                    sendReceiveIrCommad();
					LED_GREEN(RESET);

                }else {
					// Мигание синего светодиода говорит о не успешной отправке неизвестного протокола
                    LED_BLUE(SET);
                    delayUs(buttonDelayUs);
                    LED_BLUE(RESET);
                }
            }
        }
		
		// Печатаем новую принятую команду, если она есть (и если мы не в режиме отправки команды)
        if((irProtocol.isFullReceive) && (irMode != IR_MODE_SEND)) {
			// Очищаем буфер дисплея
            memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
            
			// Печатаем сообщение об ИК-тесте
            LCD_SetBufferCursor(0,0);
            lcdStruct.writeStringToBuffer("ИК тест: ");
            
			// Печатаем длину преамбулы
			LCD_SetBufferCursor(0, PAGE_2);
            lcdStruct.writeStringToBuffer("Преамбула(мкс): ");
			
			// Преобразуем числовое значение в строку
			char receiveStr_5[5] = {0};
            sprintf(receiveStr_5, "%05d", ((unsigned int)irProtocol.preambuleTime));
            lcdStruct.writeStringToBuffer(receiveStr_5);
            
			// Строки-буферы для печати команд
			char receiveStr_4[5] = {0};
			char receiveStr_8[9] = {0};
			
			// Печать команды
			LCD_SetBufferCursor(54,0);
            switch(irProtocol.type)
			{
                // Протокол NEC
                case IR_PROTOCOL_TYPE_NEC:
					// Печать названия протокола
                    lcdStruct.writeStringToBuffer("NEC");
					
					// Печать команды протокола
					LCD_SetBufferCursor(0, PAGE_3);
                    sprintf(receiveStr_8, "%08X", (unsigned int) irProtocol.irCommand);
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    break;
                
                // Протокол PANASONIC
                case IR_PROTOCOL_TYPE_PANASONIC:
					// Печать названия протокола
                    lcdStruct.writeStringToBuffer("Panasonic");
                    
					// Печать команды протокола в два этапа (т.к. sprintf поддерживает только unsigned int)
                    LCD_SetBufferCursor(0, PAGE_3);
                    sprintf(receiveStr_4, "%04X", (unsigned int)(irProtocol.irCommand >> 32));
                    lcdStruct.writeStringToBuffer(receiveStr_4);
                    
                    sprintf(receiveStr_8, "%08X", (unsigned int)irProtocol.irCommand);
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    break;
                
                // Прочие протоколы
                default:
					// Печать названия протокола
                    lcdStruct.writeStringToBuffer("Unknown");
					
					// Печать команды протокола в два этапа
                    LCD_SetBufferCursor(0, PAGE_3);    
                    sprintf(receiveStr_8, "%08X", (unsigned int) (irProtocol.irCommand >> 32));
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    
                    sprintf(receiveStr_8, "%08X", (unsigned int) irProtocol.irCommand);
                    lcdStruct.writeStringToBuffer(receiveStr_8);
                    break;
            }
			
			// Переход в режим отправки команды или обнуление данных протокола
            if(irMode == IR_MODE_CAPTURE){
                irMode = IR_MODE_SEND;
                irReceiveState = DISABLE;
            } else {
				irProtocol.isFullReceive = 0;
				irProtocol.bitsCount = 0;
				irProtocol.irCmdBitCounter = 0;
				irProtocol.irCommand = 0;
				irProtocol.preambuleTime = 0;
				irProtocol.type = 0;
				irProtocol.repeatLastCmd = 0;
			}
            drawIrMode();
            lcdStruct.displayFullUpdate();
        }
		
		// Вернуться в главное меню
        if(isButtonPressed(BUTTON_BACK)){
            irReceiveState = ENABLE;
            lcdStruct.clearOrFillDisplay(CLEAR);
            irReceiveType = IR_RECEIVE_TYPE_RUN_COMMAND;
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
		
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
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
					irProtocolTest();
					break;
				default: ;
			}
			/* Отрисовка меню заново при выходе из пункта меню */
			LCD_FillMenuBuffer();
			lcdStruct.displayFullUpdate();
		}
    }
}
