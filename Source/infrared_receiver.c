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
static uint16_t currData[140] = {0};                       // Contains pulse durations (start puls, 0 or 1)
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
void TIM15_IRQHandler(void){
    if(TIM_GetITStatus(TIM15, TIM_IT_Update) == SET){
        TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
        
        // if there is no ir signal for 300 ms, reset data
        if(lastTIM15CNT == 0){
            IRDataCounter = 0;
            currDataIndex = 0;
            IRData1 = 0;
            IRData2 = 0;
            TIM_Cmd(TIM15, DISABLE);
        }
        lastTIM15CNT = 0;
    }
}

void TIM3_IRQHandler(void){
    // pult testing
//    if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET){
//        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
//        curDataElem = TIM_GetCapture1(TIM3);
//        if(currDataIndex){
//            currData[currDataIndex-1] = abs(curDataElem - prevDataElem);
//        }
//        prevDataElem = curDataElem;
//        currDataIndex++;
//        if(curDataElem > 1000 && currDataIndex > 2){
//            currDataIndex+= 0;
//        }
//    }
    // real work
    if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET){
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
        if(isIrReceiveEn){
            if(currDataIndex > 130){
                IRDataCounter = 0;
                currDataIndex = 0;
                IRData1 = 0;
                IRData2 = 0;
            }
            
            curDataElem = TIM_GetCapture1(TIM3);
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
            
            lastTIM15CNT = TIM15->CNT;
            TIM_Cmd(TIM15, ENABLE);
        }
    }
}

void irReseivTransInit(){
    /*---IR receiver init PC6 - AF2 - TIM2_CH3*/
    RCC_AHBPeriphClockCmd(RCC_AHBENR_IREN, ENABLE);
    GPIO_InitTypeDef GPIOx_ini;
    
    GPIOx_ini.GPIO_Pin = IR_RECEIV_PIN;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;        // PC6 - AF - TIM3_CH1
    GPIOx_ini.GPIO_Speed = GPIO_Speed_2MHz;   // 2, 10, 50 ̃ö
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(IR_PORT, &GPIOx_ini);
    
    
    
    // timer2 settings
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM3_ini;
    
    TIM3_ini.TIM_Prescaler =  480 - 1;
    TIM3_ini.TIM_CounterMode = TIM_CounterMode_Up;
    TIM3_ini.TIM_Period = 0xFFFF;
    TIM3_ini.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM3_ini);
    
    // timer's chanell settings
    TIM_ICInitTypeDef TIM3_ICinit;
    TIM3_ICinit.TIM_Channel = TIM_Channel_1;
    TIM3_ICinit.TIM_ICFilter = 4;
    TIM3_ICinit.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM3_ICinit.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM3_ICinit.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM3, &TIM3_ICinit);

    // enable interrupts
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
    TIM_Cmd(TIM3, ENABLE);             // enable timer 2
    
    NVIC_EnableIRQ(TIM3_IRQn);
    
    
    /*-----------------Transmitter IR-diode init (PC7)--------------*/
    RCC_AHBPeriphClockCmd(RCC_AHBENR_IREN, ENABLE);
    GPIOx_ini.GPIO_Pin = IR_TRANS_PIN;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_OUT;
    GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;   // 2, 10, 50
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(IR_PORT, &GPIOx_ini);
    
    // Timer for correct receiver
    // if there is no signal for 300 ms, reset data
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
    TIM_TimeBaseInitTypeDef TIM15_ini;
    
    TIM15_ini.TIM_Prescaler =  48000 - 1;
    TIM15_ini.TIM_CounterMode = TIM_CounterMode_Up;
    TIM15_ini.TIM_Period = 500;
    TIM15_ini.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM15, &TIM15_ini);
    
    TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM15_IRQn);
    
    TIM_Cmd(TIM15, DISABLE);
}
//
//==========NEC========================
void mark(unsigned int time){
	//TIM_Cmd(TIM15, ENABLE);
    //if (time > 0) delayUs(time);
    
    unsigned int i;
    for(i = time/26; i > 0; i--){
        IR_PORT->BSRR = IR_TRANS_PIN;
        delayUs(13);
        IR_PORT->BRR = IR_TRANS_PIN;
        delayUs(13);
    }
}

void space(unsigned int time){
	//TIM_Cmd(TIM15, DISABLE);
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

