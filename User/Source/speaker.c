#include "speaker.h"

/* Play music on selected speaker - small (without amplifier) or big (with amplifier).
*  Small speaker on PB12, Big speaker on PB11.
*  Music can be played in the background (playBackgroundSong function) using system timer or 
*  not in background (playSong), but this stops the MCU work. */

/*===========================Variables==================================*/
Song song;
MusicSet musicSet;            // Music settings struct

// menu
static int currentItem = 0;
static const char SPEAKER_MENU_ITEMS[][21] = {
    "StarWars - MainTheme",
    "SW - ImperialMarch  ",
    "Bithoven - FurElise ",
    "Pirates of Caribbean",
    "Harry Potter        "
};

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

//    const uint16_t flightOfTheBumblebee[]        = {E6, D6_D, D6, С6_D,   D6, C6, C6, B5,   C6, B5, B5_B, A5,   G5_D, G5, F5_D, F5,    E5, D5_D, D5, С5_D,   D5, C5, C5, B4,   C5, B4, B4_B, A4,   G4_D, G4, F4_D, F4,         E4, D4_D, D4, С4_D,   D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,   E4, D4_D, D4, С4_D,    D4, C4, C4, B3,         E4, D4_D, D4, С4_D,    D4, C4, C4, B3,    C4, С4_D, D4, D4_D,    E4, F4, E4, D4,    E4, D4_D, D4, С4_D,    D4, C4, C4, B3,    C4, С4_D, D4, D4_D,    E4, F4, E4, D4};
//    const uint8_t flightOfTheBumblebee_Beates[]  = {16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16,   16,   16,   16, 16,   16,    16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16,   16,   16,   16, 16,   16,         16, 16,   16, 16,     16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,   16, 16, 16, 16,        16, 16, 16, 16,         16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16,    16, 16,   16, 16,      16, 16, 16, 16};

/*-----Pirates of the Caribbean - Main Theme-----*/
const uint16_t pirates[]        = {A3, C4,    D4, D4, D4, E4,    F4, F4, F4, G4,    E4, E4, D4, C4,    C4, D4,  P,  A3, C4,    A3, C4,    D4, D4, D4, E4,    F4, F4, F4, G4,    E4, E4, D4, C4,    D4, P, A3, C4,    D4, D4, D4, E4,    G4, G4, G4, A4,    B4, B4, A4, G4,    A4, D4, P, D4, E4,    F4, F4, G4,    A4, D4, P, D4, E4,    E4, E4, F4, D4,    E4, P};
const uint8_t pirates_Beates[]  = {32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    32, 64,  32, 32, 32,    32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64,32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    64, 64, 32, 32,    32, 64, 32,32, 32,    64, 64, 64,    32, 64, 32,32, 32,    64, 64, 32, 32,    64, 64};

/*-----Harry Potter - Main theme-----*/
const uint16_t Harry_Potter[]       = {B4,   E5, G5, F5,   E5,   B5,  F5,    E5, G5, F5,  D5,  F5,   B4, P,    B4, E5, G5, F5,   E5, B5,   D6, С6_D,  C6, B5_B,   C6, B5, A5_D,   B4, F5, E5, P};
const uint8_t Harry_Potter_Beats[]  = {64,   64, 32, 64,   128,  64, 128,    64, 32, 64,  128, 64,   156,64,   64, 64, 32, 64,   128,64,   128,64,    128,64,     64, 32, 64,     128,64, 156,64};

const uint16_t TetrisGameSong[]       = {C5,    E5, B4_B, C5, D5, E5, D5, B4, A4,    A4_D, A4, C5, E5, D5, C5,    G4_D, G4, A4, B4, E5,    C5, A4, A4, P,     F4, A4, C5, B4, A4,    G4, E4, G4, F4, E4,    G4_D, G4, B4, A4, E5,     C5, B4, B4, P};
const uint8_t TetrisGameSong_Beats[]  = {64,    64, 32,   32, 32, 16, 16, 32, 32,    64,   32, 32, 64, 32, 32,    64,   32, 32, 64, 64,    64, 64, 64, 64,    64, 32, 64, 32, 32,    64, 32, 64, 32, 32,    64,   32, 32, 64, 64,     64, 64, 64, 64};

//
/*===========================Functions==================================*/
//void SysTick_Handler(){ // system timer handler
//    if(musicSet.noteNumber < song.noteCount && musicSet.isSongPlay == 1){
//        // play one note
//        if(musicSet.songState != STATE_NOTE_PAUSE && 
//                musicSet.elapsed_time < 1000*((uint32_t)((song.wholeNoteLen*song.beats[musicSet.noteNumber])/255))){
//            
//            // set note
//            if(song.freq[musicSet.noteNumber] != P){
//                if(musicSet.songState == STATE_NOTE_SET){
//                    song.speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(SET): SPEAKER_BIG(SET);
//                    musicSet.songState = STATE_NOTE_RESET;
//                    
//                    // delayUs one signal half-period
//                    if(SysTick_Config(48*(musicSet.signal_period/2))){ while(1); /*trap*/ }
//                } else{
//                    song.speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(RESET): SPEAKER_BIG(RESET);
//                    musicSet.songState = STATE_NOTE_SET;
//                    musicSet.elapsed_time += musicSet.signal_period;
//                    
//                    if(SysTick_Config(48*(musicSet.signal_period/2))){ while(1); /*trap*/ }
//                }
//            } else{
//                musicSet.elapsed_time += musicSet.signal_period;
//                if(SysTick_Config(48*(musicSet.signal_period))){ while(1); /*trap*/ }
//            }
//        } else{
//            musicSet.elapsed_time = 0;
//            musicSet.noteNumber++;
//            musicSet.signal_period = 1000000 / song.freq[musicSet.noteNumber];
//            musicSet.songState = STATE_NOTE_SET;
//            
//            if(SysTick_Config(48*NOTES_PAUSE)){ while(1); /*trap*/ }
//        }
//    } else if(musicSet.isCyclickSong){
//        musicSet.elapsed_time = 0;
//        musicSet.signal_period = 1000000 / song.freq[0];
//        musicSet.songState = STATE_NOTE_SET;
//        musicSet.noteNumber = 0;
//        if(SysTick_Config(48000000/1000)){while(1); /*trap*/}
//    }
//}
void speakerInit(){ // Settings for small and big speaker
    RCC_AHBPeriphClockCmd(RCC_AHBENR_SPEAKEREN, ENABLE);
    GPIO_InitTypeDef GPIOx_ini;
    GPIOx_ini.GPIO_Pin = SPEAKER_MINI_PIN | SPEAKER_BIG_PIN;
    GPIOx_ini.GPIO_Mode = GPIO_Mode_OUT;
    GPIOx_ini.GPIO_Speed = GPIO_Speed_50MHz;   // 2, 10, 50
    GPIOx_ini.GPIO_OType = GPIO_OType_PP;
    GPIOx_ini.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(SPEAKER_PORT, &GPIOx_ini);
}

void playSong(uint8_t speakerType, const uint16_t *curSong, const uint8_t* songBeats, uint16_t curNoteCount, uint8_t curTempo){
    // song init
    song.tempo = curTempo;
    song.wholeNoteLen = 240000/song.tempo;
    song.noteCount = curNoteCount;
    song.freq = curSong;
    song.beats = songBeats;
    
    // play song
    uint32_t signal_period;
    uint32_t elapsed_time;
    
    // start song
    uint16_t i;
    for (i = 0; i < song.noteCount; i++) {
        elapsed_time = 0;
        signal_period = 1000000 / song.freq[i];
        while (elapsed_time < 1000*((uint32_t)((song.wholeNoteLen*song.beats[i])/255))) {
            if(song.freq[i] != P){
                speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(SET): SPEAKER_BIG(SET);
                delayUs(signal_period/2);
                speakerType == SPEAKER_TYPE_MINI ? SPEAKER_MINI(RESET): SPEAKER_BIG(RESET);
                delayUs(signal_period/2);
            } else{
                delayUs(1000*((uint32_t)((song.wholeNoteLen*song.beats[i])/255)));
                break;
            }
            elapsed_time += signal_period;
            #ifdef BUTTON_BACK
                if(isButtonPressed(BUTTON_BACK)){
                    delayUs(400000);
                    return;
                }
            #endif
        }
        delayUs(NOTES_PAUSE);
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
    
    // set settings for music
    musicSet.isSongPlay = 1;
    musicSet.elapsed_time = 0;
    musicSet.signal_period = 1000000 / song.freq[0];
    musicSet.songState = STATE_NOTE_SET;
    musicSet.noteNumber = 0;
    musicSet.isCyclickSong = isCyclick;
    
    // init system timer
    if(SysTick_Config(48000000/1000)){ while(1); /*trap*/}
}

//
// speaker menu
void fillSpeakerMenuBuffer(){
    lcdStruct.byteIndex = 0x00;
    int i;
    for(i = 0; i < SPEAKER_MENU_COUNT; i++){
        lcdStruct.writeStringToBuffer((i == currentItem) ? ">" : " ");
        lcdStruct.writeStringToBuffer(SPEAKER_MENU_ITEMS[i]);
    }
}
void drawChooseMenu(uint8_t choose){
    lcdStruct.byteIndex = 2*DISPLAY_WIDTH;
    lcdStruct.writeStringToBuffer((choose == 0? ">small   big" : " small  >big"));
    LCD_DrawPageFromBuffer(PAGE_3);
}

uint8_t chooseSpeakerType(){    
    lcdStruct.byteIndex = 0x00;
    lcdStruct.writeStringToBuffer("Choose speaker");
    lcdStruct.byteIndex = 2*DISPLAY_WIDTH;
    lcdStruct.writeStringToBuffer(">small   big");
    lcdStruct.displayFullUpdate();
    
    uint8_t curChoose = 0;
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delayUs(10000);
            curChoose -= (curChoose == 0 ? 0 : 1);
            drawChooseMenu(curChoose);
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delayUs(10000);
            curChoose += (curChoose == 1 ? 0 : 1);
            drawChooseMenu(curChoose);
        }
        if(isButtonPressed(BUTTON_SELECT)){
            delayUs(10000);
            lcdStruct.clearOrFillDisplay(CLEAR);
            return curChoose;
        }
        delayUs(100000);
    }
}

void speakerMenu(){
    lcdStruct.clearOrFillDisplay(CLEAR);
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    
    uint8_t curSpeaker = chooseSpeakerType();
    
    
    fillSpeakerMenuBuffer();
    lcdStruct.displayFullUpdate();
    
    while(1){
        if(isButtonPressed(BUTTON_TOP)){
            delayUs(buttonDelayUs);
            currentItem -= (currentItem == 0 ? 0 : 1);
            
            fillSpeakerMenuBuffer();
            lcdStruct.displayFullUpdate();
        }
        
        if(isButtonPressed(BUTTON_BOTOOM)){
            delayUs(buttonDelayUs);
            currentItem += (currentItem >= SPEAKER_MENU_COUNT-1 ? 0 : 1);
            
            fillSpeakerMenuBuffer();
            lcdStruct.displayFullUpdate();
        }
        
        if(isButtonPressed(BUTTON_SELECT)){
            delayUs(500000);
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
