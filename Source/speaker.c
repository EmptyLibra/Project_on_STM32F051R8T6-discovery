/** Using UTF8 file encoding
  * @file    speaker.c
  * @author  Shalaev Egor
  * @version V0.0.2
  * @date    23-June-2022
  * @brief   Эта библиотека реализует проигрывание мелодий на динамиках, с помощью таймеров в режиме ШИМ.
  * ========================
  * Можно выбирать, будет ли проигрываться музыка на маленьком динамике (без усилителя) или на большом (с усилителем).
  * Маленький динамик подключен через резистор к ноге PB11 с выходом четвертого канала TIM2.
  * Большой динамик подключен через усилитель на транзисторе к ноге PB10 с выходом третьего канала TIM2.
  * ========================
  * 1. Для задания мелодии используется два массива: с мелодиями в научной нотации и с их относительными длительностями.
  *    Полной ноте соответствует 255, половинной 128 и т.д.
  * 2. Для проигрывания мелодии используется два таймера. Первый (TIM2) генерирует ШИМ, а второй (TIM14) 
  *    управляет частотй генерации ШИМ, а также длительностью генерации ШИМ в зависимости от ноты и её длительности.
  * 3. Запуск мелодии осуществляется с помощью функции playBackgroundSong, задающей основные характеристики песни, такие как:
  *    её темп, цикличность, длительность целый ноты, количество нот в песне и процее.
  * 4. После установки основных параметров запускается второй главный таймер и начинается воспроизведение мелодии в прерывании
  *    таймера. Таким образом, ЦП МК остается свободным и не блокируется врмеенными задержками. 
  * 5. Если длительность паузы или ноты слишком большая, то таймер TIM14 переходит на счёт в мс. 
  * 6. Имеется возможность останавливать воспроизвдеение мелодии (songStop()), приостанавливать (songPause()),
  *    а также воспроизводить мелодию (song.isSongPlay = 1)
  */

#include "speaker.h"

/*============================== Структуры и переменные ==============================*/
// Структура с данными о текущей песне
Song song;

// текущий индекс меню песен (текущая выбранная песня)
static int currentItem = 0;

// Меню со всеми песнями (в строке 20 символ, но русский символ считается за 2 + '\0')
static const char SPEAKER_MENU_ITEMS[][41] = {
    "Звёздые войны - Тема",
    "Имперский марш      ",
    "Бетховен - К Элизе  ",
	"Полёт шмеля         ",
    "Пираты Карибского...",
    "Гарри Поттер        "
};

/*============================== Библиотека песен ==============================*/
/*-----Звёздные войны - Главная тема-----*/
const uint16_t StarWars_MainTheme[]       = {F4, F4, F4, A4_D, F5,    D5_D, D5, C5, A5_D,    F5, D5_D, D5, C5, A5_D,    F5, D5_D, D5, D5_D, C5,    F4, F4, F4, A4_D, F5,    D5_D, D5, C5, A5_D,    F5, D5_D, D5, C5, A5_D,    F5, D5_D, D5, D5_D, C5};
const uint8_t StarWars_MainTheme_Beats[]  = {21, 21, 21, 128, 128,    21,   21, 21, 128,     64, 21,   21, 21, 128,     64, 21,   21, 21,   128,   21, 21, 21, 128, 128,    21,   21, 21, 128,     64, 21,   21, 21, 128,     64, 21,   21, 21,   128};

/*-----Звёздные войны - Имперский марш------*/
const uint16_t StarWars_TheImperialMarch[]       = {A4, P,  A4, P,  A4, P,     F4, P, C5, P,  A4, P,     F4, P, C5, P, A4, P,     E5, P,  E5, P,  E5, P,     F5, P, C5, P, A4_B, P,    F4, P, C5, P, A4, P};
const uint8_t StarWars_TheImperialMarch_Beats[]  = {50, 20, 50, 20, 50, 20,    40, 5, 20, 5,  60, 10,    40, 5, 20, 5, 60, 80,    50, 20, 50, 20, 50, 20,    40, 5, 20, 5, 60,   10,   40, 5, 20, 5, 60, 40};

/*-----Людвиг Ван Бетховен - К Элизе-----*/
const uint16_t FurElise[]         = {E5, D5_D, E5, D5_D, E5, B4, D5, C5, A4, P,    C4, E4, A4, B4, P,    E4, G4_D, B4, C5,  P,   C4, E5, D5_D, E5, D5_D, E5, B4, D5, C5, A4, P,     C4, E4, A4, B4, P,   C4, C5, B4, A4, P};
const uint8_t FurElise_Beates[]   = {16, 16,   16, 16,   16, 16, 16, 16, 32, 16,   16, 16, 16, 32, 16,   16, 16,   16, 32, 16,   16, 16, 16,   16, 16,   16, 16, 16, 16, 32, 16,    16, 16, 16, 32, 16,  16, 16, 16, 32, 32};

/*-----Римский-Корсаков - Полёт шмеля-----*/
const uint16_t flightOfTheBumblebee[]        = {E6, D6_D, D6, С6_D,   D6, C6, C6, B5,   C6, B5, B5_B, A5,   G5_D, G5, F5_D, F5,    E5, D5_D, D5, С5_D,   D5, C5, C5, B4,   C5, B4, B4_B, A4,   G4_D, G4, F4_D, F4,         E4, D4_D, D4, С4_D,   D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,         E4, D4_D, D4, С4_D,    D4, C4, C4, B3,    C4, С4_D, D4, D4_D,    E4, F4, E4, D4,    E4, D4_D, D4, С4_D,    D4, C4, C4, B3,    C4, С4_D, D4, D4_D,    E4, F4, E4, D4};
const uint8_t flightOfTheBumblebee_Beates[]  = {16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16,   16,   16,   16, 16,   16,    16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16,   16,   16,   16, 16,   16,         16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,         16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16};

/*-----Пираты Карибского моря - главная тема-----*/
const uint16_t pirates[]        = {A3, C4,    D4, D4, D4, E4,    F4, F4, F4, G4,    E4, E4, D4, C4,    C4, D4,  P,  A3, C4,    A3, C4,    D4, D4, D4, E4,    F4, F4, F4, G4,    E4, E4, D4, C4,    D4, P, A3, C4,    D4, D4, D4, E4,    G4, G4, G4, A4,    B4, B4, A4, G4,    A4, D4, P, D4, E4,    F4, F4, G4,    A4, D4, P, D4, E4,    E4, E4, F4, D4,    E4, P};
const uint8_t pirates_Beates[]  = {32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    32, 64,  32, 32, 32,    32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64,32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    32, 64, 32,32, 32,    64, 64, 64,    32, 64, 32,32, 32,    64, 64, 32, 32,    64, 64};

/*-----Гарри Поттер - Главная тема-----*/
const uint16_t Harry_Potter[]       = {B4,   E5, G5, F5,   E5,   B5,  F5,    E5, G5, F5,  D5,  F5,   B4, P,    B4, E5, G5, F5,   E5, B5,   D6, С6_D,  C6, B5_B,   C6, B5, A5_D,   B4, F5, E5, P};
const uint8_t Harry_Potter_Beats[]  = {64,   64, 32, 64,   128,  64, 128,    64, 32, 64,  128, 64,   156,64,   64, 64, 32, 64,   128,64,   128,64,    128,64,     64, 32, 64,     128,64, 156,64};

/*-----Мызыкальное сопровождение игры Тетрис-----*/
const uint16_t TetrisGameSong[]       = {C5,    E5, B4_B, C5, D5, E5, D5, B4, A4,    A4_D, A4, C5, E5, D5, C5,    G4_D, G4, A4, B4, E5,    C5, A4, A4, P,     F4, A4, C5, B4, A4,    G4, E4, G4, F4, E4,    G4_D, G4, B4, A4, E5,     C5, B4, B4, P};
const uint8_t TetrisGameSong_Beats[]  = {64,    64, 32,   32, 32, 16, 16, 32, 32,    64,   32, 32, 64, 32, 32,    64,   32, 32, 64, 64,    64, 64, 64, 64,    64, 32, 64, 32, 32,    64, 32, 64, 32, 32,    64,   32, 32, 64, 64,     64, 64, 64, 64};

/*-----Мызыкальное сопровождение игры Змейка-----*/
const uint16_t SnakeGameSong[]       = {C4, D4,   F4, D5, P, A4_B, A4, G4,F4,     F4, D5,P, D5, C4, D4,   F4, D5, P, A4_B, A4, G4,F4,   F4, E4, D4, C4,    E4, D5, P, F4, C4, D4, F4,   F4, D5, P, D5, P};
const uint8_t SnakeGameSong_Beats[]  = {32, 64,   64, 32, 32,32,   32, 32,32,     32, 32,32,32, 32, 64,   64, 32, 32,32,   32, 32,32,   64, 64, 64, 64,    64, 32, 32,32, 32, 32, 32,   32, 32, 32,64, 32};

/*============================== Функции ==============================*/
/* Прерывание таймера, управляющего частотой и длительностью ШИМ (для динамиков) */
void TIM14_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM14, TIM_IT_Update) == SET)
	{
		// Очищаем флаг прерывания
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
		
		// Если песня сейчас играет
		if(song.isSongPlay == 1 && song.noteCount != 0)
		{
			// Если ещё не все ноты сыграны
			if(song.curNoteIndex < song.noteCount)
			{
				// Запускаем ноту и увелиичваем счетчик нот
				speakerSetNote(song.freq[song.curNoteIndex], song.beats[song.curNoteIndex]);
				
				// Включаем ШИМ и выключим только под конец песни
				TIM_CCxCmd(TIM2, (song.speakerType == SPEAKER_TYPE_MINI ? TIM_Channel_4 : TIM_Channel_3), ENABLE);
				TIM_Cmd(TIM2, ENABLE);
				TIM2->CNT = 0;
				
				song.curNoteIndex++;
				
			} else {
				// Сбрасываем счетчик нот
				song.curNoteIndex = 0;
				
				// Если песня циклична, то выжидаем паузу и начинаем заново
				if(song.isCyclickSong == 1) {
					speakerSetNote(0, LOOPS_PAUSE);
				} else {
					// Останавливаем песню и выключаем ШИМ
					song.isSongPlay = 0;
					
					TIM2->CNT = 0;
					TIM_CCxCmd(TIM2, (song.speakerType == SPEAKER_TYPE_MINI ? TIM_Channel_4 : TIM_Channel_3), DISABLE);
					TIM_Cmd(TIM2, DISABLE);
					
					// Возвращаем отсчёт по 50 мс
					TIM14_SET_ARR_50MS();
				}
			}
		} else if(TIM14->PSC != SystemCoreClock/1000 - 1) {
			// Возвращаем отсчёт по 50 мс
			TIM14_SET_ARR_50MS();
		}
	}
}

/* Инициализация малого (PB11/TIM2_CH4) и большого (PB10/TIM2_CH3) динамиков, TIM2, TIM14 */
void speakerInit(void)
{
	/*======================Настройка пинов динамиков (PB11, PB10)========================*/
	// Тактирование порта динамиков
    RCC_AHBPeriphClockCmd(RCC_AHBENR_SPEAKEREN, ENABLE);
	
	// Настройки пинов порта
    GPIO_InitTypeDef GPIOx_ini;
    GPIOx_ini.GPIO_Pin = SPEAKER_MINI_PIN | SPEAKER_BIG_PIN; // Выбираем маленький и большой динамик
    GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;                      // Режим работы порта: альтернативная функция
    GPIOx_ini.GPIO_Speed = GPIO_Speed_10MHz;                 // Скорость (частота) работы порта: 10 Мгц
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;                    // Режим работы выхода порта: двухтактный (push-pull)
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;                  // Подтяжка к питанию или к земле: подтяжка выключена
    GPIO_Init(SPEAKER_PORT, &GPIOx_ini);                     // Инициализация порта
	
	// Включаем в качестве альтернативной функции TIM2
	GPIO_PinAFConfig(SPEAKER_PORT, SPEAKER_MINI_SOURCE, GPIO_AF_2);
	GPIO_PinAFConfig(SPEAKER_PORT, SPEAKER_BIG_SOURCE, GPIO_AF_2);
	
	
	/*===========Настройка таймера TIM2 для генерации ШИМ (TIM2_CH3, TIM2_CH4)==============*/
	// Включаем тактирование таймера TIM2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
	// Настройка таймера на счет в мкс
	TIM_TimeBaseInitTypeDef TIMx_ini;
    TIMx_ini.TIM_Prescaler = SystemCoreClock/1000000 - 1; // Пределитель таймера (48-1): считает 1 мкс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;        // Направление счета: вверх
    TIMx_ini.TIM_Period = 0;                              // Период счета: будет меняться динамически
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;            // Делаитель частоты: 1
    TIM_TimeBaseInit(TIM2, &TIMx_ini);                    // Инициализация таймера
	
	// Настраиваем ШИМ для передатика
	TIM_OCInitTypeDef TIMx_PWM;
	TIM_OCStructInit(&TIMx_PWM);
	
	// Настраиваем канал таймера на ШИМ со скважностью 50% (период будет задаваться динамически)
	TIMx_PWM.TIM_OCMode = TIM_OCMode_PWM1;                 // Режим канала: ШИМ1
	TIMx_PWM.TIM_OutputState = TIM_OutputState_Disable;    // Состояние выхода: шим выключен
	TIMx_PWM.TIM_OutputNState = TIM_OutputNState_Disable;  // Выход с обратной полярностью: выключен
	TIMx_PWM.TIM_OCPolarity = TIM_OCPolarity_High;         // Полярность сигнала: импульс - это 5В
	TIMx_PWM.TIM_Pulse = 0;                                // Длительность импульса: задается динамически
	
	// Инициализация 3-го и 4-го каналов таймера
	TIM_OC3Init(TIM2, &TIMx_PWM);
	TIM_OC4Init(TIM2, &TIMx_PWM);
	
	// Выключаем таймер, отвечающий за генерацию ШИМ
	TIM_Cmd(TIM2, DISABLE);
	
	/*===========Настройка таймера TIM14 для управления ШИМ ==============*/
	// Включаем тактирование таймера TIM14
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
    
	// Настройка TIM14 на счёт в 50 мс
    TIMx_ini.TIM_Prescaler = SystemCoreClock/1000 - 1;     // Пределитель таймера (48-1): считает 1 мс
    TIMx_ini.TIM_CounterMode = TIM_CounterMode_Up;         // Направление счета: вверх
    TIMx_ini.TIM_Period = 50;                              // Период счета: меняется динамически (по умолчанию 50 мс)
    TIMx_ini.TIM_ClockDivision = TIM_CKD_DIV1;             // Делитель частоты: 1
    TIM_TimeBaseInit(TIM14, &TIMx_ini);                    // Инициализация таймера
    
	// Включение прерываний
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);  // Прерывание на достижение периода
    NVIC_EnableIRQ(TIM14_IRQn);                  // Включаем прерывания TIM14
	TIM_Cmd(TIM14, ENABLE);                      // Включаем таймер
}

/* Установка той или иной ноты (или паузы) в ШИМ 
 * freq: частота устанавливаемой ноты
 * duration: длительность ноты */
void speakerSetNote(uint16_t freq, uint16_t relatDuration)
{
	// Расчёт текущей длительности ноты
	uint64_t curNoteDuration = (freq == 0 ? relatDuration : 1000*((uint32_t)((song.wholeNoteLen*relatDuration)/255)) );
	
	// Обнуляем счётчик таймера
	TIM14->CNT = 0;
	
	// Устанавливаем время, в течении которого будет играть нота
	// Если оно слишком большое, то переходим на счёт в мс, иначе счёт в мкс
	TIM14->PSC = (curNoteDuration >= 0xFFF ? SystemCoreClock/1000 - 1 : SystemCoreClock/1000000 - 1);
	TIM14->ARR = (curNoteDuration >= 0xFFF ? curNoteDuration/1000     : curNoteDuration);
	
	// Установка ноты или паузы
	if(freq != P && freq != 0) 
	{
		// Период ШИМ ноты
		TIM2->ARR = 1000000/freq;
		
		// Устанавливаем скважность равной 50% и включение ШИМ
		if(song.speakerType == SPEAKER_TYPE_MINI) {
			TIM2->CCR4 = TIM2->ARR/2;
		} else {
			TIM2->CCR3 = TIM2->ARR/2;
		}
		
	} else {
		// Выключаем ШИМ и ждём паузу
		if(song.speakerType == SPEAKER_TYPE_MINI) {
			TIM2->CCR4 = 0;
		} else {
			TIM2->CCR3 = 0;
		}
	}
}

/* Устанавливаем настройки песни и заупскаем её 
 * speakerType:  тип динамика, который будет играть мелодию (SPEAKER_TYPE_MINI=0 или SPEAKER_TYPE_BIG=1)
 * curSong:      указатель на текущий массив с нотами [!NULL]
 * songBeats:    указатель на текущий массив с относительными длительностями нот [!NULL]
 * curNoteCount: кол-во нот в песне [1, ...]
 * curTempo:     темп песни [1, 255]
 * isCyclick:    песня повторяется бесконечно (1) или проигрывается один раз (0)  */
void playBackgroundSong(uint8_t speakerType, const uint16_t *curSong, const uint8_t* songBeats, uint16_t curNoteCount, uint8_t curTempo, uint8_t isCyclick)
{
	// Задаем настройки песни
	song.speakerType = speakerType;         // Тип динамика
	song.isCyclickSong = isCyclick;         // Циклична песня или нет
    song.tempo = curTempo;                  // Текущий темп песни
    song.wholeNoteLen = 240000/song.tempo;  // Длина одной полной ноты в мкс
    song.noteCount = curNoteCount;          // Общее количество нот
    song.freq = curSong;                    // Массив с нотами песни
    song.beats = songBeats;                 // Массив с относительными временными длительностями нот
	song.curNoteIndex = 0;                  // Текущий индекс ноты
	
	// Устанавливаем счёт таймера на 50 мс
	TIM14_SET_ARR_50MS();
	
	song.isSongPlay = 1;                    // Запуска песни
}

/* Полная остановка песни без возможности возобновить */
void songStop(void)
{
	// Отключаем ШИМ и таймер, отвечающий за него
	TIM2->CNT = 0;
	TIM_CCxCmd(TIM2, (song.speakerType == SPEAKER_TYPE_MINI ? TIM_Channel_4 : TIM_Channel_3), DISABLE);
	TIM_Cmd(TIM2, DISABLE);
	
	// Сбрасываем текущую песню
	song.isSongPlay = 0;
	song.noteCount = 0;
	
	// Возвращаем отсчёт по 50 мс
	TIM14_SET_ARR_50MS();
	TIM2->CNT = 0;
}

/* Приостановка песни с возвожностью возобновления (для этого надо лишь записать song.isSongPlay единицу ) */
void songPause(void)
{
	// Приостанавливаем музыку
	song.isSongPlay = 0;
	
	// Выключаем Шим, и возобновляем счёт основного таймера по 50 мс
	TIM2->CNT = 0;
	TIM_CCxCmd(TIM2, (song.speakerType == SPEAKER_TYPE_MINI ? TIM_Channel_4 : TIM_Channel_3), DISABLE);
	TIM_Cmd(TIM2, DISABLE);
	TIM14_SET_ARR_50MS();
}
//
/* Рисуем меню выбора динамика
 * choose: положение курсора (0 - юольшой динамик, 1 - маленький)*/
static void drawChooseMenu(uint8_t choose)
{
	// Начальное положение курсора
	LCD_SetBufferCursor(0, PAGE_3);
	
	// Рисуем одну страницу с учётом выбора
    lcdStruct.writeStringToBuffer((choose == 0? ">маленький  большой" : " маленький >большой"));
    LCD_DrawPageFromBuffer(PAGE_3);
}

/* Меню выбора динамика, на котором будет воспроизводиться песня */
static void chooseSpeakerType(void)
{
	// Начальное положение курсора
	LCD_SetBufferCursor(18, PAGE_1);
    lcdStruct.writeStringToBuffer("Выберите динамик");
    LCD_SetBufferCursor(0, PAGE_3);
    lcdStruct.writeStringToBuffer(">маленький  большой");
    lcdStruct.displayFullUpdate();
    
    uint8_t curChoose = 0;
	
	// Цикл с выбором элемента
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
		// Движение влево
        if(isButtonPressed(BUTTON_LEFT)) {
            delayUs(10000);
			
			// Изменение положение курсора и его отрисовка
            curChoose -= (curChoose == 0 ? 0 : 1);
            drawChooseMenu(curChoose);
        }
		
		// Движение вправо
        if(isButtonPressed(BUTTON_RIGHT)) {
            delayUs(10000);
            curChoose += (curChoose == 1 ? 0 : 1);
            drawChooseMenu(curChoose);
        }
		
		// Выбор пункта меню
        if(isButtonPressed(BUTTON_SELECT)) {
            delayUs(10000);
            lcdStruct.clearOrFillDisplay(CLEAR);
            song.speakerType = curChoose;
			break;
        }
    }
}

/* Рисует очередное меню с песнями */
static void drawSpeakerMenuBuffer(void)
{
	// Начальное положение курсора
	LCD_SetBufferCursor(0, 0);
	
	// Рисуем все пункты меню
    for(uint8_t i = 0; i < SPEAKER_MENU_COUNT; i++){
        lcdStruct.writeStringToBuffer((i == currentItem) ? ">" : " ");
        lcdStruct.writeStringToBuffer(SPEAKER_MENU_ITEMS[i]);
    }
	lcdStruct.displayFullUpdate();
}
/* Главное меню динамиков с выбором и проигрыванием песен */
void speakerMenu(void)
{
	// Очистка буфера дисплея
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
	
	// Меню выбора типа динамика
    chooseSpeakerType();
	
	// Отрисовка основного меню с песнями
    drawSpeakerMenuBuffer();
    
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
		// Движение по меню вверх
        if(isButtonPressed(BUTTON_TOP)){
            delayUs(buttonDelayUs);
            currentItem -= (currentItem == 0 ? 0 : 1);
            
            drawSpeakerMenuBuffer();
        }
        
		// Движение по меню вниз
        if(isButtonPressed(BUTTON_BOTOOM)){
            delayUs(buttonDelayUs);
            currentItem += (currentItem >= SPEAKER_MENU_COUNT-1 ? 0 : 1);
            
            drawSpeakerMenuBuffer();
        }
        
		// Выбор и запук песни для проигрывания
        if(isButtonPressed(BUTTON_SELECT))
		{
            delayUs(500000);
			
			// Выбор песни в зависисмости от положения курсора
            switch(currentItem) 
			{
				// Звёздные войны - Главная тема
                case 0:
                    playBackgroundSong(song.speakerType, StarWars_MainTheme, StarWars_MainTheme_Beats, sizeof(StarWars_MainTheme)/2, 108,0);
                    break;
				
				// Звёздные войны - Имперский марш
                case 1:
                    playBackgroundSong(song.speakerType, StarWars_TheImperialMarch, StarWars_TheImperialMarch_Beats, sizeof(StarWars_TheImperialMarch)/2, 108,0);
                    break;
				
				// Людвиг Ван Бетховен - К Элизе
                case 2:
                    playBackgroundSong(song.speakerType, FurElise, FurElise_Beates, sizeof(FurElise)/2, 90,0);
                    break;
				
				// Римский-Корсаков - Полёт шмеля
                case 3:
                    playBackgroundSong(song.speakerType, flightOfTheBumblebee, flightOfTheBumblebee_Beates, sizeof(flightOfTheBumblebee)/2, 190,0);
                    break;
				
				// Пираты Карибского моря - главная тема
                case 4:
                    playBackgroundSong(song.speakerType, pirates, pirates_Beates, sizeof(pirates)/2, 140,0);
                    break;
				
				// Гарри Поттер - Главная тема
                case 5:
                    playBackgroundSong(song.speakerType, Harry_Potter, Harry_Potter_Beats, sizeof(Harry_Potter)/2, 100,0);
                    break;
                default: ;
            }
        }
        
		// Выход из меню
		if(isButtonPressed(BUTTON_BACK))
		{
			// Остановка песни
			songStop();
			return;
		}
    }
}
