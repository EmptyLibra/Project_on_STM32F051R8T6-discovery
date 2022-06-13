#include "infrared_receiver.h"

/* Detected infrared signal from different pults on NEC protocol (using Extented NEC protocol).
*  Executes commands to control LED and buttons.
*  Using infrared receiver on the PB10 pin and channel 3 of the Timer2 in capture mode.
*  Scheme of infrared receiver:
      |----------|
      |    __    |
      |   /  \   |
      |   \__/   |
      |----------|
        |  |   |
        |  |   |
       V0 GND  5V
*/

/*===========================Variables==================================*/
static uint16_t currData[250] = {0};                       // Contains pulse durations (start puls, 0 or 1)
static uint16_t currDataIndex = 0;
static uint16_t prevDataElem = 0, curDataElem = 0;

static uint64_t IRData1 = 0, IRData2 = 0;                  // Contains current ir command like: 0x00FF9A65
static uint8_t IRDataCounter = 0;
static uint32_t lastTIM15CNT = 0;

IR_ReceiveProt irReceiveProt;
uint16_t IR_curButton = 0;
uint8_t isIrReceiveEn = 1;
//
/*===========================Functions==================================*/
void TIM15_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM15, TIM_IT_Update) == SET){
        TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
        
        // if there is no ir signal for 300 ms, reset data
		IRDataCounter = 0;
		currDataIndex = 0;
		IRData1 = 0;
		IRData2 = 0;
		TIM_Cmd(TIM15, DISABLE);
    }
}

void TIM1_CC_IRQHandler(void)
{
    // pult testing
//    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET){
//        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
//		if(currDataIndex < 200) {
//			curDataElem = TIM_GetCapture1(TIM1);
//			currData[currDataIndex] = abs(curDataElem - prevDataElem);
//			prevDataElem = curDataElem;
//			currDataIndex++;
//        } else {
//			currDataIndex += 0;
//		}
//    }
    // real work
    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET){
		TIM_Cmd(TIM15, DISABLE);
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
        if(isIrReceiveEn){
            if(currDataIndex > 130){
                IRDataCounter = 0;
                currDataIndex = 0;
                IRData1 = 0;
                IRData2 = 0;
            }
            
            curDataElem = TIM_GetCapture1(TIM1);
            if(currDataIndex){
                currData[currDataIndex-1] = curDataElem - prevDataElem;
                prevDataElem = curDataElem;
                   
                // Definition preambula
                if(currDataIndex == 1){
                    IRData1 = 0;
                    IRData2 = 0;
                    IRDataCounter = 0;
                    irReceiveProt.preambula = currData[currDataIndex-1];
                    
                    // Max preambula len (error)
                    if(currData[currDataIndex-1] > 3000){
                        IRDataCounter = 0;
                        currDataIndex = 0;
                        IRData1 = 0;
                        IRData2 = 0;
                        return;
                    }
                    
                    // NEC preambula
                    if((currData[currDataIndex-1] > 1100) && (currData[currDataIndex-1] < 1400)){
                        irReceiveProt.currProtocolType = IR_PROTOCOL_TYPE_NEC;

                    // Panasonic preambula
                    } else if((currData[currDataIndex-1] > 510) && (currData[currDataIndex-1] < 530)){
                        irReceiveProt.currProtocolType = IR_PROTOCOL_TYPE_PANASONIC;

                    // Unknown preambula space
                    } else{
                        irReceiveProt.currProtocolType = IR_PROTOCOL_TYPE_UNKNOWN;
                    }

                // Definition data bits
                } else{
                    if((currData[currDataIndex-1] > 60) && (currData[currDataIndex-1] < 125)){
                        if(IRDataCounter < 64){
                            IRData1 = (IRData1 << 1);
                        } else {
                            IRData2 = (IRData2 << 1);
                        }
                        IRDataCounter++;

                    } else if((currData[currDataIndex-1] > 160) && (currData[currDataIndex-1] < 240)){
                        if(IRDataCounter < 64){
                            IRData1 = (IRData1 << 1) | 1;
                        } else {
                            IRData2 = (IRData2 << 1) | 1;
                        }
                        IRDataCounter++;
                    }
                }
            }
            prevDataElem = curDataElem;
            
            // check result
            if(IRDataCounter == NEC_BITS &&  irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_NEC){
                if(irReceiveProt.receiveType == IR_RECEIVE_TYPE_NORMAL){
                    switch(IRData1){
                        case IR_BUTTON_TOP:
                            IR_curButton = BUTTON_TOP;
                            break;
                        case IR_BUTTON_BOTOOM:
                            IR_curButton = BUTTON_BOTOOM;
                            break;
                        case IR_BUTTON_RIGHT:
                            IR_curButton = BUTTON_RIGHT;
                            break;
                        case IR_BUTTON_LEFT:
                            IR_curButton = BUTTON_LEFT;
                            break;
                        case IR_BUTTON_SELECT:
                            IR_curButton = BUTTON_SELECT;
                            break;
                        case IR_BUTTON_BACK:
                            IR_curButton = BUTTON_BACK;
                            break;
                        case IR_GREEN_LED:
							LED_GREEN(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_GREEN_PIN));
                            //GPIO_WriteBit(GPIOC, LD4_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD4_PIN)));
                            break;
                        case IR_BLUE_LED:
							LED_BLUE(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_BLUE_PIN));
                            //GPIO_WriteBit(GPIOC, LD3_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD3_PIN)));
                            break;
                        
                        case IR_SONG_PAUSE_PLAY:
                            musicSet.isSongPlay = !musicSet.isSongPlay;
                            break;
                        case IR_SONG_SW_MT:
                            playBackgroundSong(SPEAKER_TYPE_BIG, StarWars_MainTheme, StarWars_MainTheme_Beats, sizeof(StarWars_MainTheme)/2, 108,0);
                            break;
                        case IR_SONG_PIRATES:
                            playBackgroundSong(SPEAKER_TYPE_BIG, pirates, pirates_Beates, sizeof(pirates)/2, 140,0);
                            break;
                        case IR_SONG_ELISE:
                            playBackgroundSong(SPEAKER_TYPE_BIG, FurElise, FurElise_Beates, sizeof(FurElise)/2, 90,0);
                            break;
                        case IR_SONG_TETRIS:
                            playBackgroundSong(SPEAKER_TYPE_BIG, TetrisGameSong, TetrisGameSong_Beats, sizeof(TetrisGameSong)/2, 140,1);
                            musicSet.isCyclickSong = 1;
                            break;
                        
                        default:
                            IR_curButton = 0;
                    }
                } else if(irReceiveProt.receiveType == IR_RECEIVE_TYPE_TEST){
                    irReceiveProt.receiveIRData1 = IRData1;
                    irReceiveProt.dataReceiveCount = IRDataCounter;
                    irReceiveProt.isProtocolReceive = 1;
                }
                IRDataCounter = 0;
                currDataIndex = 0;
                IRData1 = 0;
                
            } else if(IRDataCounter == PANASONIC_BITS && irReceiveProt.currProtocolType == IR_PROTOCOL_TYPE_PANASONIC){
                if(irReceiveProt.receiveType == IR_RECEIVE_TYPE_NORMAL){
                    switch(IRData1){
                        case IR_PANASONIC_BUTTON_TOP:
                            IR_curButton = BUTTON_TOP;
                            break;
                        case IR_PANASONIC_BUTTON_BOTOOM:
                            IR_curButton = BUTTON_BOTOOM;
                            break;
                        case IR_PANASONIC_BUTTON_RIGHT:
                            IR_curButton = BUTTON_RIGHT;
                            break;
                        case IR_PANASONIC_BUTTON_LEFT:
                            IR_curButton = BUTTON_LEFT;
                            break;
                        case IR_PANASONIC_BUTTON_SELECT:
                            IR_curButton = BUTTON_SELECT;
                            break;
                        case IR_PANASONIC_BUTTON_BACK:
                            IR_curButton = BUTTON_BACK;
                            break;
                        case IR_PANASONIC_GREEN_LED:
							LED_GREEN(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_GREEN_PIN));
                            //GPIO_WriteBit(GPIOC, LD4_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD4_PIN)));
                            break;
                        case IR_PANASONIC_BLUE_LED:
							LED_BLUE(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_BLUE_PIN));
                            //GPIO_WriteBit(GPIOC, LD3_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD3_PIN)));
                            break;
                        
                        case IR_PANASONIC_SONG_PLAY:
                            musicSet.isSongPlay = 1;
                            break;
                        case IR_PANASONIC_SONG_PAUSE:
                            musicSet.isSongPlay = 0;
                            break;
                        case IR_PANASONIC_SONG_SW_MT:
                            playBackgroundSong(SPEAKER_TYPE_BIG, StarWars_MainTheme, StarWars_MainTheme_Beats, sizeof(StarWars_MainTheme)/2, 108,0);
                            break;
                        case IR_PANASONIC_SONG_PIRATES:
                            playBackgroundSong(SPEAKER_TYPE_BIG, pirates, pirates_Beates, sizeof(pirates)/2, 140,0);
                            break;
                        case IR_PANASONIC_SONG_ELISE:
                            playBackgroundSong(SPEAKER_TYPE_BIG, FurElise, FurElise_Beates, sizeof(FurElise)/2, 90,0);
                            break;
                        case IR_PANASONIC_SONG_TETRIS:
                            playBackgroundSong(SPEAKER_TYPE_BIG, TetrisGameSong, TetrisGameSong_Beats, sizeof(TetrisGameSong)/2, 140,1);
                            musicSet.isCyclickSong = 1;
                            break;
                        
                        default:
                            IR_curButton = 0;
                    }
                } else if(irReceiveProt.receiveType == IR_RECEIVE_TYPE_TEST){
                    irReceiveProt.receiveIRData1 = IRData1;
                    irReceiveProt.receiveIRData2 = IRData2;
                    irReceiveProt.dataReceiveCount = IRDataCounter;
                    irReceiveProt.isProtocolReceive = 1;
                }
                IRDataCounter = 0;
                currDataIndex = 0;
                IRData1 = 0;
                IRData2 = 0;
                
            
            // maximum protocol len = 128
            }else if(IRDataCounter == 128){
                if(irReceiveProt.receiveType == IR_RECEIVE_TYPE_TEST){
                    irReceiveProt.receiveIRData1 = IRData1;
                    irReceiveProt.receiveIRData2 = IRData2;
                    irReceiveProt.dataReceiveCount = IRDataCounter;
                    irReceiveProt.isProtocolReceive = 1;
                }
                IRDataCounter = 0;
                currDataIndex = 0;
                IRData1 = 0;
                IRData2 = 0;
            }else{
                currDataIndex++;
            }
            
			TIM15->CNT = 0;
            TIM_Cmd(TIM15, ENABLE);
        }
    }
}

/* Инициализация пинов ИК-приемника (PA8/TIM1_CH1) и ИК-передатчика (PA11), а также TIM1 и TIM15*/
void irReseivTransInit(void)
{
	// ========== Инициализация ИК-приемника PA8 - AF2 - TIM1_CH1 ==========
    RCC_AHBPeriphClockCmd(RCC_AHBENR_IREN, ENABLE);  // Тактирование ИК-порта (GPIOA)
    GPIO_InitTypeDef GPIOx_ini;                      // Структура хранения настроек порта
    
	// Настраиваем порт
    GPIOx_ini.GPIO_Pin = IR_RECEIV_PIN;       // Настраев пин ИК-приемника PA8
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;       // Режим работы порта: альтернативная функция
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;  // Скорость (частота) работы порта: 10 МГц
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;     // Режим работы выхода порта: двухтактный (push-pull)
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_DOWN;     // Подтяжка к питанию или к земле: подтяжка к земле
    GPIO_Init(IR_PORT, &GPIOx_ini);           // Инициализация порта
    
	// Включаем в качестве альтернативной функции TIM1_CH1
	GPIO_PinAFConfig(GPIOA, IR_RECEIV_PIN_SOURCE, GPIO_AF_2);
	
    // Включаем тактирование таймера TIM1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
	// Настройка таймера на счет в 100 мкс
	TIM_TimeBaseInitTypeDef TIMx_ini;
    TIMx_ini.TIM_Prescaler = SystemCoreClock/100000 - 1;  // Пределитель таймера (480-1): считает 100 мкс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;        // Направление счета: вверх
    TIMx_ini.TIM_Period = 0xFFFF;                         // Период счета: максимальный - 0xFFFF
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;            // Делаитель частоты: 1
    TIM_TimeBaseInit(TIM1, &TIMx_ini);                    // Инициализация таймера
    
    // Инициализация канала таймера
    TIM_ICInitTypeDef TIMx_ICinit;
    TIMx_ICinit.TIM_Channel = TIM_Channel_1;                 // Выбранный канал: первый
    TIMx_ICinit.TIM_ICFilter = 4;                            // Входной фильтр: подтверждение наличие сигнала после 4-х циклов
    TIMx_ICinit.TIM_ICPolarity = TIM_ICPolarity_Falling;     // Активный уровень захвата: между двумя спадами
    TIMx_ICinit.TIM_ICPrescaler = TIM_ICPSC_DIV1;            // Дополнительный предделитель: 1 (захватывать каждый сигнал)
    TIMx_ICinit.TIM_ICSelection = TIM_ICSelection_DirectTI;  // Направление канала: прямое
    TIM_ICInit(TIM1, &TIMx_ICinit);

	// Включение прерываний
    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);  // Включаем прерывания по захвату с первого канала
    NVIC_EnableIRQ(TIM1_CC_IRQn);            // Включаем прерывания первого таймера по захвату
    TIM_Cmd(TIM1, ENABLE);                   // Включаем таймер 1
    
	// ========== Инициализация таймера для time out приема (TIM15) ==========
    // Если сигнала нет больше 50 мс, то закончить прием данных
	
	// Включаем тактирование таймера TIM15
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
    
	// Настройка TIM15
    TIMx_ini.TIM_Prescaler = SystemCoreClock/1000 - 1;  // Пределитель таймера (48000-1): считает 1 мс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;      // Направление счета: вверх
    TIMx_ini.TIM_Period = 50;                           // Период счета: считаем до 50 мс
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;          // Делаитель частоты: 1
    TIM_TimeBaseInit(TIM15, &TIMx_ini);                 // Инициализация таймера
    
	// Включение прерываний
    TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);  // Прерывание на достижение периода
    NVIC_EnableIRQ(TIM15_IRQn);                  // Включаем прерывания TIM15
    TIM_Cmd(TIM15, DISABLE);                     // Включаем TIM15 
	
    // ========== Инициализация ИК-передатчика PA11 ==========
	// Включаем тактирование порта
    RCC_AHBPeriphClockCmd(RCC_AHBENR_IREN, ENABLE);
	
	// Настраиваем порт
    GPIOx_ini.GPIO_Pin = IR_TRANS_PIN;        // Настраев пин ИК-передатчика PA11
    GPIOx_ini.GPIO_Mode = GPIO_Mode_OUT;      // Режим работы порта: выход
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;  // Скорость (частота) работы порта: 10 МГц
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;     // Режим работы выхода порта: двухтактный (push-pull)
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;   // Подтяжка к питанию или к земле: без подтяжки
    GPIO_Init(IR_PORT, &GPIOx_ini);           // Инициализация порта
}
//
//==========NEC========================
void mark(unsigned int time){
    unsigned int i;
    for(i = time/26; i > 0; i--){
        IR_PORT->BSRR = IR_TRANS_PIN;
        delayUs(13);
        IR_PORT->BRR = IR_TRANS_PIN;
        delayUs(13);
    }
}

void space(unsigned int time){
    IR_PORT->BRR = IR_TRANS_PIN;
	if(time > 0) delayUs(time);
}

void sendNEC (uint32_t data){
    // Header
	mark(NEC_HDR_MARK);
	space(NEC_HDR_SPACE);
    
    // Data
    uint32_t  mask;
	for (mask = (uint32_t)1 << (32 - 1);  mask > 0;  mask >>= 1) {
		if (data & mask) {
			mark(NEC_BIT_MARK);
			space(NEC_ONE_SPACE);
		} else {
			mark(NEC_BIT_MARK);
			space(NEC_ZERO_SPACE);
		}
	}

	// Footer
	mark(NEC_BIT_MARK);
	space(0);  // Always end with the LED off
}

void sendPanasonic(uint64_t data){
    // Header
	mark(PANASONIC_HDR_MARK);
	space(PANASONIC_HDR_SPACE);
    
    // Data
    uint64_t  mask;
	for (mask = (uint64_t)1 << (PANASONIC_BITS - 1);  mask > 0;  mask >>= 1) {
		if (data & mask) {
			mark(PANASONIC_BIT_MARK);
			space(PANASONIC_ONE_SPACE);
		} else {
			mark(PANASONIC_BIT_MARK);
			space(PANASONIC_ZERO_SPACE);
		}
	}

	// Footer
	mark(PANASONIC_BIT_MARK);
	space(0);  // Always end with the LED off
}

