#include "snake.h"
//###### error when snake near itself. No differrence between snake px and board px
//-----variables-------------------------------------------------------------------------------------

static Snake snake, defaultSnake = {{START_SNAKE_ROW, START_SNAKE_COL + START_SNAKE_LENGTH-1, RIGHT}, 
                                    {START_SNAKE_ROW, START_SNAKE_COL, RIGHT}, RIGHT, START_SNAKE_LENGTH, 0};
static Food food, defaultFood = {0,0,0};
static uint16_t score = 0, winScore = DISPLAY_HEIGHT*DISPLAY_WIDTH - START_SNAKE_LENGTH - 10;
static uint8_t isSnakeStop = SNAKE_NOT_STOP, gameMode = 0, isSnakeSong;


//--init function-------------------------------
void snakeInit(){
    snake = defaultSnake;
    
    // write start snake body to player field
    int i;
    for(i = 0; i < START_SNAKE_LENGTH; i++){
        displayBuffer[START_SNAKE_ROW/8*DISPLAY_WIDTH + i+START_SNAKE_COL] |= (0x01 << (START_SNAKE_ROW % 8));
    }
    // write tail to player field
    displayBuffer[(START_SNAKE_ROW+1)/8*DISPLAY_WIDTH + START_SNAKE_COL-1] |= (0x01 << ((START_SNAKE_ROW+1) % 8));
    displayBuffer[(START_SNAKE_ROW-1)/8*DISPLAY_WIDTH + START_SNAKE_COL-1] |= (0x01 << ((START_SNAKE_ROW-1) % 8));
    
    // write snake head
    displayBuffer[START_SNAKE_ROW/8*DISPLAY_WIDTH + START_SNAKE_LENGTH-2 +START_SNAKE_COL]   |= (0x07 << ((START_SNAKE_ROW-1) % 8));
    displayBuffer[START_SNAKE_ROW/8*DISPLAY_WIDTH + START_SNAKE_LENGTH-2 +START_SNAKE_COL+1] |= (0x07 << ((START_SNAKE_ROW-1) % 8));
}
void gameInit(){
    // default values
    lcdStruct.byteIndex = 0;
    score = 0;
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    isSnakeStop = SNAKE_NOT_STOP;
    food = defaultFood;
    winScore = DISPLAY_WIDTH*DISPLAY_HEIGHT- 500;
    
    int i;
    if(gameMode != GAME_EASY_MODE){
        //write frame
        for(i = 1; i< DISPLAY_WIDTH; i++){
            displayBuffer[i] |= 0x01;
            displayBuffer[(DISPLAY_HEIGHT/8-1)*DISPLAY_WIDTH + i] |= 0x80;
        }
        for(i = 0; i < LCD_BUFFER_LENGTH;){
            displayBuffer[i] |= 0xFF;
            i += (i%DISPLAY_WIDTH == 0 ? DISPLAY_WIDTH-1 : 1 );
        }
    }
    
    // write medium border
    if(gameMode == GAME_MID_MODE){
        winScore = DISPLAY_WIDTH*DISPLAY_HEIGHT- 1500;
        uint8_t maxLen = (DISPLAY_HEIGHT >=64 ? 50 : 40);
        for(i = 10; i< maxLen; i++){
            displayBuffer[(DISPLAY_HEIGHT/16-1)*DISPLAY_WIDTH + i] |= 0x80;
            displayBuffer[(DISPLAY_HEIGHT/16-1)*DISPLAY_WIDTH + DISPLAY_WIDTH/2 + i] |= 0x80;
        }
        maxLen = (DISPLAY_HEIGHT >=64 ? 20 : 10);
        i = (DISPLAY_HEIGHT >=64 ? 10 : 8);
        for(; i < maxLen ; i++){
            displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2] |= (1 << (i%8));
            displayBuffer[((i+3+maxLen)/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2] |= (1 << ((i+3+maxLen)% 8));
        }
    }
    
    if(gameMode == GAME_HARD_MODE){
        winScore = 2000;
        // horizontal line with holes
        for(i = 0; i< DISPLAY_WIDTH; i++){
            if((i > 10 && i < 15) || (i > 100 && i < 105) || (i > DISPLAY_WIDTH/2-3 && i < DISPLAY_WIDTH/2+3)) continue;
            displayBuffer[(DISPLAY_HEIGHT/16)*DISPLAY_WIDTH + i] |= (1 << (DISPLAY_HEIGHT/16));
        }
        // vertical line 1 with holes
        for(i = 0; i< DISPLAY_HEIGHT; i++){
            if((i > 5 && i < 10) || (i > DISPLAY_HEIGHT-15 && i < DISPLAY_HEIGHT-10)) continue;
            displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/4] |= (1 << (i%8));
        }
        // vertical line 2 with holes
        for(i = 0; i< DISPLAY_HEIGHT; i++){
            if((i > 10 && i < 15) || (i > DISPLAY_HEIGHT-15 && i < DISPLAY_HEIGHT-10)) continue;
            displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2+DISPLAY_WIDTH/4] |= (1 << (i%8));
        }
        // horizontal tunnel
        for(i = DISPLAY_WIDTH/4-1; i< DISPLAY_WIDTH/2+DISPLAY_WIDTH/4+2; i++){
            if(DISPLAY_HEIGHT >= 64){
                displayBuffer[((DISPLAY_HEIGHT-15)/8)*DISPLAY_WIDTH+i] |= (1 << ((DISPLAY_HEIGHT-15)%8));
            }
            if(i > DISPLAY_WIDTH/2+3 && i < DISPLAY_WIDTH/2+8)continue;
            displayBuffer[((DISPLAY_HEIGHT-10)/8)*DISPLAY_WIDTH+i] |= (1 << ((DISPLAY_HEIGHT-10)%8));
            
        }
        if(DISPLAY_HEIGHT >= 64){
            // mini horizontal tunnel
            for(i = 10; i< 18; i++){
                displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-15] |= (1 << (i%8));
                if(i >=15){
                    displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-10] |= (1 << (i%8));
                }
            }
            for(i = DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-15; i< DISPLAY_WIDTH/2+DISPLAY_WIDTH/4+2; i++){
                displayBuffer[(10/8)*DISPLAY_WIDTH+i] |= (1 << (10%8));
                if(i > DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-10){
                    displayBuffer[(15/8)*DISPLAY_WIDTH+i] |= (1 << (15%8));
                }
            }
        }
    }
    
    // write start snake
    snakeInit();
    
    // generated food in random state
    genFood();
    
    // draw field
    lcdStruct.displayFullUpdate();
}

//
//------special functions-------------------------------------------------------------
void genFood(){
    // food row and col generated with timer
    food.isBig = (GAME_TIMER->CNT < 800 ? 0 : 1);
    food.row = (2 +(GAME_TIMER->CNT % (DISPLAY_HEIGHT - 2))) % (DISPLAY_HEIGHT - 2);
    food.col = (2 +(GAME_TIMER->CNT % (DISPLAY_WIDTH - 2))) % (DISPLAY_WIDTH - 2);
    
    // move col and row while not find empty pixel
    uint16_t stopCount = 0;
    // for small food
    if(!food.isBig){
        while((((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] >> (food.row % 8)) & 0x01) == 1) ||
            ((displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] >> ((food.row-1) % 8)) & 0x01) == 1 ||
            ((displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] >> ((food.row+1) % 8)) & 0x01) == 1 ||
            ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1] >> (food.row % 8)) & 0x01) == 1 ||
            ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1] >> (food.row % 8)) & 0x01) == 1 ||
            (abs(food.row - snake.head.row) < 2) || (abs(food.col - snake.head.col) < 2) ||
             food.row < 3 || food.row > DISPLAY_HEIGHT-3 || food.col < 3 || food.col > DISPLAY_WIDTH-3){
            food.col++;
            if(food.col == DISPLAY_WIDTH){
                food.col = 0;
                food.row++;
                food.row %= DISPLAY_HEIGHT;
            }
            stopCount++;
            if(stopCount > DISPLAY_WIDTH*DISPLAY_HEIGHT) return;
        }
        
        // draw new food to buffer
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] |= (1 << (food.row % 8));
    } else{
        // for big food
        while(((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] >> (food.row % 8)) & 0x01) == 1 || 
              ((displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] >> ((food.row-1) % 8)) & 0x01) == 1 ||
              ((displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] >> ((food.row+1) % 8)) & 0x01) == 1 ||
              ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1] >> (food.row % 8)) & 0x01) == 1 ||
              ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1] >> (food.row % 8)) & 0x01) == 1 ||
        
              ((displayBuffer[(food.row-2)/8*DISPLAY_WIDTH + food.col] >> ((food.row-2) % 8)) & 0x01) == 1 ||
              ((displayBuffer[(food.row+2)/8*DISPLAY_WIDTH + food.col] >> ((food.row+2) % 8)) & 0x01) == 1 ||
              ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+2] >> (food.row % 8)) & 0x01) == 1 ||
              ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-2] >> (food.row % 8)) & 0x01) == 1 || 
        
              (abs(food.row - snake.head.row) < 2) || (abs(food.col - snake.head.col) < 2) ||
              (abs(food.row - snake.tail.row) < 2) || (abs(food.col - snake.tail.col) < 2) ||
               food.row < 3 || food.row > DISPLAY_HEIGHT-3 || food.col < 3 || food.col > DISPLAY_WIDTH-3){
            food.col++;
            if(food.col == DISPLAY_WIDTH){
                food.col = 0;
                food.row++;
                food.row %= DISPLAY_HEIGHT;
            }
            stopCount++;
            if(stopCount > DISPLAY_WIDTH*DISPLAY_HEIGHT) return;
        }
        
        // draw new food to buffer
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] |= (1 << (food.row % 8));
        displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] |= (1 << ((food.row-1) % 8));
        displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] |= (1 << ((food.row+1) % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1] |= (1 << (food.row % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1] |= (1 << (food.row % 8));
    }
}
Direction getNewTailDir(){   
    if(((displayBuffer[snake.tail.row/8*DISPLAY_WIDTH + snake.tail.col + 1] >> (snake.tail.row)%8) & 0x01) == 1){
        return RIGHT;
    }
    if(((displayBuffer[snake.tail.row/8*DISPLAY_WIDTH + snake.tail.col - 1] >> (snake.tail.row)%8) & 0x01) == 1){
        return LEFT;
    }
    if(((displayBuffer[(snake.tail.row-1)/8*DISPLAY_WIDTH + snake.tail.col] >> (snake.tail.row-1)%8) & 0x01) == 1){
        return UP;
    }
    if(((displayBuffer[(snake.tail.row+1)/8*DISPLAY_WIDTH + snake.tail.col] >> (snake.tail.row+1)%8) & 0x01) == 1){
        return DOWN;
    }
    
    if(snake.tail.row == 0) return UP;
    if((uint8_t)snake.tail.row == DISPLAY_HEIGHT-1) return DOWN;
    if((uint8_t)snake.tail.col == DISPLAY_WIDTH-1) return RIGHT;
    if(snake.tail.col == 0) return LEFT;
    
    return RIGHT;
}

uint8_t getNextHeadPx(){
    // if it is a small food px
    int8_t tempCol = snake.head.col, tempRow = snake.head.row;
    switch(snake.head.dir){
        case RIGHT:
            tempCol = ((uint8_t) snake.head.col ==  DISPLAY_WIDTH  ? 0 : snake.head.col);
            break;
        case LEFT:
            tempCol = (snake.head.col == -1 ? DISPLAY_WIDTH-1  : snake.head.col);
            break;
        case UP:
            tempRow = (snake.head.row <= -1 ? DISPLAY_HEIGHT-1 : snake.head.row);
            break;
        case DOWN:
            tempRow = ((uint8_t) snake.head.row >=  DISPLAY_HEIGHT ? 0 : snake.head.row);
            break;
    }
    if(!food.isBig){
        switch(snake.head.dir){
            case RIGHT:
            case LEFT:
                if( (tempCol == food.col && tempRow == food.row) || 
                    (tempCol == food.col && tempRow-1 == food.row) ||
                    (tempCol == food.col && tempRow+1 == food.row)){
                    snake.growCount = 1;
                    return FOOD_PX;
                }
                break;
            case UP:
            case DOWN:
                if( (tempCol == food.col && tempRow == food.row) || 
                    (tempCol-1 == food.col && tempRow == food.row) ||
                    (tempCol+1 == food.col && tempRow == food.row)){
                    snake.growCount = 1;
                    return FOOD_PX;
                }
        }
    
    // if it is a big food pixel
    }else{
        switch(snake.head.dir){
            case RIGHT:
            case LEFT:
                if((abs(tempCol - food.col) <= 1 && abs(tempRow - food.row) <= 1) || 
                   (abs(tempCol - food.col) <= 1 && abs(tempRow-1 - food.row) <= 1) ||
                   (abs(tempCol - food.col) <= 1 && abs(tempRow+1 - food.row) <= 1)){
                    snake.growCount = 3;
                    return FOOD_PX;
                }
                break;
            case UP:
            case DOWN:
                if((abs(tempCol - food.col) <= 1 && abs(tempRow - food.row) <= 1) || 
                   (abs(tempCol - food.col+1) <= 1 && abs(tempRow - food.row) <= 1) ||
                   (abs(tempCol - food.col-1) <= 1 && abs(tempRow - food.row) <= 1)){
                    snake.growCount = 3;   
                    return FOOD_PX;
                }
        }
    }
    
    // if it is a wall or a snake body
    switch(snake.head.dir){
        case RIGHT:
        case LEFT:
            if((((displayBuffer[tempRow/8*DISPLAY_WIDTH + tempCol] >> (tempRow % 8)) & 0x01) == 1) ||
            (((displayBuffer[(tempRow-1)/8*DISPLAY_WIDTH + tempCol] >> ((tempRow-1) % 8)) & 0x01) == 1) ||
            (((displayBuffer[(tempRow+1)/8*DISPLAY_WIDTH + tempCol] >> ((tempRow+1) % 8)) & 0x01) == 1)){
                return WALL_OR_BODY_PX;
            } else{
                return EMPTY_PX;
            }
        case UP:
        case DOWN:
            if((uint8_t)tempCol != DISPLAY_HEIGHT){
                if((((displayBuffer[tempRow/8*DISPLAY_WIDTH + tempCol] >> (tempRow % 8)) & 0x01) == 1) ||
                (((displayBuffer[tempRow/8*DISPLAY_WIDTH + tempCol+1] >> (tempRow % 8)) & 0x01) == 1) ||
                (((displayBuffer[tempRow/8*DISPLAY_WIDTH + tempCol-1] >> (tempRow % 8)) & 0x01) == 1)){
                    return WALL_OR_BODY_PX;
                } else{
                    return EMPTY_PX;
                }
            }
    }
    return EMPTY_PX;
}
//
//---main functions--------------------
void moveSnakeTail(){
    Direction newTailDir = getNewTailDir();
    uint16_t index = (snake.tail.row/8)*DISPLAY_WIDTH + snake.tail.col;
    
    // clear old tail
    switch(snake.tail.dir){
        case RIGHT:
            displayBuffer[index]   &= ~(1 << (snake.tail.row % 8));
            if(snake.tail.col != 0){
                displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] &= ~(1 << ((snake.tail.row+1) % 8));
                displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] &= ~(1 << ((snake.tail.row-1) % 8));
            } else{
                displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + DISPLAY_WIDTH - 1] &= ~(1 << ((snake.tail.row+1) % 8));
                displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + DISPLAY_WIDTH - 1] &= ~(1 << ((snake.tail.row-1) % 8));            
            }
            break;
        case LEFT:
            displayBuffer[index]   &= ~(1 << (snake.tail.row % 8));
            if((uint8_t)snake.tail.col != DISPLAY_WIDTH-1){
                displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] &= ~(1 << ((snake.tail.row+1) % 8));
                displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] &= ~(1 << ((snake.tail.row-1) % 8));
            } else{
                displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + 0] &= ~(1 << ((snake.tail.row+1) % 8));
                displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + 0] &= ~(1 << ((snake.tail.row-1) % 8));            
            }
            break;
        case UP:
            displayBuffer[index]   &= ~(1 << (snake.tail.row % 8));
            if((uint8_t)snake.tail.row != DISPLAY_HEIGHT-1){
                displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] &= ~(1 << ((snake.tail.row+1) % 8));
                displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] &= ~(1 << ((snake.tail.row+1) % 8));
            } else{
                displayBuffer[snake.tail.col + 1] &= ~1;
                displayBuffer[snake.tail.col - 1] &= ~1;           
            }
            break;
        case DOWN:
            displayBuffer[index]   &= ~(1 << (snake.tail.row % 8));
            if(snake.tail.row != 0){
                displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] &= ~(1 << ((snake.tail.row-1) % 8));
                displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] &= ~(1 << ((snake.tail.row-1) % 8));
            } else{
                displayBuffer[((DISPLAY_HEIGHT-1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] &= ~(1 << 7);
                displayBuffer[((DISPLAY_HEIGHT-1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] &= ~(1 << 7);           
            }
            break;
    }
    
    // new tail direction and coordinates
    snake.tail.dir = newTailDir;
    switch(snake.tail.dir){
        case RIGHT:
            snake.tail.col++;
            break;
        case LEFT:
            snake.tail.col--;
            break;
        case UP:
            snake.tail.row--;
            break;
        case DOWN:
            snake.tail.row++;
            break;
    }
    
    // draw new tail
    switch(snake.tail.dir){
        case RIGHT:
            displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + ((uint8_t)snake.tail.col) - 1] |= (1 << ((snake.tail.row+1) % 8));
            displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + ((uint8_t)snake.tail.col) - 1] |= (1 << ((snake.tail.row-1) % 8));
            snake.tail.col = ((uint8_t)snake.tail.col == DISPLAY_WIDTH ? 0 : snake.tail.col);
            break;
        case LEFT:
            displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] |= (1 << ((snake.tail.row+1) % 8));
            displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] |= (1 << ((snake.tail.row-1) % 8));
            snake.tail.col = (snake.tail.col <= -1 ? DISPLAY_WIDTH-1 : snake.tail.col);
            break;
        case UP:
            displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] |= (1 << ((snake.tail.row+1) % 8));
            displayBuffer[((snake.tail.row+1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] |= (1 << ((snake.tail.row+1) % 8));
            snake.tail.row = (snake.tail.row <= -1 ? DISPLAY_HEIGHT-1 : snake.tail.row);
            break;
        case DOWN:
            displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col + 1] |= (1 << ((snake.tail.row-1) % 8));
            displayBuffer[((snake.tail.row-1)/8)*DISPLAY_WIDTH + snake.tail.col - 1] |= (1 << ((snake.tail.row-1) % 8));
            snake.tail.row = ((uint8_t)snake.tail.row == DISPLAY_HEIGHT ? 0 : snake.tail.row);
            break;
    }
}

uint8_t moveSnakeHead(){
    uint16_t index = (snake.head.row/8)*DISPLAY_WIDTH + snake.head.col;
    uint8_t nextPx;
    
    //clear old head
    switch(snake.head.dir){
        case RIGHT:
            displayBuffer[index]   &= ~(1 << (snake.head.row % 8));
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col]   &= ~(1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col]   &= ~(1 << ((snake.head.row-1) % 8));
            
            if(snake.head.col != 0){
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col-1]   &= ~(1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col-1]   &= ~(1 << ((snake.head.row-1) % 8));
            } else{
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + DISPLAY_WIDTH-1]   &= ~(1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + DISPLAY_WIDTH-1]   &= ~(1 << ((snake.head.row-1) % 8));
            }
            break;
        case LEFT:
            displayBuffer[index]   &= ~(1 << (snake.head.row % 8));
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col]   &= ~(1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col]   &= ~(1 << ((snake.head.row-1) % 8));
            
            if((uint8_t)snake.head.col != DISPLAY_WIDTH-1){
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col+1]   &= ~(1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col+1]   &= ~(1 << ((snake.head.row-1) % 8));
            } else{
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + 0]   &= ~(1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + 0]   &= ~(1 << ((snake.head.row-1) % 8));
            }
            break;
        case UP:
            displayBuffer[index]   &= ~(1 << (snake.head.row % 8));
            displayBuffer[index-1] &= ~(1 << (snake.head.row % 8));
            displayBuffer[index+1] &= ~(1 << (snake.head.row % 8));
            
            if((uint8_t)snake.head.row != DISPLAY_HEIGHT-1){
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col-1] &= ~(1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col+1] &= ~(1 << ((snake.head.row+1) % 8));
            } else{
                displayBuffer[snake.head.col-1] &= ~1;
                displayBuffer[snake.head.col+1] &= ~1;
            }
            break;
        case DOWN:
            displayBuffer[index]   &= ~(1 << (snake.head.row % 8));
            displayBuffer[index-1] &= ~(1 << (snake.head.row % 8));
            displayBuffer[index+1] &= ~(1 << (snake.head.row % 8));
            
            if(snake.head.row != 0){
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col-1] &= ~(1 << ((snake.head.row-1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col+1] &= ~(1 << ((snake.head.row-1) % 8));                
            } else{
                displayBuffer[((DISPLAY_HEIGHT-1)/8)*DISPLAY_WIDTH + snake.head.col-1] &= ~(1 << 7);
                displayBuffer[((DISPLAY_HEIGHT-1)/8)*DISPLAY_WIDTH + snake.head.col+1] &= ~(1 << 7);    
            }
            break;
    }
    
    // generated new coordinates
    switch(snake.newDir){
        case RIGHT:
            if(snake.head.dir == snake.newDir){
                snake.head.col++;
            } else if(snake.head.dir == UP){
                snake.head.row++;
                snake.head.col +=2;
            } else if(snake.head.dir == DOWN){
                snake.head.row--;
                snake.head.col +=2;
            }
            break;
        case LEFT:
            if(snake.head.dir == snake.newDir){
                snake.head.col--;
            } else if(snake.head.dir == UP){
                snake.head.row++;
                snake.head.col -=2;
            } else if(snake.head.dir == DOWN){
                snake.head.row--;
                snake.head.col -=2;
            }
            break;
        case UP:
            if(snake.head.dir == snake.newDir){
                snake.head.row--;
            } else if(snake.head.dir == RIGHT){
                snake.head.col--;
                snake.head.row -=2;
            } else if(snake.head.dir == LEFT){
                snake.head.col++;
                snake.head.row -=2;
            }
            break;
        case DOWN:
            if(snake.head.dir == snake.newDir){
                snake.head.row++;
            } else if(snake.head.dir == RIGHT){
                snake.head.col--;
                snake.head.row +=2;
            } else if(snake.head.dir == LEFT){
                snake.head.col++;
                snake.head.row +=2;
            }
            break;
    }
    
    // next pixel and else
    snake.head.dir = snake.newDir;
    nextPx = getNextHeadPx();
    index = (snake.head.row/8)*DISPLAY_WIDTH +(uint8_t) snake.head.col;
    
    // clear big food px
    if(nextPx == FOOD_PX && food.isBig){
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col]     &= ~(1 << (food.row % 8));
        displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] &= ~(1 << ((food.row-1) % 8));
        displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] &= ~(1 << ((food.row+1) % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1]   &= ~(1 << (food.row % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1]   &= ~(1 << (food.row % 8));
    }
    
    
    // draw new head
    switch(snake.head.dir){
        case RIGHT:
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + ((uint8_t) snake.head.col)-1]   |= (1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + ((uint8_t) snake.head.col)-1]   |= (1 << ((snake.head.row-1) % 8));
            displayBuffer[index-1]   |= (1 << (snake.head.row % 8));
            //displayBuffer[index-2]   |= (1 << (snake.head.row % 8));
        
            snake.head.col = ((uint8_t) snake.head.col ==  DISPLAY_WIDTH  ? 0 : snake.head.col);
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << (snake.head.row % 8));
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row-1) % 8));
            break;
        case LEFT:
            if(snake.head.col == -1){
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH]   |= (1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH]   |= (1 << ((snake.head.row-1) % 8));
        
                displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH]   |= (1 << (snake.head.row % 8));
                //displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH+1]   |= (1 << (snake.head.row % 8));
            } else{
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + ((uint8_t)snake.head.col)+1]   |= (1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + ((uint8_t)snake.head.col)+1]   |= (1 << ((snake.head.row-1) % 8));
        
                displayBuffer[index+1]   |= (1 << (snake.head.row % 8));
                //displayBuffer[index+2]   |= (1 << (snake.head.row % 8));
            }
            
            snake.head.col = (snake.head.col == -1 ? DISPLAY_WIDTH-1  : snake.head.col);
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << (snake.head.row % 8));
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row-1) % 8));
            break;
        case UP:
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row+2)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row+2) % 8));
            
            
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col-1] |= (1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col+1] |= (1 << ((snake.head.row+1) % 8));
        
            snake.head.row = (snake.head.row <= -1 ? DISPLAY_HEIGHT-1 : snake.head.row);
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << (snake.head.row % 8));
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col-1] |= (1 << (snake.head.row % 8));
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col+1] |= (1 << (snake.head.row % 8));
            break;
        case DOWN:
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col-1] |= (1 << ((snake.head.row-1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col+1] |= (1 << ((snake.head.row-1) % 8));
            
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row-1) % 8));
            displayBuffer[((snake.head.row-2)/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << ((snake.head.row-2) % 8));
            
            snake.head.row = ((uint8_t) snake.head.row >=  DISPLAY_HEIGHT ? 0 : snake.head.row);
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col]   |= (1 << (snake.head.row % 8));
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col+1] |= (1 << (snake.head.row % 8));
            displayBuffer[(snake.head.row/8)*DISPLAY_WIDTH + snake.head.col-1] |= (1 << (snake.head.row % 8));
            break;
    }
    return nextPx;
}

void snakeMove(){
    // move snake head and check new pixel (empty, food, or wall (body))
    switch(moveSnakeHead()){
        // snake just move at new pixel
        case EMPTY_PX:
            // grow if snake eat big or small food
            if(snake.growCount){
                snake.growCount--;
            } else{
                moveSnakeTail();
            }
            
            isSnakeStop = SNAKE_NOT_STOP;
            break;
        
        // snake crash into the wall or body (game over)
        case WALL_OR_BODY_PX:
            isSnakeStop = SNAKE_STOP;
            break;
        
        // snake on food px
        case FOOD_PX:            
            score += (food.isBig ? 3 : 1);
            snake.length += (food.isBig ? 3 : 1);
            // generated new food
            genFood();
        
            isSnakeStop = (score == winScore ? SNAKE_STOP : SNAKE_NOT_STOP);
            break;
    }
}

//
//---difficulty selection---------------
void writeSnakePic(){
    // write start snake pickture
    displayBuffer[37] |= 0x14;
    displayBuffer[38] |= 0x08;
    displayBuffer[39] |= 0x08;
    displayBuffer[40] |= 0x08;
    displayBuffer[41] |= 0x08;
    displayBuffer[42] |= 0x08;
    displayBuffer[43] |= 0x08;
    displayBuffer[44] |= 0x08;
    displayBuffer[45] |= 0x1C;
    displayBuffer[46] |= 0x1C;        

    displayBuffer[80] |= 0x1C;
    displayBuffer[81] |= 0x1C;
    displayBuffer[82] |= 0x08;
    displayBuffer[83] |= 0x08;
    displayBuffer[84] |= 0x08;
    displayBuffer[85] |= 0x08;
    displayBuffer[86] |= 0x08;
    displayBuffer[87] |= 0x08;
    displayBuffer[88] |= 0x08;
    displayBuffer[89] |= 0x14;
}

void drawDiffMenu(){
    writeSnakePic();
    // write hello string
    lcdStruct.byteIndex = 48;
    lcdStruct.writeStringToBuffer("Snake");
    lcdStruct.byteIndex = (DISPLAY_HEIGHT >=64 ? DISPLAY_WIDTH*3 + 12 : DISPLAY_WIDTH*2 + 12);
    lcdStruct.writeStringToBuffer("Select difficulty");
    lcdStruct.byteIndex = (DISPLAY_HEIGHT >=64 ? DISPLAY_WIDTH*5 + 12 : DISPLAY_WIDTH*3 + 12);
    switch(gameMode){
        case GAME_EASY_MODE:
            lcdStruct.writeStringToBuffer(">easy  mid  hard");
            break;
        case GAME_MID_MODE:
            lcdStruct.writeStringToBuffer(" easy >mid  hard");
            break;
        case GAME_HARD_MODE:
            lcdStruct.writeStringToBuffer(" easy  mid >hard");
    }
    lcdStruct.displayFullUpdate();
}

void difficultySelection(){
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    gameMode = 0;
    drawDiffMenu();
    
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delayUs(10000);
            gameMode -= (gameMode == 0 ? 0 : 1);
            drawDiffMenu();
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delayUs(10000);
            gameMode += (gameMode == 2 ? 0 : 1);
            drawDiffMenu();
        }
        #ifdef MH046
            if(isButtonPressed(BUTTON_BOTOOM)){
                delayUs(10000);
                lcdStruct.clearOrFillDisplay(CLEAR);
                return;
            }
        #else
            if(isButtonPressed(BUTTON_SELECT)){
                delayUs(10000);
                lcdStruct.clearOrFillDisplay(CLEAR);
                return;
            }
        #endif
        delayUs(100000);
    }
}
void drawSongMenu(){
    lcdStruct.byteIndex = 30;
    lcdStruct.writeStringToBuffer("Play music?");
    lcdStruct.byteIndex = DISPLAY_WIDTH*2 + 30;
    switch(isSnakeSong){
        case 0:
            lcdStruct.writeStringToBuffer(">no  yes");
            break;
        case 1:
            lcdStruct.writeStringToBuffer(" no >yes");
            break;
    }
    lcdStruct.displayFullUpdate();
}
void songSelect(){
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    isSnakeSong = 0;
    drawSongMenu();
    
    while(1){
        if(isButtonPressed(BUTTON_LEFT)){
            delayUs(10000);
            isSnakeSong -= (isSnakeSong == 0 ? 0 : 1);
            drawSongMenu();
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            delayUs(10000);
            isSnakeSong += (isSnakeSong == 1 ? 0 : 1);
            drawSongMenu();
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
//---game function-----------------
void startSnakeGame(void){
    difficultySelection();
    songSelect();
    
    // game initialise
    gameInit();
    
    uint16_t SnakeGameSong[]       = {C4, D4,   F4, D5, P, A4_B, A4, G4,F4,     F4, D5,P, D5, C4, D4,   F4, D5, P, A4_B, A4, G4,F4,   F4, E4, D4, C4,    E4, D5, P, F4, C4, D4, F4,   F4, D5, P, D5, P};
    uint8_t SnakeGameSong_Beats[]  = {32, 64,   64, 32, 32,32,   32, 32,32,     32, 32,32,32, 32, 64,   64, 32, 32,32,   32, 32,32,   64, 64, 64, 64,    64, 32, 32,32, 32, 32, 32,   32, 32, 32,64, 32};
        
    // play music
    if(isSnakeSong){
        playBackgroundSong(SPEAKER_TYPE_BIG, SnakeGameSong, SnakeGameSong_Beats, sizeof(SnakeGameSong)/2, 200,1);
    }
    
    // main game loop
    Direction newDir = snake.newDir;
    while(!isSnakeStop){
        // get new snake direction from user
        if(isButtonPressed(BUTTON_TOP)){
            if(gameMode == GAME_EASY_MODE){
                if((uint8_t)snake.head.col < DISPLAY_WIDTH-2 && snake.head.col > 1 && 
                    !(abs(snake.head.col - snake.tail.col)<4 && snake.head.col != snake.tail.col)){
                    newDir = UP;
                }
            } else{
                newDir = UP;
            }
        }
        if(isButtonPressed(BUTTON_BOTOOM)){
            if(gameMode == GAME_EASY_MODE){
                if((uint8_t)snake.head.col < DISPLAY_WIDTH-2 && snake.head.col > 1 && 
                    !(abs(snake.head.col - snake.tail.col)<4 && snake.head.col != snake.tail.col)){
                     newDir = DOWN;
                }
            } else{
                newDir = DOWN;
            }
        }
        if(isButtonPressed(BUTTON_LEFT)){
            if(gameMode == GAME_EASY_MODE){
                if(gameMode == GAME_EASY_MODE && (uint8_t)snake.head.row < DISPLAY_HEIGHT-2 && snake.head.row > 1 && 
                    !(abs(snake.head.row - snake.tail.row)<4 && snake.head.row != snake.tail.row)){
                     newDir = LEFT;
                }
            } else{
                newDir = LEFT;
            }
        }
        if(isButtonPressed(BUTTON_RIGHT)){
            if(gameMode == GAME_EASY_MODE){
                if(gameMode == GAME_EASY_MODE && (uint8_t)snake.head.row < DISPLAY_HEIGHT-2 && snake.head.row > 1 && 
                    !(abs(snake.head.row - snake.tail.row)<4 && snake.head.row != snake.tail.row)){
                     newDir = RIGHT;
                }
            } else{
                newDir = RIGHT;
            }
        }
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                musicSet.isCyclickSong = 0;
                musicSet.noteNumber = song.noteCount;
                lcdStruct.clearOrFillDisplay(CLEAR);
                return;
            }
        #endif
            
        // pause
        #ifdef BUTTON_SELECT
            if(isButtonPressed(BUTTON_SELECT)){
                displayBuffer[0] = (gameMode == GAME_EASY_MODE ? 0x01 : 0xFE);
                lcdStruct.displayFullUpdate();
                musicSet.isCyclickSong = 0;
                musicSet.noteNumber = song.noteCount;
                delayUs(1000000);
                
                while(!isButtonPressed(BUTTON_SELECT)){}
                if(isSnakeSong){
                    playBackgroundSong(SPEAKER_TYPE_BIG, SnakeGameSong, SnakeGameSong_Beats, sizeof(SnakeGameSong)/2, 200,1);
                }
                displayBuffer[0] = (gameMode == GAME_EASY_MODE ? 0x00 : 0xFF);
                lcdStruct.displayFullUpdate();
                delayUs(500000);
            }
        #endif
        
        if(newDir != ((~snake.newDir) & 0x03)){
            snake.newDir = newDir;
        }
            
        // move and draw snake
        snakeMove();
        lcdStruct.displayFullUpdate();
    }
    
    //-----------game end--------
    musicSet.isCyclickSong = 0;
    musicSet.noteNumber = song.noteCount;
    
    // draw win or lose message
    lcdStruct.byteIndex = DISPLAY_WIDTH + 30;
    lcdStruct.writeStringToBuffer((score != winScore ? "You LOSE!" : "You WIN!"));
    
    // draw score message
    char receiveString[5];
    sprintf(receiveString, "%05u", score);
    lcdStruct.byteIndex = DISPLAY_WIDTH*2 + 30;
    lcdStruct.writeStringToBuffer("Score: ");
    lcdStruct.writeStringToBuffer(receiveString);
    
    lcdStruct.displayFullUpdate();
    
    // wait and return to main menu
    delayUs(2500000);
    lcdStruct.clearOrFillDisplay(CLEAR);
}
