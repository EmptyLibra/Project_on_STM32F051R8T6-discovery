/** Using UTF8 file encoding
  * @file    ir_remote_control.c
  * @author  Shalaev Egor
  * @version V0.0.2
  * @date    17-June-2022
  * @brief   Эта библиотека реализует прием сигналов от ИК-пультов и их передачу по протоколам NEC и Panasonic.
  *          Сигнал принимается от ИК-приемника, с использованием таймера в режиме захвата, а также 
  *          передается ИК-светодиодом с помощью ШИМ на несущей частоте 38 кГц.
  * =================================
  * Эта библиотека распознает два протокола: расширенный протокол NEC, а также протокол Panasonic (REC80, JAP).
  * =================================
  * ИК сигнал представляет из себя сигнал с несущей частотой (обычно) в 38 кГц, который модулируется с помощью ШИМ
  * по специальным протоколам. Протоколы используют либо манчестерское кодирование, либо кодирование временными интервалами.
  * Схема моего ИК-приемника в корпусе TSOP4xx выглядит следующим обращом:
  *   |----------|
  *   |    __    |
  *   |   /  \   |
  *   |   \__/   |
  *   |----------|
  *     |  |   |
  *     |  |   |
  *    V0 GND  5V   
  * =================================
  * Пример ИК-протокола на примере NEC (сначала идёт преамбула, затем информационные биты и в конце импульс окончания):
  *  ___________        1  0 0 1          1
  * |           |       |  | | |          |                                         | <--- импульс окончания посылки
  * |           |       |  | | |          |                                         |
  * |           |_______|__|_|_|__........|__.......................................|
  * <----9ms----><-4ms-><--16 bit Адреса--><-8 bit Command-><-8 bit invers Command->
  *
  * Кодирование бит длительностью паузы (набор черточек - это сигнал с частотой 38 кГц):
  * <-------Logical 1-----><-Logical 0--> 
  * |||||||                |||||||       |||||||   <--- импульс окончания посылки
  * |||||||                |||||||       |||||||
  * |||||||________________|||||||_______|||||||
  * <560us>                <560us><560us><560us>
  * <-------2.25ms--------><--1.125ms--->             */
 
#include "ir_remote_control.h"

/*============================== Структуры и переменные ==============================*/
// fifo, содержащее временные интервалы сигналов
FIFO_SHORT_DEFINITION(uint16_t, 128, irDataFifo);

// Стурктура для хранения информации о принятом протоколе
IR_ReceiveProt irProtocol;

// Структура для отправки ИК-команды
IR_TransmittConf irTransmittConf;

// Ик-кнопка
uint16_t IR_curButton = 0;

// Текущий ИК-режим и текущее состояние приема (включено или выключено)
uint8_t irReceiveType = IR_RECEIVE_TYPE_RUN_COMMAND, irReceiveState = ENABLE;

/*============================== Функции ==============================*/
/* Прерывание таймера, модулирующего ИК-команду */
void TIM15_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM15, TIM_IT_Update) == SET)
	{
		// Очищаем флаг прерывания
        TIM_ClearITPendingBit(TIM15, TIM_IT_Update);
		
		// Отправка преамбулы
		if(irTransmittConf.nextProtocolPart == IR_SEND_PREAMBULA) {
			
			// Меняем полярность сигнала
			irTransmittConf.polatitySignalNow ^= 1;
			
			// В следующем прерывании отправим 1-ый бит информации
			irTransmittConf.nextProtocolPart = IR_SEND_INF_BITS;
			
			// Устанавливаем длительность паузы преамбулы
			if(irProtocol.type == IR_PROTOCOL_TYPE_NEC && irProtocol.repeatLastCmd) {
				// При повторе NEC только преамбула
				TIM_SetAutoreload(TIM15, NEC_RPT_SPACE);
			} else {
				TIM_SetAutoreload(TIM15, irProtocol.preambuleTime - TIM15->ARR);
			}
			
			// Маска для взятие битов команды
			irTransmittConf.curBitMask = ((uint64_t)1 << (irProtocol.bitsCount - 1));
			
			// Отключаем ШИМ
			IR_PWM_STATE(DISABLE);
		
		// Отправка информационных бит (старшими вперёд)
		} else {
			// Если отправляем повторную команду NEC
			if(irProtocol.type == IR_PROTOCOL_TYPE_NEC && irProtocol.repeatLastCmd) {
				// Выключаем таймер и ШИМ, обнуляем счетчик
				TIM_Cmd(TIM15, DISABLE);
				IR_PWM_STATE(DISABLE);
				TIM15->CNT = 0;
				
				// Выставляем флаг об окончании отправки
				irTransmittConf.flagTransmitWork = 0;
			}
			
			// Если ещё не дошли до конца команды
			if(irTransmittConf.curBitMask) {
				// Отправка нуля или единицы
				if(irTransmittConf.polatitySignalNow == 1) {
					// Меняем полярность сигнала
					irTransmittConf.polatitySignalNow ^= 1;
					
					// Устанавливаем кол-во периодов пробела, в зависимости от 0 или 1 комады
					TIM_SetAutoreload(TIM15, irProtocol.bitMarkLen*(irProtocol.irCommand & irTransmittConf.curBitMask ? 3 : 1)); 
					irTransmittConf.curBitMask >>= 1;
					
					// Включаем ШИМ
					IR_PWM_STATE(DISABLE);
				} else {
					// Длительность одной пачки импульсов
					TIM_SetAutoreload(TIM15, irProtocol.bitMarkLen);
					
					// Меняем полярность сигнала
					irTransmittConf.polatitySignalNow ^= 1;
					
					// Включаем ШИМ
					IR_PWM_STATE(ENABLE);
				}
			} else {
				// Отправляем заключительную посылку
				if(irTransmittConf.polatitySignalNow == 0) {
					irTransmittConf.polatitySignalNow = 1;
					TIM_SetAutoreload(TIM15, irProtocol.bitMarkLen);
					IR_PWM_STATE(ENABLE);
				
				// Или заканчиваем отправку
				} else {
					// Выключаем таймер и ШИМ, обнуляем счетчик
					TIM_Cmd(TIM15, DISABLE);
					IR_PWM_STATE(DISABLE);
					TIM15->CNT = 0;
					
					// Выставляем флаг об окончании отправки
					irTransmittConf.flagTransmitWork = 0;
				}
			}
		}
	}
}
/* Прерывание по захвату временных инттервалов ИК-сигналов с приемника */
void TIM1_CC_IRQHandler(void)
{
	// Предудущий и текущий элементы (значения счетчика)
	static uint16_t predElem = 0;
	uint16_t curElem = 0;
	
	if(TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET)
	{
		// Очищаем флаг прерывания
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
		
		// Записываем данные в fifo
		if(!FIFO_IS_FULL(irDataFifo) && irReceiveState == ENABLE) 
		{
			// Считываем текущее значение счетчика
			curElem = TIM_GetCapture1(TIM1);
			
			/* Если сигнал не превышает максимальный временной интервал, 
			 * то записываем в fifo разность между текущим значение и предыдущим */
			if(abs(curElem-predElem) < MAX_IR_SIGNAL_LEN) 
			{
				// Записываем в fifo новый элемент только, если в fifo ещё не достаточно байт для протокола
				if((irProtocol.type != 0) && (FIFO_ELEMENTS_COUNT(irDataFifo) <= irProtocol.bitsCount-irProtocol.irCmdBitCounter)) 
				{
					FIFO_PUSH(irDataFifo, abs(curElem-predElem));
				} else if(irProtocol.type == 0)
				{
					FIFO_PUSH(irDataFifo, abs(curElem-predElem));
				}
			}
			
			// Запоминаем предудущий элемент
			predElem = curElem;
		}
	}
    // Тестирование пульта (просто запись всего в fifo)
//    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET) {
//		TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
//		if(!FIFO_IS_FULL(irDataFifo)) {
//			FIFO_PUSH(irDataFifo, TIM_GetCapture1(TIM1));
//		}
//    }
}

/* Инициализация пинов ИК-приемника (PA8/TIM1_CH1) и ИК-передатчика (PС7/TIM3_CH2), а также TIM1, TIM3, TIM15 */
void irReseivTransInit(void)
{
	// ========== Инициализация ИК-приемника PA8 - AF2 - TIM1_CH1 ==========
    RCC_AHBPeriphClockCmd(RCC_AHBENR_IR_RECEIVER_EN, ENABLE);  // Тактирование ИК-порта (GPIOA)
    GPIO_InitTypeDef GPIOx_ini;                      // Структура хранения настроек порта
    
	// Настраиваем порт
    GPIOx_ini.GPIO_Pin = IR_RECEIV_PIN;       // Настраев пин ИК-приемника PA8
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;       // Режим работы порта: альтернативная функция
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;  // Скорость (частота) работы порта: 10 МГц
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;     // Режим работы выхода порта: двухтактный (push-pull)
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_DOWN;     // Подтяжка к питанию или к земле: подтяжка к земле
    GPIO_Init(IR_RECEIVER_PORT, &GPIOx_ini);  // Инициализация порта
    
	// Включаем в качестве альтернативной функции TIM1_CH1
	GPIO_PinAFConfig(IR_RECEIVER_PORT, IR_RECEIV_PIN_SOURCE, GPIO_AF_2);
	
    // Включаем тактирование таймера TIM1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
	// Настройка таймера на счет в 100 мкс
	TIM_TimeBaseInitTypeDef TIMx_ini;
    TIMx_ini.TIM_Prescaler = SystemCoreClock/100000 - 1;  // Пределитель таймера (480-1): считает 100 мкс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;        // Направление счета: вверх
    TIMx_ini.TIM_Period = 0xFFFF;                         // Период счета: максимальный - 0xFFFF
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;            // Делаитель частоты: 1
    TIM_TimeBaseInit(TIM1, &TIMx_ini);                    // Инициализация таймера
    
    // Инициализация канала таймера (в режиме захвата)
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
	
	
    // ========== Инициализация ИК-передатчика PС7/TIM3_CH2 ==========
	// Включаем тактирование порта
    RCC_AHBPeriphClockCmd(RCC_AHBENR_IR_TRANSMIT_EN, ENABLE);
	
	// Настраиваем порт
    GPIOx_ini.GPIO_Pin = IR_TRANS_PIN;        // Настраев пин ИК-передатчика PA11
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;       // Режим работы порта: альтернативная функция
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;  // Скорость (частота) работы порта: 10 МГц
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;     // Режим работы выхода порта: двухтактный (push-pull)
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;   // Подтяжка к питанию или к земле: без подтяжки
    GPIO_Init(IR_TRANSMIT_PORT, &GPIOx_ini);  // Инициализация порта
	
	// Включаем тактирование таймера TIM3
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
	// Настройка таймера на счет в мкс (настраиваем на период 26 мкс, т.е. 38 кГц)
    TIMx_ini.TIM_Prescaler = SystemCoreClock/1000000 - 1; // Пределитель таймера (48-1): считает 1 мкс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;        // Направление счета: вверх
    TIMx_ini.TIM_Period = 26-1;                           // Период счета: 26 мкс
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;            // Делаитель частоты: 1
    TIM_TimeBaseInit(TIM3, &TIMx_ini);                    // Инициализация таймера
	
	// Настраиваем ШИМ для передатика
	TIM_OCInitTypeDef TIMx_PWM;
	TIM_OCStructInit(&TIMx_PWM);
	
	// Настраиваем канал таймера на ШИМ со скважностью 50% (13 мкс импульс, 13 мкс тишина)
	TIMx_PWM.TIM_OCMode = TIM_OCMode_PWM1;                 // Режим канала: ШИМ1
	TIMx_PWM.TIM_OutputState = TIM_OutputState_Disable;    // Состояние выхода: шим выключен
	TIMx_PWM.TIM_OutputNState = TIM_OutputNState_Disable;  // Выход с обратной полярностью выключен
	TIMx_PWM.TIM_OCPolarity = TIM_OCPolarity_High;         // Полярность сигнала: импульс - это 5В
	TIMx_PWM.TIM_Pulse = 13;                               // Длительность импульса: 13 мкс
	
	// Инициализация 2-го канала таймера
	TIM_OC2Init(TIM3, &TIMx_PWM);                          

	// Включаем таймер, отвечающий за генерацию ШИМ
	TIM_Cmd(TIM3, ENABLE);
	
	// ========== Настройка таймера TIM15 на счет до 560 мкс (для генерации ШИМ пачек импульсов) ==========
	// Включаем тактирование таймера TIM15
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
    
	// Настройка TIM15
    TIMx_ini.TIM_Prescaler = SystemCoreClock/1000000 - 1;  // Пределитель таймера (48-1): считает 1 мкс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;         // Направление счета: вверх
    TIMx_ini.TIM_Period = 560;                             // Период счета: считаем до 560 мкс
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;             // Делитель частоты: 1
    TIM_TimeBaseInit(TIM15, &TIMx_ini);                    // Инициализация таймера
    
	// Включение прерываний
    TIM_ITConfig(TIM15, TIM_IT_Update, ENABLE);  // Прерывание на достижение периода
    NVIC_EnableIRQ(TIM15_IRQn);                  // Включаем прерывания TIM15
	TIM_Cmd(TIM15, DISABLE);
    // Таймер по-умолчанию выключен
	
	// Отключаем ШИМ
	IR_PWM_STATE(DISABLE);
	
	// Выключаем ИК-светодиод
	GPIO_WriteBit(IR_TRANSMIT_PORT, IR_TRANS_PIN, Bit_RESET);
}
/* Анализ ИК-протоколов (протоколы с манчестерским кодированием не анализируются!) */
void irProtocolAnalyze(void)
{
	// Задержка, чтобы команды не срабатывали слишком часто
	delayUs(1000);
	
	// Текущий элемент из fifo
	uint16_t curElem = 0;
	
	// Анализируем содержмое fifo
	while(!FIFO_IS_EMPTY(irDataFifo) || irProtocol.repeatLastCmd) 
	{
		//------ Анализ преамбулы протокола ------
		if(irProtocol.type == 0) 
		{
			// Получаем текущий элемент из fifo
			curElem = FIFO_POP(irDataFifo);
			
			// Ошибка в принятии приамбулы
			if((curElem < 300) || (curElem > 3000)) {
				continue;
			
			// Преамбула протокола NEC (13500 мкс)
			} else if((curElem > 1300) && (curElem < 1430)){
				irProtocol.type = IR_PROTOCOL_TYPE_NEC;
				irProtocol.bitsCount = NEC_COUNT_BITS;
				irProtocol.bitMarkLen = NEC_BIT_MARK;
				
			// Повторная команда от NEC (11250 мкс)
			}else if((curElem > 1100) && (curElem < 1200)){
				irProtocol.type = IR_PROTOCOL_TYPE_NEC;
				irProtocol.bitsCount = irProtocol.irCmdBitCounter = NEC_COUNT_BITS;
				irProtocol.irCommand = irProtocol.lastNecCommand;
				irProtocol.repeatLastCmd = 1;
				
			// Преамбула протокола Panasonic (5100 мкс)
			} else if((curElem > 510) && (curElem < 530)){
				irProtocol.type = IR_PROTOCOL_TYPE_PANASONIC;
				irProtocol.bitsCount = PANASONIC_COUNT_BITS;
				irProtocol.bitMarkLen = PANASONIC_BIT_MARK;
				
			// Преамбула неизвестного протокола
			} else{
				irProtocol.type = IR_PROTOCOL_TYPE_UNKNOWN;
				irProtocol.bitsCount = 32;
			}
			irProtocol.preambuleTime = curElem*10;		
		}
		
		//----- Анализ команды протокола------		
		else {
			// Если протокол получен не полностью
			if(irProtocol.irCmdBitCounter != irProtocol.bitsCount) 
			{
				// Получаем текущий элемент из fifo
				curElem = FIFO_POP(irDataFifo);
				
				// Проверяем, получили ли единицу (1600 мкс) или ноль (800 мкс)
				if((curElem > 160) && (curElem < 240)) 
				{
					// Если получили 1, то надо сдвинуть команду и прибавить 1
					irProtocol.irCommand <<= 1;
					irProtocol.irCommand |= 1;
					irProtocol.irCmdBitCounter++;
				} else if((curElem > 60) && (curElem < 125)) 
				{
					// Если получили 0, то просто сдвигаем код команды влево и увеличичваем счетчик битов
					irProtocol.irCommand <<= 1;
					irProtocol.irCmdBitCounter++;
				
				// Ошибка в протоколе
				} else {
					irProtocol.type = 0;
					irProtocol.irCommand = 0;
					irProtocol.irCmdBitCounter = 0;
					irProtocol.preambuleTime = 0;
					irProtocol.isFullReceive = 0;
					irProtocol.bitsCount = 0;
					continue;
				}
			}
			
			// Если протокол получен полностью, то выполняем команду
			if(irProtocol.irCmdBitCounter ==  irProtocol.bitsCount) 
			{
				// Выполнение команды в нормальном режиме работы
				if(irReceiveType == IR_RECEIVE_TYPE_RUN_COMMAND) 
				{
					// Команды протокола NEC
					if(irProtocol.type == IR_PROTOCOL_TYPE_NEC) 
					{
						// Запоминаем текущую команду NEC для дальнейшего повтора
						irProtocol.lastNecCommand = irProtocol.irCommand;
						
						switch(irProtocol.irCommand) {
							// Команады нажатия кнопок
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
							
							// Мигание светодиодов
							case IR_GREEN_LED:
								LED_GREEN(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_GREEN_PIN));
								break;
							case IR_BLUE_LED:
								LED_BLUE(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_BLUE_PIN));
								break;
							
							// Музыкальные команды
							case IR_SONG_PAUSE_PLAY:
								if(song.isSongPlay == 1) {
									songPause();
								} else {
									song.isSongPlay = 1;
								}
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
								song.isCyclickSong = 1;
								break;
							
							default:
								IR_curButton = 0;
						}
					
					
					}
					// Команды протокола Panasonic
					else if(irProtocol.type == IR_PROTOCOL_TYPE_PANASONIC) {
						switch(irProtocol.irCommand){
							// Команады нажатия кнопок
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
							
							// Мигание светодиодов
							case IR_PANASONIC_GREEN_LED:
								LED_GREEN(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_GREEN_PIN));
								break;
							case IR_PANASONIC_BLUE_LED:
								LED_BLUE(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_BLUE_PIN));
								break;
							
							// Музыкальные команды
							case IR_PANASONIC_SONG_PLAY:
								song.isSongPlay = 1;
								break;
							case IR_PANASONIC_SONG_PAUSE:
								songPause();
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
								song.isCyclickSong = 1;
								break;
							
							default:
								IR_curButton = 0;
						}
					
					}
					// Неизвестный протокол
					else {
						LED_BLUE(!GPIO_ReadOutputDataBit(LEDS_PORT, LED_BLUE_PIN));
					}
					// Обнуляем элементы структуры и выходим из функции
					irProtocol.type = 0;
					irProtocol.irCommand = 0;
					irProtocol.irCmdBitCounter = 0;
					irProtocol.preambuleTime = 0;
					irProtocol.isFullReceive = 0;
					irProtocol.bitsCount = 0;
					irProtocol.repeatLastCmd = 0;
					return;
					
				}
				// Установка флага в режиме теста протокола
				else {
					if(irProtocol.type == IR_PROTOCOL_TYPE_NEC) {
						// Запоминаем текущую команду NEC для дальнейшего повтора
						irProtocol.lastNecCommand = irProtocol.irCommand;
					}
				
					// Устанавливаем флаг о том, что протокол полностю принят
					irProtocol.isFullReceive = 1;
					return;
				}
			}
		}
	}
}
//
// Отправка принятой ИК-команды с помощью ШИМ и таймеров
void sendReceiveIrCommad(void) 
{
	// Установка значения периода таймера (длина импульса преамбулы)
	switch(irProtocol.type) {
		case IR_PROTOCOL_TYPE_NEC:
			TIM_SetAutoreload(TIM15, NEC_HDR_MARK);
			break;
		case IR_PROTOCOL_TYPE_PANASONIC:
			TIM_SetAutoreload(TIM15, PANASONIC_HDR_MARK);
			break;
		default: return;
	}
	// Начинаем с отправки преамбулы (импульса преамбулы)
	irTransmittConf.nextProtocolPart = IR_SEND_PREAMBULA;
	irTransmittConf.polatitySignalNow = 1;
	
	// Включение таймер и ШИМ
	//irTransmittConf.flagTransmitWork = 1;  // Нужно, если эта функция вызывается очень-очень часто
	TIM_Cmd(TIM15, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	IR_PWM_STATE(ENABLE);
	
	// Ждём, пока закончится отправка
	while(irTransmittConf.flagTransmitWork);
}
