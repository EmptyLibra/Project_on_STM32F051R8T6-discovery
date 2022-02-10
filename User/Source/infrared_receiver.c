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
static uint16_t currData[64] = {0};                       // Contains pulse durations (start puls, 0 or 1)
static uint8_t currDataIndex = 0;
static uint16_t prevDataElem = 0, curDataElem = 0;


static uint32_t IRData = 0;                               // Contains current ir command like: 0x00FF9A65
static uint8_t IRDataCounter = 0;
uint16_t IR_curButton = 0;
static uint32_t lastTIM15CNT = 0;
//
/*===========================Functions==================================*/
void TIM15_IRQHandler(void){
    if(TIM_GetITStatus(TIM15, TIM_IT_Update) == SET){
        TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
        
        // if there is no ir signal for 300 ms, reset data
        if(lastTIM15CNT == 0){
            IRDataCounter = 0;
            currDataIndex = 0;
            IRData = 0;
            TIM_Cmd(TIM15, DISABLE);
        }
        lastTIM15CNT = 0;
    }
}

void TIM2_IRQHandler(void){
    // pult testing
//    if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET){
//        TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
//        curDataElem = TIM_GetCapture3(TIM2);
//        if(currDataIndex > 0){
//            currData[currDataIndex] = curDataElem - prevDataElem;
//            prevDataElem = curDataElem;
//        } else{
//            currData[currDataIndex] = curDataElem;;
//            prevDataElem = curDataElem;
//        }
//        currDataIndex++;
//    }
    // real work
    if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET){
        if(currDataIndex > 60){
            IRDataCounter = 0;
            currDataIndex = 0;
            IRData = 0;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
        curDataElem = TIM_GetCapture3(TIM2);
        if(currDataIndex > 0){
            currData[currDataIndex] = curDataElem - prevDataElem;
            prevDataElem = curDataElem;
            if((currData[currDataIndex] > 460) && (currData[currDataIndex] < 520)){
                IRData = 0;
                IRDataCounter = 0;
            } else if((currData[currDataIndex] > 100) && (currData[currDataIndex] < 125)){
                if(IRDataCounter < 32){
                    IRData = IRData << 1;
                    IRDataCounter++;
                }                
            } else if((currData[currDataIndex] > 210) && (currData[currDataIndex] < 240)){
                if(IRDataCounter < 32){
                    IRData = (IRData << 1) | 1;
                    IRDataCounter++;
                }
            }
        } else {
            currData[currDataIndex] = curDataElem;;
            prevDataElem = curDataElem;
        }
        if(IRDataCounter == 32){
            switch(IRData){
                case IR_BUTTON_TOP:
                case IF_FUNAI_BUTTON_TOP:
                    IR_curButton = BUTTON_TOP;
                    break;
                case IR_BUTTON_BOTOOM:
                case IF_FUNAI_BUTTON_BOTTOM:
                    IR_curButton = BUTTON_BOTOOM;
                    break;
                case IR_BUTTON_RIGHT:
                case IF_FUNAI_BUTTON_RIGHT:
                    IR_curButton = BUTTON_RIGHT;
                    break;
                case IR_BUTTON_LEFT:
                case IF_FUNAI_BUTTON_LEFT:
                    IR_curButton = BUTTON_LEFT;
                    break;
                case IR_BUTTON_SELECT:
                case IR_FUNAI_BUTTON_OK:
                    IR_curButton = BUTTON_SELECT;
                    break;
                case IR_BUTTON_BACK:
                case IF_FUNAI_BUTTON_BACK:
                    IR_curButton = BUTTON_BACK;
                    break;
                case IR_GREEN_LED:
                case IF_FUNAI_GREEN_LED:
                    GPIO_WriteBit(GPIOC, LD4_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD4_PIN)));
                    break;
                case IR_BLUE_LED:
                case IF_FUNAI_BLUE_LED:
                    GPIO_WriteBit(GPIOC, LD3_PIN, (BitAction)(!GPIO_ReadOutputDataBit(GPIOC, LD3_PIN)));
                    break;
                
                case IR_SONG_PAUSE:
                    musicSet.isSongPlay = 0;
                    break;
                case IR_SONG_PLAY:
                    musicSet.isSongPlay = 1;
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
            IRDataCounter = 0;
            currDataIndex = 0;
            IRData = 0;
        } else{
            currDataIndex++;
        }
        lastTIM15CNT = TIM15->CNT;
        TIM_Cmd(TIM15, ENABLE);
    }
}

void IR_init(){
    /*---IR receiver init PB10 - AF2 - TIM2_CH3*/
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
    GPIO_InitTypeDef GPIOx_ini;
    
    GPIOx_ini.GPIO_Pin = IR_RECEIV_PIN;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;
    GPIOx_ini.GPIO_Speed = GPIO_Speed_2MHz;   // 2, 10, 50 ̃ö
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(IR_PORT, &GPIOx_ini);
    
    // PB10 - AF2 - TIM2_CH3
    GPIO_PinAFConfig(IR_PORT, IR_RECEIV_PIN_SOURCE, GPIO_AF_2);
    
    // timer2 settings
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef TIM2_ini;
    
    TIM2_ini.TIM_Prescaler =  480 - 1;
    TIM2_ini.TIM_CounterMode = TIM_CounterMode_Up;
    TIM2_ini.TIM_Period = 0xFFFF;
    TIM2_ini.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM2_ini);
    
    // timer's chanell settings
    TIM_ICInitTypeDef TIM2_ICinit;
    TIM2_ICinit.TIM_Channel = TIM_Channel_3;
    TIM2_ICinit.TIM_ICFilter = 4;
    TIM2_ICinit.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM2_ICinit.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM2_ICinit.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM2, &TIM2_ICinit);

    // enable interrupts
    TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
    TIM_ClearFlag(TIM2, TIM_FLAG_CC3);
    TIM_Cmd(TIM2, ENABLE);             // enable timer 2
    
    NVIC_EnableIRQ(TIM2_IRQn);
    
    
    /*-----------------Transmitter IR-diode init (PB14 - AF1 - TIM15_CH1)--------------*/
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
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
    TIM15_ini.TIM_Period = 300;
    TIM15_ini.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM15, &TIM15_ini);
    
    TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM15_IRQn);
    
    TIM_Cmd(TIM15, DISABLE);
    
//    GPIOx_ini.GPIO_Pin = IR_TRANS_PIN;
//    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;
//    GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;   // 2, 10, 50
//    GPIOx_ini.GPIO_OType = GPIO_OType_PP;
//    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(IR_PORT, &GPIOx_ini);
//    
//    // PB14 - AF1 - TIM15_CH1
//    GPIO_PinAFConfig(IR_PORT, IR_TRANS_PIN_SOURCE, GPIO_AF_1);
    
//    // timer15 settings - Slave
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);            //TIM15 clock enable
//    TIM_TimeBaseInitTypeDef  TIM15_ini;
//    TIM15_ini.TIM_Period             = 1264;              // clockFreq/receiverFreq = 48 000 000/ 38 000 = 1263,15789
//    TIM15_ini.TIM_Prescaler          = 0;
//    TIM15_ini.TIM_ClockDivision      = TIM_CKD_DIV1;
//    TIM15_ini.TIM_CounterMode        = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM15, &TIM15_ini);
//    
//    TIM_OCInitTypeDef        TIM_OCini;
//    TIM_OCini.TIM_OCMode         = TIM_OCMode_PWM1;
//    TIM_OCini.TIM_OutputState    = TIM_OutputState_Enable;
//    TIM_OCini.TIM_Pulse          = 632;                      //Capture/Compare 1 value (ARR/2 = 1264/2 = 632)
//    TIM_OCini.TIM_OCPolarity     = TIM_OCPolarity_High; 
//    TIM_OC1Init(TIM15, &TIM_OCini);

//    TIM_Cmd(TIM15, DISABLE);
}
//
//==========NEC========================
void mark(unsigned int time){
	//TIM_Cmd(TIM15, ENABLE);
    //if (time > 0) delay(time);
    
    unsigned int i;
    for(i = time/26; i > 0; i--){
        IR_PORT->BSRR = IR_TRANS_PIN;
        delay(13);
        IR_PORT->BRR = IR_TRANS_PIN;
        delay(13);
    }
}

void space(unsigned int time){
	//TIM_Cmd(TIM15, DISABLE);
    IR_PORT->BRR = IR_TRANS_PIN;
	if(time > 0) delay(time);
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

