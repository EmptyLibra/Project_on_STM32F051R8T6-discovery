#include "speaker.h"
#include "tim_but_led_pwm.h"

// Song settings struct
Song song;
MusicSet musicSet;

// menu
static int currentItem = 0;
static const char SPEAKER_MENU_ITEMS[][21] = {
    "StarWars - MainTheme",
    "SW - ImperialMarch  ",
    "Bithoven - FurElise ",
    "Pirates of Caribbean",
    "Harry Potter        "
};
static uint8_t menuBuffer[LCD_BUFFER_LENGTH] = {0x00};
//
// play music in system timer
void SysTick_Handler(){
    if(musicSet.noteNumber < song.noteCount){
        // play one note
        if(musicSet.songState != STATE_NOTE_PAUSE && 
                musicSet.elapsed_time < 1000*((uint32_t)((song.wholeNoteLen*song.beats[musicSet.noteNumber])/255))){
            
            // set note
            if(song.freq[musicSet.noteNumber] != P){
                if(musicSet.songState == STATE_NOTE_SET){
                    song.speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(SET): SPEAKER_BIG(SET);
                    musicSet.songState = STATE_NOTE_RESET;
                    
                    // delay one signal half-period
                    if(SysTick_Config(48*(musicSet.signal_period/2))){ while(1); /*trap*/ }
                } else{
                    song.speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(RESET): SPEAKER_BIG(RESET);
                    musicSet.songState = STATE_NOTE_SET;
                    musicSet.elapsed_time += musicSet.signal_period;
                    
                    if(SysTick_Config(48*(musicSet.signal_period/2))){ while(1); /*trap*/ }
                }
            } else{
                musicSet.elapsed_time += musicSet.signal_period;
                if(SysTick_Config(48*(musicSet.signal_period))){ while(1); /*trap*/ }
            }
        } else{
            musicSet.elapsed_time = 0;
            musicSet.noteNumber++;
            musicSet.signal_period = 1000000 / song.freq[musicSet.noteNumber];
            musicSet.songState = STATE_NOTE_SET;
            
            if(SysTick_Config(48*NOTES_PAUSE)){ while(1); /*trap*/ }
        }
    } else if(musicSet.isCyclickSong){
        musicSet.elapsed_time = 0;
        musicSet.signal_period = 1000000 / song.freq[0];
        musicSet.songState = STATE_NOTE_SET;
        musicSet.noteNumber = 0;
        if(SysTick_Config(48000000/1000)){while(1); /*trap*/}
    }
}
void speakerInit(){
    // Settings for small and big speaker
    RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOBEN, ENABLE);
    GPIO_InitTypeDef GPIOB_ini;
    GPIOB_ini.GPIO_Pin = SPEAKER_MINI_PIN | SPEAKER_BIG_PIN;
    GPIOB_ini.GPIO_Mode = GPIO_Mode_OUT;
    GPIOB_ini.GPIO_Speed = GPIO_Speed_50MHz;   // 2, 10, 50
    GPIOB_ini.GPIO_OType = GPIO_OType_PP;
    GPIOB_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIOB_ini);
}

void playSong(uint8_t speakerType, uint16_t *curSong, uint8_t* songBeats, uint16_t curNoteCount, uint8_t curTempo){
    // song init
    song.tempo = curTempo;
    song.wholeNoteLen = 240000/song.tempo;
    song.noteCount = curNoteCount;
    song.freq = curSong;
    song.beats = songBeats;
    uint16_t i;
//    for(i = 0; i < song.noteCount; i++){
//        song.freq[i] = curSong[i];
//        song.beats[i] = songBeats[i];
//    }
    
    // play song
    uint32_t signal_period;
    uint32_t elapsed_time;
    
    // start song
    for (i = 0; i < song.noteCount; i++) {
        elapsed_time = 0;
        signal_period = 1000000 / song.freq[i];
        while (elapsed_time < 1000*((uint32_t)((song.wholeNoteLen*song.beats[i])/255))) {
            if(song.freq[i] != P){
                speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(SET): SPEAKER_BIG(SET);
                delay(signal_period/2);
                speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(RESET): SPEAKER_BIG(RESET);
                delay(signal_period/2);
            } else{
                delay(1000*((uint32_t)((song.wholeNoteLen*song.beats[i])/255)));
                break;
            }
            elapsed_time += signal_period;
            #ifdef BUTTON_BACK
                if(isButtonPressed(BUTTON_BACK)){
                    delay(400000);
                    return;
                }
            #endif
        }
        delay(NOTES_PAUSE);
    }
}

void playBackgroundSong(uint8_t speakerType, const uint16_t *curSong, const uint8_t* songBeats, uint16_t curNoteCount, uint8_t curTempo, uint8_t isCyclick){
    // song init
    song.speakerType = speakerType;
    song.tempo = curTempo;
    song.wholeNoteLen = 240000/song.tempo;
    song.noteCount = curNoteCount;
    song.freq = curSong;
    song.beats = songBeats;
//    uint16_t i;
//    for(i = 0; i < song.noteCount; i++){
//        song.freq[i] = curSong[i];
//        song.beats[i] = songBeats[i];
//    }
    
    // set settings for music
    musicSet.elapsed_time = 0;
    musicSet.signal_period = 1000000 / song.freq[0];
    musicSet.songState = STATE_NOTE_SET;
    musicSet.noteNumber = 0;
    musicSet.isCyclickSong = isCyclick;
    
    // init system timer
    if(SysTick_Config(48000000/1000)){ 
        while(1); // *trap*
    }
}

//
void fillSpeakerMenuBuffer(){
    lcdStruct.byteIndex = 0x00;
    int i;
    for(i = 0; i < SPEAKER_MENU_COUNT; i++){
        lcdStruct.writeSymbolToBuffer(menuBuffer, (i == currentItem) ? '>' : ' ');
        lcdStruct.writeStringToBuffer(menuBuffer, SPEAKER_MENU_ITEMS[i]);
    }
}
void drawChooseMenu(uint8_t choose){
    lcdStruct.byteIndex = 2*DISPLAY_WIDTH;
    lcdStruct.writeStringToBuffer(menuBuffer, (choose == 0? ">small   big" : " small  >big"));
    LCD_DrawPageFromBuffer(menuBuffer, PAGE_3);
}

uint8_t chooseSpeakerType(){    
    lcdStruct.byteIndex = 0x00;
    lcdStruct.writeStringToBuffer(menuBuffer, "Choose speaker");
    lcdStruct.byteIndex = 2*DISPLAY_WIDTH;
    lcdStruct.writeStringToBuffer(menuBuffer, ">small   big");
    lcdStruct.displayFullUpdate(menuBuffer);
    
    uint8_t curChoose = 0;
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delay(10000);
            curChoose -= (curChoose == 0 ? 0 : 1);
            drawChooseMenu(curChoose);
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delay(10000);
            curChoose += (curChoose == 1 ? 0 : 1);
            drawChooseMenu(curChoose);
        }
        if(isButtonPressed(BUTTON_SELECT)){
            delay(10000);
            lcdStruct.clearOrFillDisplay(CLEAR);
            return curChoose;
        }
        delay(100000);
    }
}

void speakerMenu(){
    lcdStruct.clearOrFillDisplay(CLEAR);
    memset(menuBuffer, 0x00, LCD_BUFFER_LENGTH);
    
    uint8_t curSpeaker = chooseSpeakerType();
    
    /*==================Songs library===================================================*/
    /*-----Star Wars - Main Theme-----*/
    const uint16_t StarWars_MainTheme[]       = {F4, F4, F4, A4_D, F5,    D5_D, D5, C5, A5_D,    F5, D5_D, D5, C5, A5_D,    F5, D5_D, D5, D5_D, C5,    F4, F4, F4, A4_D, F5,    D5_D, D5, C5, A5_D,    F5, D5_D, D5, C5, A5_D,    F5, D5_D, D5, D5_D, C5};
    const uint8_t StarWars_MainTheme_Beats[]  = {21, 21, 21, 128, 128,    21,   21, 21, 128,     64, 21,   21, 21, 128,     64, 21,   21, 21,   128,   21, 21, 21, 128, 128,    21,   21, 21, 128,     64, 21,   21, 21, 128,     64, 21,   21, 21,   128};

    /*-----Star Wars - The Imperial March------*/
    const uint16_t StarWars_TheImperialMarch[]       = {A4, P,  A4, P,  A4, P,     F4, P, C5, P,  A4, P,     F4, P, C5, P, A4, P,     E5, P,  E5, P,  E5, P,     F5, P, C5, P, A4_B, P,    F4, P, C5, P, A4, P};
    const uint8_t StarWars_TheImperialMarch_Beats[]  = {50, 20, 50, 20, 50, 20,    40, 5, 20, 5,  60, 10,    40, 5, 20, 5, 60, 80,    50, 20, 50, 20, 50, 20,    40, 5, 20, 5, 60,   10,   40, 5, 20, 5, 60, 40};

    /*-----Ludwig Van Beethoven - Fur Elise-----*/
    const uint16_t FurElise[]         = {E5, D5_D, E5, D5_D, E5, B4, D5, C5, A4, P,    C4, E4, A4, B4, P,    E4, G4_D, B4, C5,  P,   C4, E5, D5_D, E5, D5_D, E5, B4, D5, C5, A4, P,     C4, E4, A4, B4, P,   C4, C5, B4, A4, P};
    const uint8_t FurElise_Beates[]   = {16, 16,   16, 16,   16, 16, 16, 16, 32, 16,   16, 16, 16, 32, 16,   16, 16,   16, 32, 16,   16, 16, 16,   16, 16,   16, 16, 16, 16, 32, 16,    16, 16, 16, 32, 16,  16, 16, 16, 32, 32};
    
//    uint16_t flightOfTheBumblebee[]        = {E6, D6_D, D6, С6_D,   D6, C6, C6, B5,   C6, B5, B5_B, A5,   G5_D, G5, F5_D, F5,    E5, D5_D, D5, С5_D,   D5, C5, C5, B4,   C5, B4, B4_B, A4,   G4_D, G4, F4_D, F4,         E4, D4_D, D4, С4_D,   D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,         E4, D4_D, D4, С4_D,    D4, C4, C4, B3,    C4, С4_D, D4, D4_D,    E4, F4, E4, D4,    E4, D4_D, D4, С4_D,    D4, C4, C4, B3,    C4, С4_D, D4, D4_D,    E4, F4, E4, D4};
//    uint8_t flightOfTheBumblebee_Beates[]  = {16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16,   16,   16,   16, 16,   16,    16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16,   16,   16,   16, 16,   16,         16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,         16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16};
//    
    const uint16_t pirates[]        = {A3, C4,    D4, D4, D4, E4,    F4, F4, F4, G4,    E4, E4, D4, C4,    C4, D4,  P,  A3, C4,    A3, C4,    D4, D4, D4, E4,    F4, F4, F4, G4,    E4, E4, D4, C4,    D4, P, A3, C4,    D4, D4, D4, E4,    G4, G4, G4, A4,    B4, B4, A4, G4,    A4, D4, P, D4, E4,    F4, F4, G4,    A4, D4, P, D4, E4,    E4, E4, F4, D4,    E4, P};
    const uint8_t pirates_Beates[]  = {32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    32, 64,  32, 32, 32,    32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64,32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    32, 64, 32,32, 32,    64, 64, 64,    32, 64, 32,32, 32,    64, 64, 32, 32,    64, 64};
    
    /*-----Harry Potter - Main theme-----*/
    const uint16_t Harry_Potter[]       = {B4,   E5, G5, F5,   E5,   B5,  F5,    E5, G5, F5,  D5,  F5,   B4, P,    B4, E5, G5, F5,   E5, B5,   D6, С6_D,  C6, B5_B,   C6, B5, A5_D,   B4, F5, E5, P};
    const uint8_t Harry_Potter_Beats[]  = {64,   64, 32, 64,   128,  64, 128,    64, 32, 64,  128, 64,   156,64,   64, 64, 32, 64,   128,64,   128,64,    128,64,     64, 32, 64,     128,64, 156,64};
    
        
    fillSpeakerMenuBuffer();
    lcdStruct.displayFullUpdate(menuBuffer);
    
    while(1){
        if(isButtonPressed(BUTTON_TOP)){
            delay(300000);
            currentItem -= (currentItem == 0 ? 0 : 1);
            
            fillSpeakerMenuBuffer();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        
        if(isButtonPressed(BUTTON_BOTOOM)){
            delay(300000);
            currentItem += (currentItem >= SPEAKER_MENU_COUNT-1 ? 0 : 1);
            
            fillSpeakerMenuBuffer();
            lcdStruct.displayFullUpdate(menuBuffer);
        }
        
        if(isButtonPressed(BUTTON_SELECT)){
            delay(500000);
            switch(currentItem){
                case 0:
                    playBackgroundSong(curSpeaker, StarWars_MainTheme, StarWars_MainTheme_Beats, sizeof(StarWars_MainTheme)/2, 108,0);
                    break;
                case 1:
                    playBackgroundSong(curSpeaker, StarWars_TheImperialMarch, StarWars_TheImperialMarch_Beats, sizeof(StarWars_TheImperialMarch)/2, 108,0);
                    break;
                case 2:
                    playBackgroundSong(curSpeaker, FurElise, FurElise_Beates, sizeof(FurElise)/2, 90,0);
                    break;
//                case 3:
//                    playBackgroundSong(curSpeaker, flightOfTheBumblebee, flightOfTheBumblebee_Beates, sizeof(flightOfTheBumblebee)/2, 190,0);
//                    break;
                case 3:
                    playBackgroundSong(curSpeaker, pirates, pirates_Beates, sizeof(pirates)/2, 140,0);
                    break;
                case 4:
                    playBackgroundSong(curSpeaker, Harry_Potter, Harry_Potter_Beats, sizeof(Harry_Potter)/2, 100,0);
                    break;
                default: ;
            }
        }
        
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                musicSet.noteNumber = song.noteCount;
                lcdStruct.clearOrFillDisplay(CLEAR);
                return;
            }
        #endif
    }
}
