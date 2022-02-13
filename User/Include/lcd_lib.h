#ifndef LCD_HEADER
#define LCD_HEADER

#include "pins_and_settings.h"

/*==============================Defines==========================================*/
// direction of the shift for the running line
#define RIGHT_SHIFT      0
#define LEFT_SHIFT       1
#define UP_SHIFT         2
#define CYCLIC_SHIFT     0
#define NOT_CYCLIC_SHIFT 1

/*===========================Structs and Variables==================================*/
extern LCD_STRUCT lcdStruct;                                              // struct with main functions and display buffer
extern const uint8_t LIBRARY_SYMBOL[];                                          // library with all symbols in ASCII
    
/*===========================Functions==================================*/
//-----init functions--------------------------------------------------------------
void LCD_PortsInit(void);                                                 // initializes all LCD pins
void LCD_Init(void);                                                      // init LCD display and pins

//-----send functions-----------------------------------------------------------
void LCD_SendCommand(uint8_t byte);                                       // send command or data to GDRAM
void LCD_SendByteData(uint8_t byte);
void LCD_SetAddr(uint8_t vertAddr, uint8_t horizAddr);                    // set addres in GDRAM (auto increase)

//-----functions with displayBuffer-------------------------------------------
uint8_t LCD_GetHorizontalByte(uint8_t *pBuff, uint8_t Row, uint8_t Col);
void LCD_BufferShiftPage(uint8_t *pBuff, uint8_t page, uint8_t direct, uint8_t isCyclic);
void LCD_WriteSymbolToBuffer(uint8_t *pBuff, char sumbol);
void LCD_WriteStringToBuffer(uint8_t *pBuff, const char* str);
void LCD_WritePartOfStringToBuffer(uint8_t *pBuff, const char* str, uint8_t count);
void LCD_DrawPageFromBuffer(uint8_t *pBuff, uint8_t page);
void LCD_DisplayFullUpdate(uint8_t *pBuff);

//----draw functions-----------------------------------------------------
void LCD_ClearOrFillDisplay(uint8_t isClear);
void LCD_RunLine(const char *str, uint8_t charCount, uint8_t page, uint8_t direct, uint8_t isCyclic);

#endif
