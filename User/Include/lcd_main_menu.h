#ifndef LCD_MAIN_MENU_HEADER
#define LCD_MAIN_MENU_HEADER

#include "lcd_lib.h"
#include "tim_but_led_pwm.h"
#include "infrared_receiver.h"
#include "speaker.h"
#include "game_2048.h"
#include "game_tetris.h"
#include "snake.h"
#include "my_uart.h"
#include <string.h>  // for memset

#define MENU_COUNT sizeof(MENU_ITEMS)/sizeof(MENU_ITEMS[0])
#define DAHATSU_MENU_COUNT sizeof(DAHATSU_MENU_ITEMS)/sizeof(DAHATSU_MENU_ITEMS[0])

#define  IR_MODE_TEST 0 
#define  IR_MODE_CAPTURE 1
#define  IR_MODE_SEND 2 
//-----------------------------Variables----------------------------------------
static void LCD_FillMenuBuffer(void);
extern void LCD_StartMainMenu(void);

#endif
