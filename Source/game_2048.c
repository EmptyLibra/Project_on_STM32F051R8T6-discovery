#include "game_2048.h"

//-----------Variables--------------------------------------------
static Matrix gameMatrix = {{0x00}, 4};
typedef enum {
    UP,
    LEFT, 
    RIGHT,
    DOWN,
} Direction;
//static Direction newDirection;
static uint16_t score = 0;

static uint8_t NUMBERS_2048[] = {
    0x00, 0x7C, 0x44, 0x7C,   // 0
    0x00, 0x44, 0x7C, 0x40,   // 1
    0x00, 0x74, 0x54, 0x5C,   // 2
    0x00, 0x44, 0x54, 0x7C,   // 3
    0x00, 0x1C, 0x10, 0x7C,   // 4
    0x00, 0x5C, 0x54, 0x74,   // 5
    0x00, 0x7C, 0x54, 0x74,   // 6
    0x00, 0x04, 0x04, 0x7C,   // 7
    0x00, 0x7C, 0x54, 0x7C,   // 8
    0x00, 0x5C, 0x54, 0x7C    // 9
};

//
//-----------Functions----------------------------------------------------------
void gameInit(){
    //---default values
    int i, j;
    for(i = 0; i < LCD_BUFFER_LENGTH; i++){
        displayBuffer[i] = 0x00;
        if(i < 16){
            gameMatrix.matrix[i] = 0x00;
        }
    }
    gameMatrix.maxNumber = 2;
    score = 0;
    
    //---write frame of field---------------------------------------------------
    // write upper and bottom border
    for(i = 1; i < FIELD_WIDTH - 1; i++){
        for(j = 0; j < 3*DISPLAY_WIDTH; j += DISPLAY_WIDTH){
            displayBuffer[j + i] |= 0x01;
        }
        displayBuffer[DISPLAY_WIDTH*3 + i] |= 0x81;
    }
    
    // write lateral border
    for(i = 0; i < 4*DISPLAY_WIDTH; i += DISPLAY_WIDTH){
        for(j = 0; j < 5*CELL_WIDTH; j += CELL_WIDTH){
            displayBuffer[i+j]  |= 0xFF;
        }
    }
    
    //---generated first numbers---------------------------------------------------
    genNewNumber();
    genNewNumber();

    // draw matrix
    writeMatrixToField();
    writeScore();
    LCD_DisplayFullUpdate();
}

//
//----write functions----
void writeScore(){
    lcdStruct.byteIndex = FIELD_WIDTH + 1;
    LCD_WriteStringToBuffer("Score:");
    
    char receiveString[5];
    sprintf(receiveString, "%05u", score);
    lcdStruct.byteIndex = FIELD_WIDTH+DISPLAY_WIDTH + 1;
    LCD_WriteStringToBuffer(receiveString);
}

void writeNumberToField(uint16_t number, uint8_t row, uint8_t col){
    // in which byte write new number or space byte
    uint16_t index = row*DISPLAY_WIDTH + col*CELL_WIDTH + 1;
    
    // clear cell
    int i, j;
    for(i = 0; i < 16; i++){
        displayBuffer[index + i] &= (row == 3 ? 0x81: 0x01);
    }
    // if element = 0
    if(number == 0) return;
    
    // calculate digits of number
    uint8_t digits[4] = {number/1000, (number/100)%10, (number/10)%10, number%10};
    uint8_t startDigit = 0;
    
    // calculate start space and start digit index
    if(digits[0] == 0){
        if(digits[1] != 0){
            index += 2;
            startDigit = 1;
        } else if(digits[2] != 0){
            startDigit = 2;
            index += 4;
        } else {
            index += 6;
            startDigit = 3;
        }
    }
    
    // write digits of number
    for(i = 0; i < 4 - startDigit; i++){
        for(j = 0; j < 4; j++){
            displayBuffer[index + i*4 + j] |= NUMBERS_2048[digits[i+startDigit]*4+j];
        }
    }
}

void writeMatrixToField(){
    int i;
    for(i = 0; i < 16; i++){
        writeNumberToField(gameMatrix.matrix[i], i/4, i%4);
    }    
}

//
// functions for game mechanics
uint8_t getAvailableIndex(){
    // array with available index
    int availableCells[16] = {0};
    uint8_t availableCellsCount = 0;
    
    // find free index
    int i;
    for(i = 0; i < 16; i++){
        availableCells[i] = (gameMatrix.matrix[i] == 0 ? availableCellsCount++, i : -1);
    }
    
    // if field is full
    if(!availableCellsCount){ return 250; }
    
    uint8_t ind = GAME_TIMER->CNT % 16;
    
    while(availableCells[ind] == -1){
        ind++;
        ind %= 16;
    }

    return ind;
}

void genNewNumber(){
    uint8_t index = getAvailableIndex();
    if(index != 250){
        *(gameMatrix.matrix + index) = (GAME_TIMER->CNT < 900 ? 2 : 4);
    }
}

// shifts the desired col of data or check one col for game end
uint8_t moveColAndEndCheck(uint8_t isGameEndCheck, uint16_t *value1, uint16_t *value2, uint16_t *value3, uint16_t *value4){
    // new values and count of shifts
    uint16_t values[4] = {*value1, *value2, *value3, *value4};
    uint8_t shiftCount = 0;
    
    int index, in_index;
    // move all
    for(index = 1; index < 4; index++){
        if(values[index] == 0) continue;
        else {
            for(in_index = index - 1; in_index >= 0; in_index--){
                // move value if there is free space
                if(values[in_index] == 0){
                    values[in_index] = values[in_index + 1];
                    values[in_index + 1] = 0;                   
                    shiftCount++;
                } else{
                    break;
                }
            }
        }
    }
    
    // change all
    for(index = 1; index < 4; index++){
        if(values[index] == 0) continue;
        // add values if they are the same
        if((index !=0) && (values[index - 1] == values[index])){
            values[index - 1] *= 2;
            values[index] = 0;
            shiftCount++;
            //update score and max number
            if(isGameEndCheck == 0){
                score += (isGameEndCheck == 1 ? 0 : values[index - 1]);
                gameMatrix.maxNumber = (values[index - 1] > gameMatrix.maxNumber ? values[index - 1] :  gameMatrix.maxNumber);
            }
        }
    }
    
    // move all again
    for(index = 1; index < 4; index++){
        if(values[index] == 0) continue;
        else {
            for(in_index = index - 1; in_index >= 0; in_index--){
                // move value if there is free space
                if(values[in_index] == 0){
                    values[in_index] = values[in_index + 1];
                    values[in_index + 1] = 0;
                    shiftCount++;
                } else{
                    break;
                }
            }
        }
    }
    
    // if nothing has changed
    if(shiftCount == 0){
        return 0;
    } else if(isGameEndCheck == 1){
        return 1;
    } else {
        *value1 = values[0];
        *value2 = values[1];
        *value3 = values[2];
        *value4 = values[3];
        return 1;
    }
}
uint8_t checkGameEnd(){
    if(gameMatrix.maxNumber == WIN_NUMBER) return GAME_WIN;
    
    uint8_t check = 0;
    
    uint8_t i;
    // check all colls on the possibility of a shift
    for(i = 0; i < 4; i++){
        check += moveColAndEndCheck(1, &gameMatrix.matrix[i], &gameMatrix.matrix[4+i], 
                                 &gameMatrix.matrix[8+i], &gameMatrix.matrix[12+i]);
        
        check += moveColAndEndCheck(1, &gameMatrix.matrix[12+i], &gameMatrix.matrix[8+i], 
                                 &gameMatrix.matrix[4+i], &gameMatrix.matrix[i]);
        
        check += moveColAndEndCheck(1, &gameMatrix.matrix[i*4+3], &gameMatrix.matrix[i*4+2], 
                                 &gameMatrix.matrix[i*4+1], &gameMatrix.matrix[i*4]);
        
        check += moveColAndEndCheck(1, &gameMatrix.matrix[i*4], &gameMatrix.matrix[i*4+1], 
                                 &gameMatrix.matrix[i*4+2], &gameMatrix.matrix[i*4+3]);
    }
    return (check == 0 ? GAME_LOSE : GAME_CONTINUE);
}
void moveAndDrawMatrix(uint8_t newDirection){
    uint8_t shiftCounts = 0;
    int i;
    switch(newDirection){
        case UP:
            for(i = 0; i < 4; i++){
                shiftCounts += moveColAndEndCheck(0, &gameMatrix.matrix[i], &gameMatrix.matrix[4+i], 
                                 &gameMatrix.matrix[8+i], &gameMatrix.matrix[12+i]);
            }
            break;
        case DOWN:
            for(i = 0; i < 4; i++){
                shiftCounts += moveColAndEndCheck(0, &gameMatrix.matrix[12+i], &gameMatrix.matrix[8+i], 
                                 &gameMatrix.matrix[4+i], &gameMatrix.matrix[i]);
            }
            break;
        case RIGHT:
            for(i = 0; i < 4; i++){
                shiftCounts += moveColAndEndCheck(0, &gameMatrix.matrix[i*4+3], &gameMatrix.matrix[i*4+2], 
                                 &gameMatrix.matrix[i*4+1], &gameMatrix.matrix[i*4]);
            }
            break;
        case LEFT:
            for(i = 0; i < 4; i++){
                shiftCounts += moveColAndEndCheck(0, &gameMatrix.matrix[i*4], &gameMatrix.matrix[i*4+1], 
                                 &gameMatrix.matrix[i*4+2], &gameMatrix.matrix[i*4+3]);
            }
            break;
    }
    
    writeMatrixToField();
    writeScore();    
    LCD_DisplayFullUpdate();
    delayUs(10000);
    
    // generate new number if there were shifts
    if(shiftCounts != 0){
        genNewNumber();
        writeMatrixToField();
        LCD_DisplayFullUpdate();
    }
}

void startGame2048(){
    LCD_ClearOrFillDisplay(CLEAR);
    gameInit();
    
    delayUs(1000);
    uint8_t gameStatus = GAME_CONTINUE;
    while(1){
        if(isButtonPressed(BUTTON_TOP)){
            moveAndDrawMatrix(UP);
        }
        if(isButtonPressed(BUTTON_BOTOOM)){
            moveAndDrawMatrix(DOWN);
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            moveAndDrawMatrix(RIGHT);
        }
        if(isButtonPressed(BUTTON_LEFT)){
            moveAndDrawMatrix(LEFT);
        }
        
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                break;
            }
        #endif
        
        if(gameStatus == GAME_CONTINUE){
            gameStatus = checkGameEnd();
        } else{
            lcdStruct.byteIndex = DISPLAY_WIDTH*5 + 20;
            LCD_WriteStringToBuffer((gameStatus == GAME_WIN ? "You WIN!!!" : "You LOSE!!!" ));
            LCD_DisplayFullUpdate();
            delayUs(2000000);
            break;
        }
        delayUs(200000);
    }
}
