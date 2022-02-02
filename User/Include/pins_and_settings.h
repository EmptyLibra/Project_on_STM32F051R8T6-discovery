#ifndef SETTINGS_GUARD
#define SETTINGS_GUARD

//-----------------------------------------------Select debugging board and MCU-----------------------------------------------
/*### select debugging board (only one) ### */
//#define EVAL_220B                                  // so big debugging board with LCD, six buttons, two osc and other
//#define MH046                                      // debugging board with LCD MT-12232A11v3 and four buttons
#define STM32F0_DISCOVERY                          // debugging board with user button and two leds

/*### select microcontroller (only one) and LCD (only one)### */
#ifdef EVAL_220B
    #define MCU_1986BE1T                         // mcu on debugging board EVAL_220B
    #define LCD_12864Av5                         // Lcd MT-12864Av5 on EVAL_220B (128x64)
#elif defined MH046
    #define MCU_1986BE4Y1                        // mcu with lcd MT-12232A11v3 and four buttons
    #define LCD_12232A11v3                       // LCD MT-12232A11v3 on MH046 (122x32)
#elif defined STM32F0_DISCOVERY
    #define MCU_STM32F051R8T6
    #define LCD_ERM12864B
#endif

/*### Other Settings ###*/
/* Enable LCD */
#if defined LCD_12864Av5 || defined LCD_12232A11v3 || defined LCD_ERM12864B
    #define LCD_ENABLE                           // enable special struct and defines for LCD
#endif

/* Enable UART, enable timer */
#define UART_ENABLE
#define TIMER_ENABLE

//
//-----------------------------------------------Select includes and pins-----------------------------------------------
/* select include main lib */
#ifdef MCU_1986BE1T
    #include "MDR1986VE1T.h"          // main CMSIS file for MCU 1986BE1T
    #include <MDR32F9Qx_port.h>       // file for ports init
    #include <MDR32F9Qx_rst_clk.h>    // file for enable clocking on ports
#elif defined MCU_1986BE4Y1
    #include "MDR1986BE4.h"           // main CMSIS file for MCU 1986BE4Y1
    #include <MDR32F9Qx_port.h>       // file for ports init
    #include <MDR32F9Qx_rst_clk.h>    // file for enable clocking on ports
#elif defined MCU_STM32F051R8T6
    #include "stm32f0xx.h"
    #include "stm32f0xx_rcc.h"
    #include "stm32f0xx_gpio.h"
#endif

/* Select timer port */
#ifdef TIMER_ENABLE
    #if defined MCU_1986BE1T || defined MCU_1986BE4Y1
        #define TIMER MDR_TIMER1
    #elif defined MCU_STM32F051R8T6
        #define TIMER TIM3
    #endif
#endif

/* Select UART includes and settings*/
#ifdef UART_ENABLE
    #ifdef STM32F0_DISCOVERY
        #include "stm32f0xx_usart.h"
        
        #define USART1_PIN_RX GPIO_Pin_10  // PA10 - RX (USART1)
		#define USART1_PIN_TX GPIO_Pin_9   // PA9 - TX (USART1)
        
        #define UART1 USART1
        #define UART_SEND_DATA(data) USART_SendData(UART1, data)
        #define UART_RECEIVE_DATA() ((uint8_t) USART_ReceiveData(UART1))
    #endif
#endif

/* select pins for buttons, leds and else */
#if defined EVAL_220B && defined MCU_1986BE1T
    // six-buttons keyboard on port E
    #define BUTTONS_PORT          MDR_PORTE
    #define RST_CLK_BUTTONS_PORT  RST_CLK_PCLK_PORTE
    
    #define BUTTON_RIGHT          PORT_Pin_5   // (SW6  - PE5)
    #define BUTTON_TOP            PORT_Pin_8   // (SW7  - PE8)
    #define BUTTON_LEFT           PORT_Pin_10  // (SW8  - PE10)
    #define BUTTON_BOTOOM         PORT_Pin_15  // (SW9  - PE15)
    #define BUTTON_SELECT         PORT_Pin_9   // (SW10 - PE9)
    #define BUTTON_BACK           PORT_Pin_11  // (SW11 - PE11)
    
    // eight leds on port D
    #define LEDS_PORT             MDR_PORTD
    #define RST_CLK_LEDS_PORT     RST_CLK_PCLK_PORTD
    #define VD6_PIN               PORT_Pin_7
    #define VD7_PIN               PORT_Pin_8
    #define VD8_PIN               PORT_Pin_9
    #define VD9_PIN               PORT_Pin_10
    #define VD10_PIN              PORT_Pin_11
    #define VD11_PIN              PORT_Pin_12
    #define VD12_PIN              PORT_Pin_13
    #define VD13_PIN              PORT_Pin_14
    
    // set/reset leds pin
    #define VD6(mode)   ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD6_PIN,  Bit_SET)  : PORT_WriteBit(LEDS_PORT,  VD6_PIN,  Bit_RESET))
    #define VD7(mode)   ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD7_PIN,  Bit_SET)  : PORT_WriteBit(LEDS_PORT,  VD7_PIN,  Bit_RESET))
    #define VD8(mode)   ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD8_PIN,  Bit_SET)  : PORT_WriteBit(LEDS_PORT,  VD8_PIN,  Bit_RESET))
    #define VD9(mode)   ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD9_PIN,  Bit_SET)  : PORT_WriteBit(LEDS_PORT,  VD9_PIN,  Bit_RESET))
    #define VD10(mode)  ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD10_PIN, Bit_SET)  : PORT_WriteBit(LEDS_PORT,  VD10_PIN, Bit_RESET))
    #define VD11(mode)  ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD11_PIN,  Bit_SET) : PORT_WriteBit(LEDS_PORT,  VD11_PIN, Bit_RESET))
    #define VD12(mode)  ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD12_PIN,  Bit_SET) : PORT_WriteBit(LEDS_PORT,  VD12_PIN, Bit_RESET))
    #define VD13(mode)  ((mode) == 1 ? PORT_WriteBit(LEDS_PORT,  VD13_PIN,  Bit_SET) : PORT_WriteBit(LEDS_PORT,  VD13_PIN, Bit_RESET))
    
#elif defined MH046 && defined MCU_1986BE4Y1
    // four-buttons keyboard on port B
    #define BUTTONS_PORT     MDR_PORTB
    #define BUTTON_TOP       PORT_Pin_10 // (S4)
    #define BUTTON_LEFT      PORT_Pin_9  // (S3)
    #define BUTTON_RIGHT     PORT_Pin_8  // (S2)
    #define BUTTON_BOTOOM    PORT_Pin_7  // (S1)
    // # without leds #

#elif defined STM32F0_DISCOVERY && defined MCU_STM32F051R8T6
    // six-buttons keyboard on port A and port F and one additional user  button
    
    // four buttons on port A
    #define BUTTONS_BRTL_PORT     GPIOA
    #define BUTTON_BACK     GPIO_Pin_0    // PA0
    #define BUTTON_RIGHT    GPIO_Pin_1    // PA1
    #define BUTTON_TOP      GPIO_Pin_2    // PA2
    #define BUTTON_LEFT     GPIO_Pin_3    // PA3
    
    #define BUTTONS_BOTOOM_SEL_PORT   GPIOF
    #define BUTTON_BOTOOM   GPIO_Pin_4    // PF4
    #define BUTTON_SELECT   GPIO_Pin_5    // PF5
    
    #define USER_BUTTON     GPIO_Pin_0    // PA0
    
    
    // two leds on port C
    #define LEDS_PORT       GPIOC
    #define LD3_PIN            GPIO_Pin_9    // PC9
    #define LD4_PIN            GPIO_Pin_8    // PC8
    
    #define LD3(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOC, LD3_PIN, Bit_SET)  : GPIO_WriteBit(GPIOC, LD3_PIN, Bit_RESET))
    #define LD4(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOC, LD4_PIN, Bit_SET)  : GPIO_WriteBit(GPIOC, LD4_PIN, Bit_RESET))

#endif

/* LCD pins and settings*/
#if defined LCD_12864Av5 && defined LCD_ENABLE    // (LCD 128x64)
    // display width, height
    #define DISPLAY_WIDTH 128
    #define DISPLAY_HEIGHT 64
    
    /* LCD Pins (8-bits interface only)
     * 1.  GND       - general output  
     * 2.  Ucc       - module power supply
     * 3.  U0        - lcd panel power input
     * 4.  A0        - select command or data
     * 5.  RW        - select read or write
     * 6.  E         - data strobing
     * 7-14. DB0-DB7 - data bus
     * 15. E1        - select cristall 1
     * 16. E2        - select cristall 2
     * 17. RES       - reset
     * 18. Uee       - Output DC-DC convector 
     * 19. A         - plus the backlight power supply
     * 20. K         - minus the backlight power supply*/
    // ports for data and control pins
    #define LCD_DATA_BUS_PORT   MDR_PORTA
    #define LCD_RW_E_PORT       MDR_PORTC
    #define LCD_A0_E1_E2_PORT   MDR_PORTE
    
    // LCD control pins
    #define RW_PIN  PORT_Pin_0   // PC0
    #define E_PIN   PORT_Pin_2   // PC2
    #define A0_PIN  PORT_Pin_12  // PE12
    #define E1_PIN  PORT_Pin_13  // PE13
    #define E2_PIN  PORT_Pin_14  // PE14
    
    // LCD data pins
    #define DB0_PIN PORT_Pin_0  // PA0
    #define DB1_PIN PORT_Pin_1  // PA1
    #define DB2_PIN PORT_Pin_2  // PA2
    #define DB3_PIN PORT_Pin_3  // PA3
    #define DB4_PIN PORT_Pin_4  // PA4
    #define DB5_PIN PORT_Pin_5  // PA5
    #define DB6_PIN PORT_Pin_6  // PA6
    #define DB7_PIN PORT_Pin_7  // PA7
    
    // set/reset control and data pins
    #define RW(mode)  ((mode) == 1 ? PORT_WriteBit(LCD_RW_E_PORT,      RW_PIN,  Bit_SET)  : PORT_WriteBit(LCD_RW_E_PORT,     RW_PIN,  Bit_RESET))
    #define E(mode)   ((mode) == 1 ? PORT_WriteBit(LCD_RW_E_PORT,      E_PIN,   Bit_SET)  : PORT_WriteBit(LCD_RW_E_PORT,     E_PIN,   Bit_RESET))
    #define A0(mode)  ((mode) == 1 ? PORT_WriteBit(LCD_A0_E1_E2_PORT,  A0_PIN,  Bit_SET)  : PORT_WriteBit(LCD_A0_E1_E2_PORT, A0_PIN,  Bit_RESET))
    #define E1(mode)  ((mode) == 1 ? PORT_WriteBit(LCD_A0_E1_E2_PORT,  E1_PIN,  Bit_SET)  : PORT_WriteBit(LCD_A0_E1_E2_PORT, E1_PIN,  Bit_RESET))
    #define E2(mode)  ((mode) == 1 ? PORT_WriteBit(LCD_A0_E1_E2_PORT,  E2_PIN,  Bit_SET)  : PORT_WriteBit(LCD_A0_E1_E2_PORT, E2_PIN,  Bit_RESET))

    #define DB7(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB7_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB7_PIN, Bit_RESET))
    #define DB6(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB6_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB6_PIN, Bit_RESET))
    #define DB5(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB5_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB5_PIN, Bit_RESET))
    #define DB4(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB4_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB4_PIN, Bit_RESET))
    #define DB3(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB3_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB3_PIN, Bit_RESET))
    #define DB2(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB2_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB2_PIN, Bit_RESET))
    #define DB1(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB1_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB1_PIN, Bit_RESET))
    #define DB0(mode) ((mode) == 1 ? PORT_WriteBit(LCD_DATA_BUS_PORT,  DB0_PIN, Bit_SET)  : PORT_WriteBit(LCD_DATA_BUS_PORT, DB0_PIN, Bit_RESET))
    
    // cristall select
    #define LEFT_CRYSTAL 0x01   // E1 = 1
    #define RIGHT_CRYSTAL 0x02  // E2 = 1
    
    // commands for LCD12232A
    #define DISPLAY_ON          0x3F  // display on
    #define DISPLAY_START_LINE  0xC0  // defines first lcd line
    #define SET_PAGE_COMMAND    0xB8  // set page [0,7]
    #define PAGE_MASK           0x07  // bit mask for page
    #define SET_ADDR_COMMAND    0x40  // set col address [0, 63]
    #define ADDR_MASK           0x3F  // bit mask for addr
    
#elif defined LCD_12232A11v3 && defined LCD_ENABLE  // (LCD 122x32)
    // display width, height
    #define DISPLAY_WIDTH 122
    #define DISPLAY_HEIGHT 32

    /* LCD Pins (8-bits interface only)
     * 1-4.  DB4-DB7 - data bus
     * 5.    A0      - select command or data
     * 6.    RW      - select read or write
     * 7.    E       - data strobing
     * 8-11. DB3-DB0 - data bus
     * 12.   GND     - general output
     * 13.   U0      - contrast control
     * 14.   Ucc     - supply voltage
     * 15.   K       - minus the backlight power supply
     * 16.   A       - plus the backlight power supply
     * 17.   RES     - reset
     * 18.   CS      - crystall select*/
    // ports for data and control pins
    #define LCD_DATA_BUS_PORT MDR_PORTA
    #define LCD_CONTROL_PORT  MDR_PORTB
    
    // LCD control pins
    #define A0_PIN  PORT_Pin_2  // PB2
    #define RW_PIN  PORT_Pin_3  // PB3
    #define E_PIN   PORT_Pin_4  // PB4
    #define RES_PIN PORT_Pin_5  // PB5
    #define CS_PIN  PORT_Pin_6  // PB6
    
    // LCD data pins
    #define DB0_PIN PORT_Pin_8  // PA8
    #define DB1_PIN PORT_Pin_9  // PA9
    #define DB2_PIN PORT_Pin_10 // PA10
    #define DB3_PIN PORT_Pin_11 // PA11
    #define DB4_PIN PORT_Pin_12 // PA12
    #define DB5_PIN PORT_Pin_13 // PA13
    #define DB6_PIN PORT_Pin_14 // PA14
    #define DB7_PIN PORT_Pin_15 // PA15
    
    // set/reset control and data pins
    #define A0(mode)  ((mode) == 1 ? PORT_WriteBit(MDR_PORTB,  A0_PIN,  Bit_SET)  : PORT_WriteBit(MDR_PORTB, A0_PIN,  Bit_RESET))
    #define RW(mode)  ((mode) == 1 ? PORT_WriteBit(MDR_PORTB,  RW_PIN,  Bit_SET)  : PORT_WriteBit(MDR_PORTB, RW_PIN,  Bit_RESET))
    #define E(mode)   ((mode) == 1 ? PORT_WriteBit(MDR_PORTB,  E_PIN,   Bit_SET)  : PORT_WriteBit(MDR_PORTB, E_PIN,   Bit_RESET))
    #define RES(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTB,  RES_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTB, RES_PIN, Bit_RESET))
    #define CS(mode)  ((mode) == 1 ? PORT_WriteBit(MDR_PORTB,  CS_PIN,  Bit_SET)  : PORT_WriteBit(MDR_PORTB, CS_PIN,  Bit_RESET))

    #define DB7(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB7_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB7_PIN, Bit_RESET))
    #define DB6(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB6_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB6_PIN, Bit_RESET))
    #define DB5(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB5_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB5_PIN, Bit_RESET))
    #define DB4(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB4_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB4_PIN, Bit_RESET))
    #define DB3(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB3_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB3_PIN, Bit_RESET))
    #define DB2(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB2_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB2_PIN, Bit_RESET))
    #define DB1(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB1_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB1_PIN, Bit_RESET))
    #define DB0(mode) ((mode) == 1 ? PORT_WriteBit(MDR_PORTA,  DB0_PIN, Bit_SET)  : PORT_WriteBit(MDR_PORTA, DB0_PIN, Bit_RESET))
    
    // commands for LCD12232A
    #define END_COMMAND                0xEE               // removing the RMW flag
    #define STATIC_DRIVE_OFF_COMMAND   0xA4               // select normal operation mode
    #define DUTY_SELECT_COMMAND        0xA9               // choosing a multiplexer
    #define ADC_SELECT_FORWARD_COMMAND 0xA0               // forward correspondence between the column address and the position
    #define ADC_SELECT_REVERSE_COMMAND 0xA1               // reverse correspondence between the column address and the position
    #define DISPLAY_ON_COMMAND         0xAF               // switch on display
    
    // cristall select
    #define RIGHT_CRYSTAL 0
    #define LEFT_CRYSTAL 1
    
#elif defined STM32F0_DISCOVERY && defined LCD_ERM12864B // LCD 128x64
    // display width, height
    #define DISPLAY_WIDTH 128
    #define DISPLAY_HEIGHT 64
    
    //-----LCD Pins (8-bits interface only)----------------------------------------------------------------------------------------------------------
    /* 1.  GND       - general output  
     * 2.  Ucc       - module power supply
     * 3.  U0        - lcd panel power input
     * 4.  RS        - select command or data
     * 5.  RW        - select read or write
     * 6.  E         - data strobing
     * 7-14. DB0-DB7 - data bus
     * 15. PSB       - select parallel (1) or serial interface (0)
     * 16. NC        - no connection
     * 17. RST       - reset
     * 18. Uout      - Output DC-DC convector 
     * 19. BLA       - plus the backlight power supply
     * 20. BLK       - minus the backlight power supply*/
    // LCD control pins
    #define RS_PIN   GPIO_Pin_12   // PA12 - RS   (0 - instruction, 1 - data)(A0 on Milander Display)
    #define RW_PIN   GPIO_Pin_6    // PF6  - R/W  (0 - write, 1- read)
    #define E_PIN    GPIO_Pin_7    // PF7  - E    (enable trigger)
    #define RST_PIN  GPIO_Pin_7    // PB7  - RST  (0 - reset system)  (RES on Milander Display)
    
    // LCD data pins
    #define DB0_PIN  GPIO_Pin_10   // PC10 - DB0 
    #define DB1_PIN  GPIO_Pin_11   // PC11 - DB1 
    #define DB2_PIN  GPIO_Pin_12   // PC12 - DB2 
    #define DB3_PIN  GPIO_Pin_2    // PD2  - DB3 
    #define DB4_PIN  GPIO_Pin_3    // PB3  - DB4 
    #define DB5_PIN  GPIO_Pin_4    // PB4  - DB5
    #define DB6_PIN  GPIO_Pin_5    // PB5  - DB6
    #define DB7_PIN  GPIO_Pin_6    // PB6  - DB7

    //Set or reset lcd pins
    #define RS(mode)  ((mode) == 1 ? GPIO_WriteBit(GPIOA, RS_PIN,  Bit_SET) : GPIO_WriteBit(GPIOA, RS_PIN,  Bit_RESET))
    #define RW(mode)  ((mode) == 1 ? GPIO_WriteBit(GPIOF, RW_PIN,  Bit_SET) : GPIO_WriteBit(GPIOF, RW_PIN,  Bit_RESET))
    #define E(mode)   ((mode) == 1 ? GPIO_WriteBit(GPIOF, E_PIN,   Bit_SET) : GPIO_WriteBit(GPIOF, E_PIN,   Bit_RESET))
    #define RST(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOB, RST_PIN, Bit_SET) : GPIO_WriteBit(GPIOB, RST_PIN, Bit_RESET))

    #define DB0(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOC, DB0_PIN, Bit_SET) : GPIO_WriteBit(GPIOC, DB0_PIN, Bit_RESET))
    #define DB1(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOC, DB1_PIN, Bit_SET) : GPIO_WriteBit(GPIOC, DB1_PIN, Bit_RESET))
    #define DB2(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOC, DB2_PIN, Bit_SET) : GPIO_WriteBit(GPIOC, DB2_PIN, Bit_RESET))
    #define DB3(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOD, DB3_PIN, Bit_SET) : GPIO_WriteBit(GPIOD, DB3_PIN, Bit_RESET))
    #define DB4(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOB, DB4_PIN, Bit_SET) : GPIO_WriteBit(GPIOB, DB4_PIN, Bit_RESET))
    #define DB5(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOB, DB5_PIN, Bit_SET) : GPIO_WriteBit(GPIOB, DB5_PIN, Bit_RESET))
    #define DB6(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOB, DB6_PIN, Bit_SET) : GPIO_WriteBit(GPIOB, DB6_PIN, Bit_RESET))
    #define DB7(mode) ((mode) == 1 ? GPIO_WriteBit(GPIOB, DB7_PIN, Bit_SET) : GPIO_WriteBit(GPIOB, DB7_PIN, Bit_RESET))
    
    //-----Commands for LCD12864B----------------------------------------------------------------------------------------------------------
    #define FUNCTION_SET_BASE    0x30    // select function for display (8-bit interface)
    #define DISPLAY_ON           0x0C    // display on
    #define CLEAR_DISPLAY        0x01    // clear display
    #define ENTRY_MODE_RS        0x06    // set cursor position right and display no shift
    #define SET_GDRAM_ADDR       0x80    // set graphical ram address
    #define FUNCTION_SET_EXT     0x34    // Go to extended instruction
    #define ENABLE_GRAFICAL_MODE 0x36    // Enable graphical mode
    
    //----Page numbers)--------------------------------------------------------------------------------------------------------------------
    #define PAGE_1 0x00
    #define PAGE_2 0x01
    #define PAGE_3 0x02
    #define PAGE_4 0x03
    #define PAGE_5 0x04
    #define PAGE_6 0x05
    #define PAGE_7 0x06
    #define PAGE_8 0x07
#endif

//
//----------------------------------------------Other settings and function declarations-------------------------------
/* special struct for LCD */
#ifdef LCD_ENABLE
    // clear or fill all display
    #define CLEAR 0
    #define FILL 1
    
    // display buffer length
    #define LCD_BUFFER_LENGTH (DISPLAY_HEIGHT*DISPLAY_WIDTH)/8
    
    typedef struct LCD_STRUCT{
        //---Variables--------
        uint16_t byteIndex;
        
        //---Functions------------------
        // init LCD functions
        void (*portsInit) (void);                                         // init all LCD pins
        void (*init) (void);                                              // init and switch on LCD
        
        // functions for display buffer
        void (*writeSymbolToBuffer) (uint8_t *pBuff, char sumbol);        // write one any symbol to display buffer
        void (*writeStringToBuffer) (uint8_t *pBuff, const char* str);    // write any string to display buffer
        void (*displayFullUpdate)   (uint8_t *pBuff);                     // draw all display buffer to display
        
        // other functions
        void (*sendByteData)        (uint8_t byte);                       // draw one byte to display
        void (*clearOrFillDisplay)  (uint8_t isFILL);                     // clear all display or fill all FILL
    } LCD_STRUCT;
#endif
//
// general function declarations
extern void delay(long mcs);                                  // delay over 1 mcs

extern uint8_t isLongButtonResetPressed(uint16_t portPin);    // long button press
extern void ledPinsInit(void);                                // initialize leds
extern void buttonPinsInit(void);                             // initialise buttons
    
uint8_t isButtonPressed(uint16_t pin);

#endif
