/**
  * @file    lcd_lib.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    04-June-2022
  * @brief   Библиотека для работы с дисплеем LCD12864B V 2.0 на контроллере ST7920.
  * ==================================
  * Позволяет работать с дисплеем по любому из трех поддерживаемых интерфейсов:
  * парарлельный 8-ми битны, параллельный 4-х битный и SPI-интерфейс.
  * Дисплей поддерживает как символьный, так и графический режим. По умолчанию используется графический.
  */
#include "lcd_lib.h"

/*============================== Стуктуры и переменные ==============================*/
/* Структура с основными переменными и функциями для работы с дисплеем. */
LCD_STRUCT lcdStruct = { 
	.byteIndex           = 0x00, 
	.portsInit           = LCD_PortsInit, 
	.init                = LCD_Init, 
	.writeStringToBuffer = LCD_WriteStringToBuffer,
	.displayFullUpdate   = LCD_DisplayFullUpdate, 
	.sendByteData        = LCD_SendByteData, 
    .clearOrFillDisplay  = LCD_ClearOrFillDisplay 
};
uint8_t displayBuffer[LCD_BUFFER_LENGTH] = {0}; /* Буфер байтов всего дисплея */

/* Библиотека с символами в UTF8 Basic Latin коде (шрифт 5*7) + кириллица
   (для латиницы, начиная с проблеа) и отдельно кирилица */
static const uint8_t LIBRARY_SYMBOL[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0/ --> space     0x20
		0x00, 0x00, 0x4F, 0x00, 0x00, 0x00, //1/ --> !         0x21
		0x00, 0x07, 0x00, 0x07, 0x00, 0x00, //2/ --> "         0x22 и т.д.
		0x00, 0x14, 0x7F, 0x14, 0x7F, 0x14, //3/ --> #
		0x00, 0x24, 0x2A, 0x7F, 0x2A, 0x12, //4/ --> $
		0x00, 0x23, 0x13, 0x08, 0x64, 0x62, //5/ --> %
		0x00, 0x36, 0x49, 0x55, 0x22, 0x40, //6/ --> &
		0x00, 0x00, 0x05, 0x03, 0x00, 0x00, //7/ --> '
		0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, //8/ --> (
		0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, //9/ --> )
		0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, //10/ --> *
		0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, //11/ --> +
		0x00, 0xA0, 0x60, 0x00, 0x00, 0x00, //12/ --> ,
		0x00, 0x08, 0x08, 0x08, 0x08, 0x08, //13/ --> -
		0x00, 0x60, 0x60, 0x00, 0x00, 0x00, //14/ --> .
		0x00, 0x20, 0x10, 0x08, 0x04, 0x02, //15/ --> /
		0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, //16/ --> 0
		0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, //17/ --> 1
		0x00, 0x42, 0x61, 0x51, 0x49, 0x46, //18/ --> 2
		0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, //19/ --> 3
		0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, //20/ --> 4
		0x00, 0x27, 0x45, 0x45, 0x45, 0x39, //21/ --> 5
		0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, //22/ --> 6
		0x00, 0x01, 0x71, 0x09, 0x05, 0x03, //23/ --> 7
		0x00, 0x36, 0x49, 0x49, 0x49, 0x36, //24/ --> 8
		0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, //25/ --> 9
		0x00, 0x6C, 0x6C, 0x00, 0x00, 0x00, //26/ --> :
		0x00, 0x00, 0x56, 0x36, 0x00, 0x00, //27/ --> ;
		0x00, 0x08, 0x14, 0x22, 0x41, 0x00, //28/ --> <
		0x00, 0x24, 0x24, 0x24, 0x24, 0x24, //29/ --> =
		0x00, 0x00, 0x41, 0x22, 0x14, 0x08, //30/ --> >
		0x00, 0x02, 0x01, 0x51, 0x09, 0x06, //31/ --> ?
		0x00, 0x32, 0x49, 0x79, 0x41, 0x3E, //32/ --> @
		0x00, 0x7E, 0x11, 0x11, 0x11, 0x7E, //33/ --> A
		0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, //34/ --> B
		0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, //35/ --> C
		0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, //36/ --> D
		0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, //37/ --> E
		0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, //38/ --> F
		0x00, 0x3E, 0x41, 0x49, 0x49, 0x3A, //39/ --> G
		0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, //40/ --> H
		0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, //41/ --> I
		0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, //42/ --> J
		0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, //43/ --> K
		0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, //44/ --> L
		0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, //45/ --> M
		0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, //46/ --> N
		0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, //47/ --> O
		0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, //48/ --> P
		0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, //49/ --> Q
		0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, //50/ --> R
		0x00, 0x46, 0x49, 0x49, 0x49, 0x31, //51/ --> S
		0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, //52/ --> T
		0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, //53/ --> U
		0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, //54/ --> V
		0x00, 0x3F, 0x40, 0x60, 0x40, 0x3F, //55/ --> W
		0x00, 0x63, 0x14, 0x08, 0x14, 0x63, //56/ --> X
		0x00, 0x07, 0x08, 0x70, 0x08, 0x07, //57/ --> Y
		0x00, 0x61, 0x51, 0x49, 0x45, 0x43, //58/ --> Z
		0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, //59/ --> [
		0x00, 0x15, 0x16, 0x7C, 0x16, 0x15, //60/ --> '\'
		0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, //61/ --> ]
		0x00, 0x04, 0x02, 0x01, 0x02, 0x04, //62/ --> ^
		0x00, 0x40, 0x40, 0x40, 0x40, 0x40, //63/ --> _
		0x00, 0x01, 0x02, 0x04, 0x00, 0x00, //64/ --> `
		0x00, 0x20, 0x54, 0x54, 0x54, 0x78, //65/ --> a
		0x00, 0x7F, 0x44, 0x44, 0x44, 0x38, //66/ --> b
		0x00, 0x38, 0x44, 0x44, 0x44, 0x00, //67/ --> c
		0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, //68/ --> d
		0x00, 0x38, 0x54, 0x54, 0x54, 0x18, //69/ --> e
		0x00, 0x10, 0x7E, 0x11, 0x01, 0x02, //70/ --> f
		0x00, 0x0C, 0x52, 0x52, 0x52, 0x3E, //71/ --> g
		0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, //72/ --> h
		0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, //73/ --> i
		0x00, 0x20, 0x40, 0x40, 0x3D, 0x00, //74/ --> j
		0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, //75/ --> k
		0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, //76/ --> l
		0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, //77/ --> m
		0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, //78/ --> n
		0x00, 0x38, 0x44, 0x44, 0x44, 0x38, //79/ --> o
		0x00, 0x7C, 0x14, 0x14, 0x14, 0x08, //80/ --> p
		0x00, 0x08, 0x14, 0x14, 0x18, 0x7C, //81/ --> q
		0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, //82/ --> r
		0x00, 0x48, 0x54, 0x54, 0x54, 0x20, //83/ --> s
		0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, //84/ --> t
		0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, //85/ --> u
		0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, //86/ --> v
		0x00, 0x1E, 0x20, 0x10, 0x20, 0x1E, //87/ --> w
		0x00, 0x22, 0x14, 0x08, 0x14, 0x22, //88/ --> x
		0x00, 0x06, 0x48, 0x48, 0x48, 0x3E, //89/ --> y
		0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, //90/ --> z
		0x00, 0x08, 0x36, 0x41, 0x00, 0x00, //91/ --> {
		0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, //92/ --> |
		0x00, 0x41, 0x36, 0x08, 0x00, 0x00, //93/ --> }
		0x00, 0x08, 0x08, 0x2A, 0x1C, 0x08, //94/ --> ~
		0x00, 0x7E, 0x11, 0x11, 0x11, 0x7E, //95/ --> А
		0x00, 0x7F, 0x49, 0x49, 0x49, 0x33, //96/ --> Б
		0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, //97/ --> В
		0x00, 0x7F, 0x01, 0x01, 0x01, 0x03, //98/ --> Г
		0x00, 0xE0, 0x51, 0x4F, 0x41, 0xFF, //99/ --> Д
		0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, //100/ --> Е
		0x00, 0x77, 0x08, 0x7F, 0x08, 0x77, //101/ --> Ж
		0x00, 0x41, 0x49, 0x49, 0x49, 0x36, //102/ --> З
		0x00, 0x7F, 0x10, 0x08, 0x04, 0x7F, //103/ --> И
		0x00, 0x7C, 0x21, 0x12, 0x09, 0x7C, //104/ --> Й
		0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, //105/ --> К
		0x00, 0x20, 0x41, 0x3F, 0x01, 0x7F, //106/ --> Л
		0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, //107/ --> М
		0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, //108/ --> Н
		0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, //109/ --> О
		0x00, 0x7F, 0x01, 0x01, 0x01, 0x7F, //110/ --> П
		0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, //111/ --> Р
		0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, //112/ --> С
		0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, //113/ --> Т
		0x00, 0x47, 0x28, 0x10, 0x08, 0x07, //114/ --> У
		0x00, 0x1C, 0x22, 0x7F, 0x22, 0x1C, //115/ --> Ф
		0x00, 0x63, 0x14, 0x08, 0x14, 0x63, //116/ --> Х
		0x00, 0x7F, 0x40, 0x40, 0x40, 0xFF, //117/ --> Ц
		0x00, 0x07, 0x08, 0x08, 0x08, 0x7F, //118/ --> Ч
		0x00, 0x7F, 0x40, 0x7F, 0x40, 0x7F, //119/ --> Ш
		0x00, 0x7F, 0x40, 0x7F, 0x40, 0xFF, //120/ --> Щ
		0x00, 0x01, 0x7F, 0x48, 0x48, 0x30, //121/ --> Ъ
		0x00, 0x7F, 0x48, 0x30, 0x00, 0x7F, //122/ --> Ы
		0x00, 0x00, 0x7F, 0x48, 0x48, 0x30, //123/ --> Э
		0x00, 0x22, 0x41, 0x49, 0x49, 0x3E, //124/ --> Ь
		0x00, 0x7F, 0x08, 0x3E, 0x41, 0x3E, //125/ --> Ю
		0x00, 0x46, 0x29, 0x19, 0x09, 0x7f, //126/ --> Я
		0x00, 0x20, 0x54, 0x54, 0x54, 0x78, //127/ --> a
		0x00, 0x3c, 0x4a, 0x4a, 0x49, 0x31, //128/ --> б
		0x00, 0x7c, 0x54, 0x54, 0x28, 0x00, //129/ --> в
		0x00, 0x7c, 0x04, 0x04, 0x04, 0x0c, //130/ --> г
		0x00, 0xe0, 0x54, 0x4c, 0x44, 0xfc, //131/ --> д
		0x00, 0x38, 0x54, 0x54, 0x54, 0x18, //132/ --> е
		0x00, 0x6c, 0x10, 0x7c, 0x10, 0x6c, //133/ --> ж
		0x00, 0x44, 0x44, 0x54, 0x54, 0x28, //134/ --> з
		0x00, 0x7c, 0x20, 0x10, 0x08, 0x7c, //135/ --> и
		0x00, 0x7c, 0x41, 0x22, 0x11, 0x7c, //136/ --> й
		0x00, 0x7c, 0x10, 0x28, 0x44, 0x00, //137/ --> к
		0x00, 0x20, 0x44, 0x3c, 0x04, 0x7c, //138/ --> л
		0x00, 0x7c, 0x08, 0x10, 0x08, 0x7c, //139/ --> м
		0x00, 0x7c, 0x10, 0x10, 0x10, 0x7c, //140/ --> н
		0x00, 0x38, 0x44, 0x44, 0x44, 0x38, //141/ --> о
		0x00, 0x7c, 0x04, 0x04, 0x04, 0x7c, //142/ --> п
		0x00, 0x7C, 0x14, 0x14, 0x14, 0x08, //143/ --> р
		0x00, 0x38, 0x44, 0x44, 0x44, 0x28, //144/ --> с
		0x00, 0x04, 0x04, 0x7c, 0x04, 0x04, //145/ --> т
		0x00, 0x0C, 0x50, 0x50, 0x50, 0x3C, //146/ --> у
		0x00, 0x30, 0x48, 0xfc, 0x48, 0x30, //147/ --> ф
		0x00, 0x44, 0x28, 0x10, 0x28, 0x44, //148/ --> х
		0x00, 0x7c, 0x40, 0x40, 0x40, 0xfc, //149/ --> ц
		0x00, 0x0c, 0x10, 0x10, 0x10, 0x7c, //150/ --> ч
		0x00, 0x7c, 0x40, 0x7c, 0x40, 0x7c, //151/ --> ш
		0x00, 0x7c, 0x40, 0x7c, 0x40, 0xfc, //152/ --> щ
		0x00, 0x04, 0x7c, 0x50, 0x50, 0x20, //153/ --> ъ
		0x00, 0x7c, 0x50, 0x50, 0x20, 0x7c, //154/ --> ы
		0x00, 0x7c, 0x50, 0x50, 0x20, 0x00, //155/ --> э
		0x00, 0x28, 0x44, 0x54, 0x54, 0x38, //156/ --> ь
		0x00, 0x7c, 0x10, 0x38, 0x44, 0x38, //157/ --> ю
		0x00, 0x08, 0x54, 0x34, 0x14, 0x7c, //158/ --> я
		0x00, 0x7E, 0x4B, 0x4A, 0x4B, 0x42, //159/ --> Ё
		0x00, 0x38, 0x55, 0x54, 0x55, 0x18, //160/ --> ё
		0x00, 0x00, 0x06, 0x09, 0x09, 0x06, //161/ --> °
};

//
/*============================== Функции ==============================*/
//
/*-----Функции отправки команд или данных -----------------------------------*/
/* Чтение Флага занятости и счетчика адреса памяти (!!только в параллельном интерфейсе!!)
 * Возвращаемое значение: байт вида |BF|AC6|AC5|AC4 | AC3|AC2|AC1|AC0|
 * Старший бит - значение флага занятости дисплея, остальные биты - значение счетчика адреса*/
#ifdef INTERFACE_8BIT_PARALLEL
uint8_t LCD_ReadBsyFlagAndAdressCounter(void)
{
	/* Для 4-х битного интерфейса требуются изменения */
	uint8_t byte = 0;
	
	/* Отпускаем пины (назначаем как вход), чтобы можно было считать значение с них */
	GPIOC->MODER &= 0xFFFFFFFFFC0FFFFFU;
	GPIOD->MODER &= 0xFFFFFFFFFFFFFFCFU;
	GPIOB->MODER &= 0xFFFFFFFFFFFFC03FU;
	
	/* Комбинация для чтения флага занятости и счетчика адреса */
	RS(0);
    RW(1);
    
	/* Читаем данные с пинов */
    E(1);
    byte |= (GPIO_ReadInputDataBit(GPIOB, DB7_PIN) << 7);
	byte |= (GPIO_ReadInputDataBit(GPIOB, DB6_PIN) << 6);
	byte |= (GPIO_ReadInputDataBit(GPIOB, DB5_PIN) << 5);
	byte |= (GPIO_ReadInputDataBit(GPIOB, DB4_PIN) << 4);
	byte |= (GPIO_ReadInputDataBit(GPIOD, DB3_PIN) << 3);
	byte |= (GPIO_ReadInputDataBit(GPIOC, DB2_PIN) << 2);
	byte |= (GPIO_ReadInputDataBit(GPIOC, DB1_PIN) << 1);
	byte |= (GPIO_ReadInputDataBit(GPIOC, DB0_PIN));
    E(0);
		
	/* Возвращаем конфигурацию пинов на выход */
	GPIOC->MODER |= 0x1500000U;
	GPIOD->MODER |= 0x10U;
	GPIOB->MODER |= 0x1540U;
	
	return byte;
}
#endif
/* Отправка байта данных или команды
* dataOrCmd - выбор команды (LCD_COMMAND = 0) или данных (LCD_DATA = 1) */
void LCD_SendByteData(uint8_t dataOrCmd, uint8_t byte)
{
	#ifdef INTERFACE_SPI
	/*-------- Интейрфейс SPI -------*/
	
		CS(1); /* Начало передачи */
		
		/* Отправка синхронизирующего байта и битов RW, RS */
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_SendData8(SPI2, 0xF8U | (dataOrCmd << 1));
		
		/* Отправка перовй половины байта */
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_SendData8(SPI2, (uint8_t)(byte & 0xF0));
		
		/* Отправка второй половины байта */
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_SendData8(SPI2, (uint8_t)(byte << 4));
		
		/* Ожидаем окончание передачи */
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET){}
		CS(0); /* конец передачи */
		
		/* Ожидаем, когда выполнится команда или отправятся данные - 72 мкС */
		delayUs(72);
	
	#elif defined INTERFACE_8BIT_PARALLEL
	/*-------- Интейрфейс параллельный 8-ми битный -------*/
		RS(dataOrCmd); /* Запись инструкции: RS = 1, Запись данных: RS = 0 */
		RW(0);         /* Записываем данные: RW = 0 */
		
		E(1);                     /* Поднимаем такотовый сигнал */
		DB7((byte >> 7) & 0x01);  /* Отправка битов данных или команды */
		DB6((byte >> 6) & 0x01);  /* Страший бит в DB7, младший в DB0 */
		DB5((byte >> 5) & 0x01);
		DB4((byte >> 4) & 0x01);
		DB3((byte >> 3) & 0x01);
		DB2((byte >> 2) & 0x01);
		DB1((byte >> 1) & 0x01);
		DB0(byte & 0x01);
		E(0);                     /* Опускаем такотовый сигнал */
		
		/* Ожидаем, когда выполнится команда или отправятся данные - 72 мкС */
		delayUs(72);
		
	#elif defined INTERFACE_4BIT_PARALLEL
	/*-------- Интейрфейс параллельный 4-х битный -------*/
		/*-------- Первый полубайт ---------*/
		RS(dataOrCmd); /* Запись инструкции: RS = 1, Запись данных: RS = 0 */
		RW(0);         /* Записываем данные: RW = 0 */
		
		E(1);                     /* Поднимаем такотовый сигнал */
		DB7((byte >> 7) & 0x01);  /* Отправка битов данных или команды */
		DB6((byte >> 6) & 0x01);  /* Страший бит в DB7, младший в DB0 */
		DB5((byte >> 5) & 0x01);
		DB4((byte >> 4) & 0x01);
		E(0);                     /* Опускаем такотовый сигнал */
		
		/*-------- Второй полубайт ---------*/
		E(1);                     /* Поднимаем такотовый сигнал */
		DB7((byte >> 3) & 0x01);
		DB6((byte >> 2) & 0x01);
		DB5((byte >> 1) & 0x01);
		DB4(byte & 0x01);
		E(0);                     /* Опускаем такотовый сигнал */
		
		/* Ожидаем, когда выполнится команда или отправятся данные - 72 мкС */
		delayUs(72);
	#endif
}

/* Устанавливает адрес графического дисплея */
void LCD_SetAddr(uint8_t vertAddr, uint8_t horizAddr)
{
    /* Регистр счетчика адреса (AC) автоматически увеличивается только после установления координаты y 
	* Максимум по y: 0x3F (63)
	* Максимум по x: 0x0F (15) */ 
    
	/* Переходим в расширенный режим */
	#if defined INTERFACE_8BIT_PARALLEL || defined INTERFACE_SPI
		LCD_SendByteData(LCD_COMMAND, EX_GRAPHIC_DISPLAY_ON);
	#elif defined INTERFACE_4BIT_PARALLEL
		LCD_SendByteData(LCD_COMMAND, 0x26);
	#endif
	
    /* Установка адреса y [0, 63] */
    LCD_SendByteData(LCD_COMMAND, (EX_SET_GDRAM_ADDR | (vertAddr & 0x3F)));
  
	/* Установка адреса x [0, 15] */
    LCD_SendByteData(LCD_COMMAND, (EX_SET_GDRAM_ADDR | (horizAddr & 0x0F)));
	
	/* Возвращаемся в базовый режим */
	#if defined INTERFACE_8BIT_PARALLEL || defined INTERFACE_SPI
		LCD_SendByteData(LCD_COMMAND, EX_GR_GO_TO_BASIC_FUNC_SET);
	#elif defined INTERFACE_4BIT_PARALLEL
		LCD_SendByteData(LCD_COMMAND, 0x22);
	#endif
}

//
/*------------- Функции инициализации -----------------*/
/* Инициализация портов (пинов) дисплея в зависимости от выбранного интерфейса 
 * Пины SPI: PB12-PB15 (SPI2)
 * Пины 8-ми битного: PB0-PB7, PB12-PB15
 * Пины 4-x битного:  PB4-PB7, PB12-PB15 */
void LCD_PortsInit(void)
{
	#if defined INTERFACE_SPI
	/* Для последовательного SPI интерфейса */
		/* Включаем тактирование портов */
		RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
		
		/* Структура для инициализации порта */ 
		GPIO_InitTypeDef GPIOx_ini;
		
		/* Инициализация порта B */
		GPIOx_ini.GPIO_Pin = LCD_SCK_PIN | LCD_MOSI_PIN;
		GPIOx_ini.GPIO_Mode = GPIO_Mode_AF;       /* Режим работы порта: альтернативная функция */
		GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;  /* Скорость (частота) работы порта: 50 Мгц */
		GPIOx_ini.GPIO_OType = GPIO_OType_PP;     /* Режим работы выхода порта: тяни-талкай (push-pull) */
		GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;   /* Подтяжка к питанию или к земле: подтяжка выключена */
		GPIO_Init(GPIOB, &GPIOx_ini);
		
		/* Пины RST и CS контролируются программно */
		GPIOx_ini.GPIO_Pin = LCD_RST_PIN | LCD_NSS_PIN;
		GPIOx_ini.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOB, &GPIOx_ini);
		
		/* Включаем в качестве альтернативной функции SPI2 */
		GPIO_PinAFConfig(GPIOB, LCD_SCK_SOURCE, GPIO_AF_0);
		GPIO_PinAFConfig(GPIOB, LCD_MOSI_SOURCE, GPIO_AF_0);
		
		/* Тактирование SPI2 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		delayUs(1);
		
		SPI_InitTypeDef spiStruct;
		spiStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  /* Направление передачи: в обе сторны, полныйдуплекс */
		spiStruct.SPI_Mode = SPI_Mode_Master;                       /* Режим работы SPI: мастер */
		spiStruct.SPI_DataSize = SPI_DataSize_8b;                   /* Размер отправляемых данных: 8 бит */
		spiStruct.SPI_CPOL = SPI_CPOL_High;                         /* Исходное состояние сигнала SCK: высокий уровень (CPOL = 1) */
		spiStruct.SPI_CPHA = SPI_CPHA_2Edge;                        /* Выставляем данные по заднему фронту (спаду) (CPHA = 1) */
		spiStruct.SPI_NSS = SPI_NSS_Soft;                           /* Управление сигналом CS программно (самостоятельно) */
		spiStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  /* Делитель частоты: 48/2 = 24 МГц */
		spiStruct.SPI_FirstBit = SPI_FirstBit_MSB;                  /* Порядок передачи битов: сначала старший */
		spiStruct.SPI_CRCPolynomial = 7;                            /* Нужно для рачета контрольной суммы (не используется) */
		
		SPI_Init(SPI2, &spiStruct);
		SPI_Cmd(SPI2, ENABLE);       /* Включаем SPI */
	
	#elif defined INTERFACE_8BIT_PARALLEL || defined INTERFACE_4BIT_PARALLEL
	/* Для параллельного 8-битного или 4-х битного интерфейса*/
		/* Включаем тактирование портов */
		RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
		
		/* Структура для инициализации порта */ 
		GPIO_InitTypeDef GPIOx_ini;
		
		/* Общие настройки всех пинов */
		GPIOx_ini.GPIO_Pin = (RW_PIN | E_PIN | RS_PIN | RST_PIN | DB4_PIN | DB5_PIN | DB6_PIN | DB7_PIN);
		GPIOx_ini.GPIO_Mode = GPIO_Mode_OUT;      /* Режим работы порта: как выход */
		GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;  /* Скорость (частота) работы порта: 50 Мгц */
		GPIOx_ini.GPIO_OType = GPIO_OType_PP;     /* Режим работы выхода порта: тяни-талкай (push-pull) */
		GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;   /* Подтяжка к питанию или к земле: подтяжка выключена */
	
		GPIO_Init(GPIOB, &GPIOx_ini);
		
		GPIOx_ini.GPIO_Pin = (DB0_PIN | DB1_PIN | DB2_PIN | DB3_PIN);
		GPIO_Init(GPIOB, &GPIOx_ini);
	#endif
}

/* Включение и инициализация дисплея */
void LCD_Init(void)
{
	#if defined INTERFACE_8BIT_PARALLEL || defined INTERFACE_SPI
	/* Графический режим */
		/* Ждём более 40 мс после включения питания */
		delayUs(40000);
		
		/* Сбрасываем дисплей */
		RST(0);
		delayUs(10); /* В сброшенном состоянии 10 мкс */
		RST(1);

		/* Отправляем базовую команду FUNCTION SET */
		LCD_SendByteData(LCD_COMMAND, FUNCTION_SET_BASE); /* 0011x0xx (RW = 0, RS = 0) */
		/* Ждём более 100 мкс */
		delayUs(110);
		
		/* Отправляем базовую команду FUNCTION SET снова*/
		LCD_SendByteData(LCD_COMMAND, FUNCTION_SET_BASE); /* 0011x0xx (RW = 0, RS = 0) */
		/* Ждём более 37 мкс */
		delayUs(47);
		
		/* Включаем дисплей */
		LCD_SendByteData(LCD_COMMAND, DISPLAY_ON);
		/* Ждём более 100 мкс */
		delayUs(110);
		
		/* Очищаем дисплей */
		LCD_SendByteData(LCD_COMMAND, CLEAR_DISPLAY);
		/* Ждём более 10 мс */
		delayUs(10010);
		
		/* Режим входа: курсор двигается вправо, экран сдвигается вправо */
		LCD_SendByteData(LCD_COMMAND, ENTRY_MODE_SET_RIGHT);
		
		/* Инициализация окончена */
		#ifdef LCD_GRAPHICAL_MODE
			/* Переходим в расширенный режим */
			LCD_SendByteData(LCD_COMMAND, FUNCTION_SET_EXT);  /* 0x30 + RE = 1 */ 
			
			/* Включаем графический режим */
			LCD_SendByteData(LCD_COMMAND, EX_GRAPHIC_DISPLAY_ON);
			
			/* Возвращаемся в базовый режим */
			LCD_SendByteData(LCD_COMMAND, EX_GR_GO_TO_BASIC_FUNC_SET);
			
		#elif defined LCD_CHARACTER_MODE
		/*------ Символьный режим ------*/
			/* Включение курсора и его мигание */
			LCD_SendByteData(LCD_COMMAND, DISPLAY_ON | CURSOR_ON | CURSOR_BLINK_ON);
		#endif
	#elif defined INTERFACE_4BIT_PARALLEL
		/* Ждём более 40 мс после включения питания */
		delayUs(40000);
		
		/* Сбрасываем дисплей */
		RST(0);
		delayUs(10); /* В сброшенном состоянии 10 мкс */
		RST(1);
		
		/* Отправляем базовую команду FUNCTION SET */
		RS(0); /* Запись инструкции: RS = 1, Запись данных: RS = 0 */
		RW(0);         /* Записываем данные: RW = 0 */
		
		E(1);                     /* Поднимаем такотовый сигнал */
		DB7(0);  /* Отправка битов данных или команды */
		DB6(0);  /* Страший бит в DB7, младший в DB0 */
		DB5(1);
		DB4(0);
		E(0);                     /* Опускаем такотовый сигнал */
		
		/* Ожидаем, когда выполнится команда или отправятся данные - 72 мкС */
		delayUs(72);
		delayUs(110);
		
		LCD_SendByteData(LCD_COMMAND, I4_FUNCTION_SET_BASE); /* 0011x0xx (RW = 0, RS = 0) */
		/* Ждём более 100 мкс */
		delayUs(110);
		
		
		/* Включаем дисплей */
		LCD_SendByteData(LCD_COMMAND, DISPLAY_ON);
		/* Ждём более 100 мкс */
		delayUs(110);
		
		/* Очищаем дисплей */
		LCD_SendByteData(LCD_COMMAND, CLEAR_DISPLAY);
		/* Ждём более 10 мс */
		delayUs(10010);
		
		/* Режим входа: курсор двигается вправо, экран сдвигается вправо */
		LCD_SendByteData(LCD_COMMAND, ENTRY_MODE_SET_RIGHT);
		
		/* Инициализация окончена */
		
		/* Переходим в расширенный режим */
		LCD_SendByteData(LCD_COMMAND, 0x24);  /* 0x30 + RE = 1 */ 
		
		/* Включаем графический режим */
		LCD_SendByteData(LCD_COMMAND, 0x26);
		
		/* Возвращаемся в базовый режим */
		LCD_SendByteData(LCD_COMMAND, 0x22);
	#endif
}

//
/*------------- Функции для работы с буфером дисплея -----------------*/
/* Получение горизонтального байта из буфера дисплея по адресу (COL, ROW)
* (В буфере каждый байт представляет собой вертикальный байт пикселей 
* Row - строка байта, Col - столбец байта 
* Возращает горизонтальный байт данных */
uint8_t LCD_GetHorizontalByte(uint8_t Row, uint8_t Col)
{
	uint8_t Byte = 0;
  
	/* Вычисление начального индекса байта */
	uint16_t ByteIdx = (Row >> 3) * DISPLAY_WIDTH;         /* Индекс начального байта строки в буфере */
	ByteIdx += (Col << 3);
  
	/* Битовая маска, которая выбирает строку с восьмью пикселями в буфере */
	uint8_t BitMask = Row % 8;
	BitMask = (1 << BitMask);
  
	/* Выбираем 8 бит */
	for (uint8_t Bit = 0; Bit < 8; Bit++) {
		if (displayBuffer[ByteIdx + Bit] & BitMask)
			Byte |= (1 << (7 - Bit));
	}

	return Byte;
}

/* Установка адреса курсора в буфере дисплея */
void LCD_SetBufferCursor(uint8_t xCoord, uint8_t page)
{
	// Устанавливаем адрес байта в буфере дисплея
	lcdStruct.byteIndex = page*DISPLAY_WIDTH + xCoord;
}
/* Запись одного символа по его индексу в буфер (внутреняя функция)
 * Автоматически следит за индексом текущего байта и переходит на новую страницу или кристалл.
 * symbolIndex - индекс символа из библиотеки символов LIBRARY_SYMBOL, который надо записать. */
static void LCD_WriteSymbolToBuffer(uint16_t symbolIndex)
{	
	/* Максимальное количество символов в строке - 21, максимально на дисплее - 168 */
	/* Если превышен размер дисплея в 168 символов, то печатаем с начала дисплея */
    if(lcdStruct.byteIndex > (LCD_BUFFER_LENGTH - 6)) lcdStruct.byteIndex = 0;
    
	/* Если оставшееся на странице место мешьне, чем размер символа, то переходим на следующую страницу */
    if((DISPLAY_WIDTH - lcdStruct.byteIndex%DISPLAY_WIDTH < 6)) {
        lcdStruct.byteIndex += DISPLAY_WIDTH - lcdStruct.byteIndex%DISPLAY_WIDTH;
    }
	
	/* Записываем один символ в буффер дисплея */
	for(int i = 0; i < 6; i++) {
		displayBuffer[lcdStruct.byteIndex + i] = (uint8_t) LIBRARY_SYMBOL[ (symbolIndex * 6) + i ];
	}
    lcdStruct.byteIndex += 6;
}

/* Получение индекса символа по нашей библиотеки из UTF8 кода символа
 * str - строка, из которой берется символ
 * i - индекс символа, код которого надо получить*/
uint16_t LCD_getUTF8SymbolIndex(const char* str, uint16_t i)
{
	uint16_t symbol;
	//Английский текст и символы. Если до русского текста, то [i] <0xD0. Но в LIBRARY_SYMBOL добавлен знак "°"
	if (str[i] < 0xC0U) {
		symbol = str[i];
		return symbol - 32;
	}
	//Русский текст
	else {
		symbol = str[i] << 8 | str[i + 1];
		if (symbol < 0xD180 && symbol > 0xD081) {
			return symbol - 53297;                  // Таблица UTF-8. Кириллица. С буквы "А" до "п".
		} else if (symbol == 0xD081) {
			return 159;                             // Таблица UTF-8. Кириллица. Буква "Ё".
		
		} else if (symbol == 0xD191) {
			return 160;                             // Таблица UTF-8. Кириллица. Буква "ё".
			
		} else if (symbol == 0xC2B0) {
			return 161;                             // Таблица UTF-8. Basic Latin. Символ "°"		
		} else {
			return symbol - 53489;                  //Таблица UTF-8. Кириллица. С буквы "р", начинается сдвиг.
		}
	}
}
/* Запись строки символов в буфер дисплея (сразу декодирует русские символы)
 * Учитываются особенности записи кириллицы (один русский символ кодируется как два байта).
 * str - срока, которую надо записать (максимум в строке - 21, на всем дисплее - 168 */
void LCD_WriteStringToBuffer(const char* str)
{
	for(int i = 0; str[i] != 0; i++) {
		LCD_WriteSymbolToBuffer(LCD_getUTF8SymbolIndex(str, i));
		if(str[i] >= 0xC0U) i++;
	}
}

/* Запись горизонтальной строки символов в буфер 
 * str - срока, которую надо записать. */
void LCD_writeHorStringToBuffer(const char* str)
{
    uint8_t currentRow = (lcdStruct.byteIndex/DISPLAY_WIDTH+1)*8-1;
    
    int i, j, k, rowCount = (lcdStruct.byteIndex%DISPLAY_WIDTH)/8;
    
	uint16_t symbol = 0;
	for(i = 0; str[i] != 0; i++)
	{
		if(lcdStruct.byteIndex > (LCD_BUFFER_LENGTH - 6)) lcdStruct.byteIndex = 0;
    
        if((DISPLAY_WIDTH - lcdStruct.byteIndex%DISPLAY_WIDTH < 6)){
            lcdStruct.byteIndex += DISPLAY_WIDTH - lcdStruct.byteIndex%DISPLAY_WIDTH;
        }
        
		/*------ Декодирование символа и получение его индекса по библиотеке ------*/
		symbol = LCD_getUTF8SymbolIndex(str, i);
		
		/* Одна итреация - запись одного символа на дисплей */
        for(j = 0; j < 6; j++)
        {
            /* Запись одного горизонтального байта */
            for(k = 0; k < 8; k++) 
			{
				/* Записываем в нужный байт либо единичный бит, либо бит = 0 */
                if((((uint8_t) LIBRARY_SYMBOL[ (symbol * 6) + j ] >> k) & 0x01)){
                    displayBuffer[currentRow/8*DISPLAY_WIDTH + rowCount*8+k] |= 0x01 << currentRow%8;
                } else{
                    displayBuffer[currentRow/8*DISPLAY_WIDTH + rowCount*8+k] &= ~(0x01 << currentRow%8);
                }
            }
            if(currentRow == 3) {
                currentRow = (lcdStruct.byteIndex/DISPLAY_WIDTH+1)*8-1;
                rowCount++;
            }
            currentRow--;
        }
	}
}

/* Стирает выбранную страницы в буфере 
 * page - выбранная страница (PAGE_1 - PAGE_8) */
void LCD_ClearBufferPage(uint8_t page)
{
	for(uint32_t i = page*DISPLAY_WIDTH; i < (page+1)*DISPLAY_WIDTH; i++) {
		displayBuffer[i] = 0;
	}
}
/* Рисование одной выбранной страницы из буфера дидсплея (адекватно работает только на первых 4-х)
 * page - выбранная страница (PAGE_1 - PAGE_4)*/
void LCD_DrawPageFromBuffer(uint8_t page)
{
    for (uint8_t Row = (page << 3); Row < ((page+1) << 3); Row++)
    {
        LCD_SetAddr(Row, 0);
        for (uint8_t Col = 0; Col < 16; Col++){
            LCD_SendByteData(LCD_DATA, LCD_GetHorizontalByte(Row, Col));
        }
    }
}

/* Полное обновления каритинки на дисплее */
void LCD_DisplayFullUpdate(void)
{
    for (uint8_t Row = 0; Row < 32; Row++)
    {
        /* Выбор новой строки */
        LCD_SetAddr(Row, 0);
        
        /* Рисуем одну верхнюю строку пикселей */
        for (uint8_t Col = 0; Col < 16; Col++)
            LCD_SendByteData(LCD_DATA, LCD_GetHorizontalByte(Row, Col));
		
        /* Рисуем одну нижнюю строку пикселей */
        for (uint8_t Col = 0; Col < 16; Col++)
            LCD_SendByteData(LCD_DATA, LCD_GetHorizontalByte(Row + 32, Col));
    }
}

//
/*------------- Функции рисования -----------------*/
/* Рисует все символы из библиотеки (максимум 168) */
void LCD_DrawAllLibSymbolls(void)
{
	/* Очищаем дисплей */
	for(uint16_t i = 0; i < LCD_BUFFER_LENGTH; i++) { displayBuffer[i] = 0; }
	
	/* Записываем сиволы библиотеки в буфер */
	for(uint16_t i = 0; i < 162; i++){
		LCD_WriteSymbolToBuffer(i);
	}
	
	/* Обновляем дисплей */
	LCD_DisplayFullUpdate();
}

/* Очистка или закрашивание всего дисплея 
 * isClear - очистка (CLEAR = 0) или заполнение дисплея (FILL = 1) */
void LCD_ClearOrFillDisplay(uint8_t isClear)
{
	/* Выбор данных, которые будут записаны на дисплей */
    uint8_t data = (isClear == CLEAR ? 0x00 : 0xFF);
    for (uint8_t Row = 0; Row < 32; Row++)
    {
        /* Переходим на первый столбец каждой строки */
        LCD_SetAddr(Row, 0);
    
        /* Рисуем верхнюю линию пикселей */
        for (uint8_t Col = 0; Col < 16; Col++)
            LCD_SendByteData(LCD_DATA, data);
        
        /* Рисуем нижнюю линию пикселей */
        for (uint8_t Col = 0; Col < 16; Col++)
            LCD_SendByteData(LCD_DATA, data);
    }
}

/* Обычный или циклический сдвиг указанной страницы дисплея вправо (или влево) 
 * page - выбранная страница (PAGE_1 - PAGE_8)
 * direct - направление сдвига (RIGHT_SHIFT или LEFT_SHIFT) 
 * isCyclic - является ли сдвиг циклическим (CYCLIC_SHIFT) или обычным (NOT_CYCLIC_SHIFT) */
static void LCD_BufferShiftPage(uint8_t page, uint8_t direct, uint8_t isCyclic)
{
    /* Запоминаем первый или последний элемент, если сдвиг цикличный */
    uint8_t temp;
    if(isCyclic == CYCLIC_SHIFT){
        temp = (direct == RIGHT_SHIFT ? displayBuffer[(page+1)*DISPLAY_WIDTH-1] : displayBuffer[page*DISPLAY_WIDTH]);
    }
    
	/* Сдвиг дисплея вправо */
    if(direct == RIGHT_SHIFT){
        for(int index = (page+1)*DISPLAY_WIDTH-1; index > page*DISPLAY_WIDTH; index--){
            displayBuffer[index] = displayBuffer[index-1];
        }
        displayBuffer[page*DISPLAY_WIDTH] = (isCyclic == CYCLIC_SHIFT ? temp : 0x00);
    } else{
        /* Сдвиг дисплея влево */
        for(int index = page*DISPLAY_WIDTH; index < (page+1)*DISPLAY_WIDTH - 1; index++){
            displayBuffer[index] = displayBuffer[index+1];
        }
        displayBuffer[(page+1)*DISPLAY_WIDTH-1] = (isCyclic == CYCLIC_SHIFT ? temp : 0x00);
    }
}

/* Запуск бегущей строки (обычной или циклической) на указанной странице в указанном направлении (только верхние 4 строки!!!)
 * Сдвиг строки заканчивается либо, когда строка полностью склась за краем экрана, 
 * либо не заканчивается, если она циклична
 * str - указатель на строку, которую надо запустить
 * charCount - количество символов в строке str
 * page - выбранная страница (PAGE_1 - PAGE_4)
 * direct - направление сдвига (RIGHT_SHIFT или LEFT_SHIFT) 
 * isCyclic - является ли сдвиг циклическим (CYCLIC_SHIFT) или обычным (NOT_CYCLIC_SHIFT) */
void LCD_RunLine(const char *str, uint8_t charCount, uint8_t page, uint8_t direct, uint8_t isCyclic)
{
    /* Буфер для хранения текущей бегущей строки */
	//uint8_t runLineBuffer[LCD_BUFFER_LENGTH] = {0x00};
    
	/* Задержка между отрисовками, а также счетчик того, на сколько уже сдвинута строка */
    long runLineDelay = 45000, shiftCount = 0;
	
    /* Начинаем заполнение экрана по столбцам (строка выезжает из-за границы дисплея */
    if(direct == RIGHT_SHIFT){
        /* Сдвиг вправо */
        for(int i = charCount-1; i >= 0; i--) {
			if(str[i] == '\0') continue;
            for(int j = 6; j > 0; j--) {  
                LCD_BufferShiftPage(page, direct, isCyclic);
                shiftCount++;
                displayBuffer[page*DISPLAY_WIDTH] = (uint8_t) LIBRARY_SYMBOL[ (LCD_getUTF8SymbolIndex(str, i) * 6) + j ];
                LCD_DrawPageFromBuffer(page);
                delayUs(runLineDelay);
            }
        }
    } else{
        /* Сдвиг влево */
        for(int i = 0; i < charCount; i++) {
			if(str[i] == '\0') continue;
            for(int j = 0; j < 6; j++) {
                LCD_BufferShiftPage(page, direct, isCyclic);
                shiftCount++;
                displayBuffer[(page+1)*DISPLAY_WIDTH-1] = (uint8_t) LIBRARY_SYMBOL[ (LCD_getUTF8SymbolIndex(str, i) * 6) + j ];
                LCD_DrawPageFromBuffer(page);
                delayUs(runLineDelay); 
            }
        }
    }
    
    /* Выезжание из-за границы закончилось, запуск обычной бегущей строки */
    while(shiftCount < DISPLAY_WIDTH+charCount*6) {
		/* Сдвигаем строку */
        LCD_BufferShiftPage(page, direct, isCyclic);
		
		/* Увеличиваем счетчик */
        shiftCount += (isCyclic == CYCLIC_SHIFT ? 0 : 1);
		
		/* Рисуем страницу */
        LCD_DrawPageFromBuffer(page);
        delayUs(runLineDelay); 
    }
}
