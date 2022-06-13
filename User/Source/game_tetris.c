#include "game_tetris.h"

//=================Variables====================================================
static int curFigX = 0, curFigY = 8; // coord of up right corner of figure
static uint16_t curFigType = 0, nextFigType = 0, score = 0, winScore = 60000,lines = 0;
static uint8_t curFig[8] = {0x00};
static uint8_t gameState = GAME_CONTINUE, fisrtStart = 1, isTetrisSong = 0;
static long curDelay = 100000;

//------list of figures----------
/* Cube - Smashboy
 *  {0,0, 0,0, 0,0, 0,0} <- young pixel
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}  
 
 * T-shaped - Teewee
 *  {0,0, 0,0, 1,1, 0,0} <- young pixel
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 
 J-shaped - Orenge Ricky
 *  {0,0, 0,0, 1,1, 1,1} <- young pixel
 *  {0,0, 0,0, 1,1, 1,1}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0} 
 
 L-shaped - Blue Ricky
 *  {0,0, 1,1, 1,1, 0,0} <- young pixel
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 
 Z-shaped - Cleveland Z
 *  {0,0, 0,0, 1,1, 0,0} <- young pixel
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 
 S-shaped - Rhode island Z
 *  {0,0, 1,1, 0,0, 0,0} <- young pixel
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 1,1, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 1,1, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 *  {0,0, 0,0, 0,0, 0,0}
 
 I-shaped - Hero
 *  {0,0, 1,1, 0,0, 0,0} <- young pixel
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 *  {0,0, 1,1, 0,0, 0,0}
 */
static uint8_t FIGURES[] = { 
    0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00, // Cube - Smashboy
    0x0C, 0x0C, 0x3C, 0x3C, 0x0C, 0x0C, 0x00, 0x00, // T-shaped - Teewee
    0x0F, 0x0F, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, // J-shaped - Orenge Ricky
    0x3C, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x00, 0x00, // L-shaped - Blue Ricky
    0x0C, 0x0C, 0x3C, 0x3C, 0x30, 0x30, 0x00, 0x00, // Z-shaped - Cleveland Z
    0x30, 0x30, 0x3C, 0x3C, 0x0C, 0x0C, 0x00, 0x00, // S-shaped - Rhode island Z
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // I-shaped - Hero
};

//
//=================Functions====================================================
void gameInit(){
    // set default data
    gameState = GAME_CONTINUE;
    curFigX = 0; curFigY = 8;
    score = 0; lines = 0;
    curDelay = 100000;
    fisrtStart = 1;
    memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
    
    // generated current and next figure
    genNextFigType();
    
    // write current figure to field
    writeFigToField();
    
    // write border line
    uint16_t index;
    for(index = 0; index < DISPLAY_WIDTH; index++){
        displayBuffer[(TETRIS_FIELD_WIDTH/8)*DISPLAY_WIDTH + index] |= (0x01 << TETRIS_FIELD_WIDTH%8);
    }
    
    updateScoreAndFigure();
    lcdStruct.displayFullUpdate();
}
//
//---other functions----------------------
uint8_t tgm3Randomizer(){
    // create 35 pool
    uint8_t pool[35] = {
        6, 6, 2, 6, 3, 1, 1, 
        6, 2, 5, 0, 3, 4, 0, 
        1, 0, 4, 2, 6, 5, 5, 
        0, 2, 3, 3, 0, 4, 5, 
        3, 1, 4, 1, 5, 4, 2
    };
    
    // special first piece
    uint8_t firstPiece = GAME_TIMER->CNT % 4;
    static uint8_t firstGen = 1;
    if(firstGen){
        firstGen = 0;
        return firstPiece;
    }
    
    static uint8_t history[4] = {5, 4, 5, 0};
    
    uint8_t roll, i, piece;
    while(1){
        for(roll = 0; roll <6; roll++){
            i = GAME_TIMER->CNT % 35;
            piece = pool[i];            
            
            uint8_t isExist = 0, j;
            for(j = 0; j < 4; j++){
                if(history[j] == piece){
                    isExist = 1;
                    break;
                }
            }
            
            if(isExist == 0 || roll == 5){
                break;
            }
        }
        
        // shift history and add new piece
        for(i = 0; i < 3; i++){
            history[i] = history[i+1];
        }
        history[3] = piece;
        
        return piece;
    }
}

void genNextFigType(){
    curFigX = 0; curFigY = 8;
    if(fisrtStart){
        fisrtStart = 0;
        nextFigType = tgm3Randomizer();
    }
    
    curFigType = nextFigType;
    nextFigType = tgm3Randomizer();
    
    uint8_t i;
    for(i = 0; i < 8; i++){
        curFig[i] = FIGURES[curFigType*8 + i];
    }
}

void writeFigToField(){
    uint8_t col, row = curFigY+1;
    
    // if empty string of figure is behind the screen right
    if(curFigY < 0){
        for(col = 0; col < 8; col++){
            displayBuffer[curFigX+col] |= (curFig[col] >> -curFigY);
        }
        return;
    }
    
    // the same, but left
    if(curFigY > DISPLAY_HEIGHT-8){
        for(col = 0; col < 8; col++){
            displayBuffer[DISPLAY_HEIGHT/8*DISPLAY_WIDTH + curFigX+col] |= (curFig[col] << curFigY%8);
        }
        return;
    }
    
    // separate figure into to parts (on first page and on second page)
    // count, how many lines one one page    
    while(curFigY/8 == row/8){row++;}
    
    // if all on one page
    if(row == curFigY+8){
        for(col = 0; col < 8; col++){
            displayBuffer[(curFigY >> 3)*DISPLAY_WIDTH + curFigX+col] |= curFig[col];
        }
        return;
    }
    
    // write to first page
    for(col = 0; col < 8; col++){
        displayBuffer[(curFigY >> 3)*DISPLAY_WIDTH + curFigX+col] |= (curFig[col] << (8-(row - curFigY)));
    }
    
    // write to second page
    for(col = 0; col < 8; col++){
        displayBuffer[(row >> 3)*DISPLAY_WIDTH + curFigX+col] |= (curFig[col] >> (row - curFigY));
    }
}

uint8_t isLost(){
    uint8_t row;
    for(row = 0 ; row < TETRIS_FIELD_WIDTH; row++){
        if(displayBuffer[(row/8)*DISPLAY_WIDTH] & (0x01 << (row%8))) return 1;
    }
    return 0;
}
void shiftAllDown(uint16_t lineNumber){
    int col, newcol = lineNumber-1;
    for(col = lineNumber; col > 0; newcol--, col--){
        // miss empty lines
        while(displayBuffer[newcol] == 0x00 && displayBuffer[DISPLAY_WIDTH + newcol] == 0x00 && 
            (displayBuffer[2*DISPLAY_WIDTH + newcol]&0x0F) == 0x00){
                newcol--;
                if(newcol <= 0) break;
        }
        
        if(newcol <= 0) break;
        
        displayBuffer[col] = displayBuffer[newcol];
        displayBuffer[DISPLAY_WIDTH + col] = displayBuffer[DISPLAY_WIDTH + newcol];
        displayBuffer[2*DISPLAY_WIDTH + col] |= displayBuffer[2*DISPLAY_WIDTH + newcol] & 0x0F;
        
        displayBuffer[newcol] = 0x00;
        displayBuffer[DISPLAY_WIDTH + newcol] = 0x00;
        displayBuffer[2*DISPLAY_WIDTH + newcol] &= 0xF0;
    }
    
    displayBuffer[col] = 0x00;
    displayBuffer[DISPLAY_WIDTH + col] = 0x00;
    displayBuffer[2*DISPLAY_WIDTH + col] = displayBuffer[2*DISPLAY_WIDTH + col] & 0xF0;
}

void checkAndShiftLines(){

    uint16_t col, emptyLineCount = 0, firstLine = 0;
    // delete full lines
    for(col = DISPLAY_WIDTH-1; col > 0; col--){
        // if the line is full
        if(displayBuffer[col] == 0xFF && displayBuffer[DISPLAY_WIDTH + col] == 0xFF &&
            (displayBuffer[2*DISPLAY_WIDTH + col]&0x0F) == 0x0F){
            // delete line
            displayBuffer[col] = 0x00;
            displayBuffer[DISPLAY_WIDTH + col] = 0x00;
            displayBuffer[2*DISPLAY_WIDTH + col] &= 0xF0;
            
            displayBuffer[col-1] = 0x00;
            displayBuffer[DISPLAY_WIDTH + col-1] = 0x00;
            displayBuffer[2*DISPLAY_WIDTH + col-1] &= 0xF0;
            
            if(!firstLine) firstLine = col;
            emptyLineCount += 2;
            col--;
        }
    }
    
    if(emptyLineCount){
        shiftAllDown(firstLine);
        
        // add score
        switch(emptyLineCount){
            case 2:
                score += 100;
                break;
            case 4:
                score += 300;
                break;
            case 6:
                score += 700;
                break;
            case 8:
                score += 1500;
                break;
            default: ;
        }
        lines += emptyLineCount/2;
    }
}

void updateScoreAndFigure(){
    lcdStruct.byteIndex = DISPLAY_WIDTH*7;
    LCD_writeHorStringToBuffer(" Next");
    
    // draw next figure
    uint8_t i;
    for(i = 0; i < 8; i++){
        displayBuffer[DISPLAY_WIDTH*5 + 9 + i] = FIGURES[nextFigType*8 + i];
    }
    
    // draw score
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+24;
    LCD_writeHorStringToBuffer("Score:");
    
    char receiveString[5];
    sprintf(receiveString, "%05u", score);
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+32 ;
    LCD_writeHorStringToBuffer(receiveString);
    
    // draw lines
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+48;
    LCD_writeHorStringToBuffer("Lines:");
    
    sprintf(receiveString, "%05u", lines);
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+56;
    LCD_writeHorStringToBuffer(receiveString);
}
//
//---move functions----------------
void rotate(){
    // if it is a cube
    if(curFigType == 0) return;
    
    // clear old figure
    int x, y;
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    uint8_t is3x3 = 1, buffer[8] = {0x00};
    
    // figure 3x3 or not
    for(x = 0; x < 8; x++){
        if(curFig[x]&0xC0){
            is3x3 = 0;
            break;
        }
    }
    
    if(curFig[7] != 0 || curFig[6] != 0){
        is3x3 = 0;
    }
    
    // write rotated fig to a buffer
    uint8_t maxRow = (is3x3 ? 6 : 8);
    for(y = 0; y < maxRow; y++){
        for(x = 0; x < maxRow; x++){
            buffer[x] |= (((curFig[y] >> (maxRow-1-x)) & 0x01) << y);
        }
    }
    
    //---change y coordinate if we near the border----------
    int temp = curFigY;
    
    // change y coord if we near the left border
    if((!is3x3 && curFigY > 12) || (is3x3 && curFigY > 14)){
        for(y = 7; y > 3; y-=2){
            for(x = 0; x < 8; x++){
                if((buffer[x] >> y) & 0x01){
                    curFigY -= 2;
                    break;
                }
            }
            if(curFigY < 12) break;
        }
    }
    
    //change y coord if we near the rigth border
    if(curFigY < 0){
        for(y = 0; y < 8; y+=2){
            for(x = 0; x < 8; x++){
                if((buffer[x] >> y) & 0x01){
                    curFigY += 2;
                    break;
                }
            }
            if(curFigY >= 0) break;
        }
    }
    
    //---check rotate-----------------------------------------
    // can rotate?
    for(y = 0; y < 8; y++){
        for(x = 0; x < 8; x++){
            if(((buffer[x] >> y) & 0x01) && 
                ((displayBuffer[(curFigY+y)/8*DISPLAY_WIDTH + curFigX+x] >> (curFigY+y)%8) & 0x01)){
                    curFigY = temp;
                    return;
                }
                
        }
    }

    // check corner
    for(y = 0; y < 8; y++){
        for(x = 0; x < 8; x++){
            if(((buffer[x] >> y) & 0x01) && 
                (curFigY+y > TETRIS_FIELD_WIDTH-1 || curFigX+x > DISPLAY_WIDTH-1)){
                    curFigY = temp;
                    return;
                }
        }
    }
    
    //---write figure from buffer---------------------
    for(x = 0; x < 8; x++){
        curFig[x] = buffer[x];
    }
}

void shiftLeft(){
    int x, y, emptylines = 0;
    uint8_t empty = 1, possible = 1;
    
    // clear old figure
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    // counting the number of empty lines
    for(y = 7; y >= 0; y--){
        for(x = 0; x < 8; x++){
            if((curFig[x] >> y) & 0x01){
                empty = 0;
                break;
            }
        }
        if(!empty) break;
        emptylines++;
    }
    
    // check if shifting possible
    if(curFigY-emptylines+8 != TETRIS_FIELD_WIDTH){
        for(y = 7; y >= 0; y--){
            for(x = curFigX+7; x >= curFigX; x--){
                if((curFig[x - curFigX] & (0x01 << y)) && 
                        (displayBuffer[((curFigY+y+1)/8)*DISPLAY_WIDTH + x] & (0x01 << (curFigY+y+1)%8))){
                    possible = 0;
                    break;
                }
            }
            if(!possible) break;
        }
        
        if(possible){
            curFigY+=2;
        }
    }
}
void shiftRight(){
    int x, y, emptylines = 0;
    uint8_t empty = 1, possible = 1;
    
    // clear old figure
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    // counting the number of empty lines
    for(y = 0; y < 8; y++){
        for(x = 0; x < 8; x++){
            if((curFig[x] >> y) & 0x01){
                empty = 0;
                break;
            }
        }
        if(!empty) break;
        emptylines++;
    }
    
    // check if shifting possible
    if(curFigY+emptylines != 0){
        for(y = 0; y < 8; y++){
            for(x = curFigX+7; x >= curFigX; x--){
                if((curFig[x - curFigX] & (0x01 << y)) && 
                        (displayBuffer[((curFigY+y-1)/8)*DISPLAY_WIDTH + x] & (0x01 << (curFigY+y-1)%8))){
                    possible = 0;
                    break;
                }
            }
            if(!possible) break;
        }
        
        if(possible){
            curFigY-=2;
        }
    }
}

uint8_t shiftDown(){
    uint8_t empty = 1, possible = 1, emptylines = 0;
    int x, y;
    
    // counting the number of empty lines
    for(x = 7; x >= 0; x--){
        if(curFig[x]){
            empty = 0;
            break;
        }
        if(!empty) break;
        emptylines++;
    }    
    
    // clear old figure
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    // can we shift figure down?
    for(y = 0; y < 8; y++){
        for(x = curFigX+7; x > curFigX; x--){
            if((curFig[x - curFigX] & (0x01 << y)) && (displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x+1] & (1 << (curFigY+y)%8))){
                possible = 0;
                break;
            }
        }
    }
    
    if(curFigX < DISPLAY_WIDTH-8+emptylines && possible){
        curFigX++;
    } else {
        return 0;
    }
    
    return 1;
}

void drawSongMenuTetris(){
    lcdStruct.byteIndex = 30;
    lcdStruct.writeStringToBuffer("Play music?");
    lcdStruct.byteIndex = DISPLAY_WIDTH*2 + 30;
    switch(isTetrisSong){
        case 0:
            lcdStruct.writeStringToBuffer(">no  yes");
            break;
        case 1:
            lcdStruct.writeStringToBuffer(" no >yes");
            break;
    }
    lcdStruct.displayFullUpdate();
}
void songSelectTetris(){
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    isTetrisSong = 0;
    drawSongMenuTetris();
    
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delayUs(10000);
            isTetrisSong -= (isTetrisSong == 0 ? 0 : 1);
            drawSongMenuTetris();
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delayUs(10000);
            isTetrisSong += (isTetrisSong == 1 ? 0 : 1);
            drawSongMenuTetris();
        }
        if(isButtonPressed(BUTTON_SELECT)){
            delayUs(10000);
            lcdStruct.clearOrFillDisplay(CLEAR);
            return;
        }
        delayUs(100000);
    }
}

//
//---Main function----------------
void startTetrisGame(){
    lcdStruct.clearOrFillDisplay(CLEAR);
    
    songSelectTetris();
    gameInit();
    
    if(isTetrisSong){
        playBackgroundSong(SPEAKER_TYPE_BIG, TetrisGameSong, TetrisGameSong_Beats, sizeof(TetrisGameSong)/2, 140,1);
    }
    
    uint8_t isShiftDown = 0, isShiftOrRotate = 0;
    while(1){
        // update delayUs
        curDelay = 100000 - score*10;
        
        if(curDelay < 0) curDelay = 0;
        
        if(isButtonPressed(BUTTON_TOP)){
            shiftRight();
            isShiftOrRotate = 1;
            if(curDelay <= 50000){
                delayUs(90000);
            }
        }
        
        if(isButtonPressed(BUTTON_BOTOOM)){
            shiftLeft();
            isShiftOrRotate = 1;
            if(curDelay <= 50000){
                delayUs(90000);
            }
        }
        
        if(isButtonPressed(BUTTON_LEFT)){
            rotate();
            isShiftOrRotate = 1;
            if(curDelay <= 50000){
                delayUs(90000);
            }
        }
        
        // update display if fig rotate or shift
        if(isShiftOrRotate){
            writeFigToField();
        
            LCD_DrawPageFromBuffer(PAGE_2);
            if(curFigY <= 8){
                LCD_DrawPageFromBuffer(PAGE_1);
            } else {
                LCD_DrawPageFromBuffer(PAGE_3);
            }
            
            if(curFigType == 6 && curFigY == 8){
                LCD_DrawPageFromBuffer(PAGE_3);
            }
            isShiftOrRotate = 0;
        }
        
        // normal or fast move down
        do{
            // shift figure down
            isShiftDown = shiftDown();
            writeFigToField();
            
            if(!isShiftDown){
                // check game end
                if(isLost()){
                    gameState = GAME_LOSE;
                    break;
                }
                
                // check lines
                checkAndShiftLines();
                
                // generate and write new figure
                genNextFigType();
                writeFigToField();
                
                updateScoreAndFigure();
                lcdStruct.displayFullUpdate();
            }
            
            // draw player field
            LCD_DrawPageFromBuffer(PAGE_2);
            if(curFigY <= 8){
                LCD_DrawPageFromBuffer(PAGE_1);
            } else{
                LCD_DrawPageFromBuffer(PAGE_3);
            }
        } while(isButtonPressed(BUTTON_RIGHT) && gameState != GAME_LOSE);
        
        // exit the game
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                musicSet.isCyclickSong = 0;
                musicSet.noteNumber = song.noteCount;
                
                lcdStruct.clearOrFillDisplay(CLEAR);
                break;
            }
        #endif
        
        // pause
        #ifdef BUTTON_SELECT
            if(isButtonPressed(BUTTON_SELECT)){
                lcdStruct.byteIndex = DISPLAY_WIDTH*8-9;
                LCD_writeHorStringToBuffer("Pause");
                lcdStruct.displayFullUpdate();
                musicSet.isCyclickSong = 0;
                musicSet.noteNumber = song.noteCount;
                delayUs(1000000);
                
                while(!isButtonPressed(BUTTON_SELECT)){}
                if(isTetrisSong){
                    playBackgroundSong(SPEAKER_TYPE_BIG, TetrisGameSong, TetrisGameSong_Beats, sizeof(TetrisGameSong)/2, 140,1);
                }
                lcdStruct.byteIndex = DISPLAY_WIDTH*8-9;
                LCD_writeHorStringToBuffer("     ");
                lcdStruct.displayFullUpdate();
                delayUs(500000);
            }
        #endif
            
        //-----game end---------
        if(score >= winScore){ gameState = GAME_WIN;}
        
        if(gameState != GAME_CONTINUE){
            musicSet.isCyclickSong = 0;
            musicSet.noteNumber = song.noteCount;
            
            lcdStruct.byteIndex = DISPLAY_WIDTH*7+72;
            LCD_writeHorStringToBuffer((gameState == GAME_WIN ? "You WIN" : "You Lose"));
            lcdStruct.displayFullUpdate();
            
            delayUs(2000000);
            lcdStruct.clearOrFillDisplay(CLEAR);
            return;
        }
            
        delayUs(curDelay);
    }
}
