/** Using UTF8 file encoding
  * @file    game_tetris.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    26-June-2022
  * @brief   Эта библиотека реализует игру Тетрис.
  * Экран следует расположить вертикально, а также изменены назначения кнопок. 
  * В этой игре 7 стандартных фигур (у каждой есть своё имя):
  * (фигуры потом записываются в массив в виде байт. Запись начинается с левого верхнего угла картинки)
	* Куб - Smashboy
	*  {0,0, 0,0, 0,0, 0,0} <- young pixel (0x00)
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}  

	* T-образная - Teewee
	*  {0,0, 0,0, 1,1, 0,0} <- young pixel (0x0C)
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}

	* J-образная - Orenge Ricky
	*  {0,0, 0,0, 1,1, 1,1} <- young pixel
	*  {0,0, 0,0, 1,1, 1,1}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0} 

	* L-образная - Blue Ricky
	*  {0,0, 1,1, 1,1, 0,0} <- young pixel
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}

	* Z-образная - Cleveland Z
	*  {0,0, 0,0, 1,1, 0,0} <- young pixel
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}

	* S-образная - Rhode island Z
	*  {0,0, 1,1, 0,0, 0,0} <- young pixel
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 1,1, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 1,1, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}
	*  {0,0, 0,0, 0,0, 0,0}

	* I-образная - Hero
	*  {0,0, 1,1, 0,0, 0,0} <- young pixel
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}
	*  {0,0, 1,1, 0,0, 0,0}    */
  
#include "game_tetris.h"

/*============================== Структуры и переменные ==============================*/
// Текущие координаты верхнего правого угла фигуры
static int curFigX = 0, curFigY = 8;

// Тип фигуры, тип следующей фигуры, счёт, счёт для победы, количество удалённых строк
static uint16_t curFigType = 0, nextFigType = 0, score = 0, winScore = 60000, lines = 0;

// Массив с текущей фигурой
static uint8_t curFig[8] = {0x00};

// Состояние игры и флаг, является ли это первым запуском игры
static uint8_t gameState = GAME_CONTINUE, fisrtStart = 1;

// Текущая задержка между циклами отрисовки (отвечает за скорость игры)
static long curDelay = 30000;

// Играет ли сейчас песня из тетриса
static uint8_t isTetrisSong = 0;

// Массив со всеми фигурами
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
/*============================== Функции ==============================*/
/* Инициализация игры */
void gameInit(void)
{
    // Установка значений по-умолчанию
    gameState = GAME_CONTINUE;
    curFigX = 0; curFigY = 8;
    score = 0; lines = 0;
    curDelay = 30000;
    fisrtStart = 1;
    memset(displayBuffer, 0, LCD_BUFFER_LENGTH);
    
    // Генерация текущей и следующей фигуры
    genNextFigType();
    
    // Запись текущей фигуры на поле
    writeFigToField();
    
    // Рисуем границу поля
    for(uint16_t index = 0; index < DISPLAY_WIDTH; index++){
        displayBuffer[(TETRIS_FIELD_WIDTH/8)*DISPLAY_WIDTH + index] |= (0x01 << TETRIS_FIELD_WIDTH%8);
    }
    
	// Записываем счёт и отрисовываем буфер дисплея
    updateScoreAndFigure();
    lcdStruct.displayFullUpdate();
}
//
/* Специальный генератор типов фигур, предотвращающий "засухи" и "наводнения" фигур 
 * @retval: тип очередной фигуры*/
uint8_t tgm3Randomizer(void)
{
    // Мешок из 35 типов фигур
    uint8_t pool[35] = {
        6, 6, 2, 6, 3, 1, 1, 
        6, 2, 5, 0, 3, 4, 0, 
        1, 0, 4, 2, 6, 5, 5, 
        0, 2, 3, 3, 0, 4, 5, 
        3, 1, 4, 1, 5, 4, 2
    };
    
    // Специальный первый кусочек
    uint8_t firstPiece = randLcg() % 4;
    static uint8_t firstGen = 1;
    if(firstGen){
        firstGen = 0;
        return firstPiece;
    }
    
    static uint8_t history[4] = {5, 4, 5, 0};
    
    uint8_t roll, i, piece;
    while(1){
        for(roll = 0; roll <6; roll++){
            i = randLcg() % 35;
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
        
        // Сдвигаем историю и добавляем новый кусочек
        for(i = 0; i < 3; i++){
            history[i] = history[i+1];
        }
        history[3] = piece;
        
        return piece;
    }
}

/* Генерирует новый тип фигуры */
void genNextFigType(void)
{
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

/* Запись фигуры в поле */
void writeFigToField(void)
{
    uint8_t col, row = curFigY+1;
    
    // Если пустая часть фигуры находится за экраном справа
    if(curFigY < 0){
        for(col = 0; col < 8; col++){
            displayBuffer[curFigX+col] |= (curFig[col] >> -curFigY);
        }
        return;
    }
    
    // Тоже самое, но слево
    if(curFigY > DISPLAY_HEIGHT-8){
        for(col = 0; col < 8; col++){
            displayBuffer[DISPLAY_HEIGHT/8*DISPLAY_WIDTH + curFigX+col] |= (curFig[col] << curFigY%8);
        }
        return;
    }
    
    // Разделяем фигуру на две части (на первой странице и на второй)
    // Считаем, как много линий на одной странице
    while(curFigY/8 == row/8){row++;}
    
    // Если все линии на одной странице
    if(row == curFigY+8){
        for(col = 0; col < 8; col++){
            displayBuffer[(curFigY >> 3)*DISPLAY_WIDTH + curFigX+col] |= curFig[col];
        }
        return;
    }
    
    // Записываем на одну старницу
    for(col = 0; col < 8; col++){
        displayBuffer[(curFigY >> 3)*DISPLAY_WIDTH + curFigX+col] |= (curFig[col] << (8-(row - curFigY)));
    }
    
    // Записываем на другую страницу
    for(col = 0; col < 8; col++){
        displayBuffer[(row >> 3)*DISPLAY_WIDTH + curFigX+col] |= (curFig[col] >> (row - curFigY));
    }
}

/* Проверям, проиграли ли мы или нет
 *@retval: 1 - проиграли, 0 - нет*/
uint8_t isLost(void)
{
    for(uint8_t row = 0 ; row < TETRIS_FIELD_WIDTH; row++){
        if(displayBuffer[(row/8)*DISPLAY_WIDTH] & (0x01 << (row%8))) return 1;
    }
    return 0;
}

/* Сдвигаем все фигуры на указанное число строк вниз
 * lineNumber: колв-во строк, на которое надо сдвинуть все фигуры */
void shiftAllDown(uint16_t lineNumber)
{
    int col, newcol = lineNumber-1;
    for(col = lineNumber; col > 0; newcol--, col--){
        // Пропускаем пустые строки
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

/* Проверяем и сдвигаем все линии поля */
void checkAndShiftLines(void)
{
    uint16_t col, emptyLineCount = 0, firstLine = 0;
    // Удаляем полную линию тетриса
    for(col = DISPLAY_WIDTH-1; col > 0; col--){
        // Если линия полная
        if(displayBuffer[col] == 0xFF && displayBuffer[DISPLAY_WIDTH + col] == 0xFF &&
            (displayBuffer[2*DISPLAY_WIDTH + col]&0x0F) == 0x0F){
            
			// Удаляем линию
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
        
        // Увеличиваем счёт
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

/* Обновляем счёт и фигуру */
void updateScoreAndFigure(void)
{
    lcdStruct.byteIndex = DISPLAY_WIDTH*7;
    LCD_writeHorStringToBuffer(" Next");
    
    // Рисуем следующую фигуру
    uint8_t i;
    for(i = 0; i < 8; i++){
        displayBuffer[DISPLAY_WIDTH*5 + 9 + i] = FIGURES[nextFigType*8 + i];
    }
    
    // Рисуем счёт
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+24;
    LCD_writeHorStringToBuffer("Score:");
    
    char receiveString[5];
    sprintf(receiveString, "%05u", score);
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+32 ;
    LCD_writeHorStringToBuffer(receiveString);
    
    // Рисуем кол-во убранных линий
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+48;
    LCD_writeHorStringToBuffer("Lines:");
    
    sprintf(receiveString, "%05u", lines);
    lcdStruct.byteIndex = DISPLAY_WIDTH*7+56;
    LCD_writeHorStringToBuffer(receiveString);
}
//
//---Функции движения фигуры----------------
/* Фращает фигуру */
void rotate(void)
{
    // Если это куб, то ничего не меняется
    if(curFigType == 0) return;
    
    // Очищаем старую фигуру
    for(int y = 0; y < 8; y++){
        for(int x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
	// Является ли фигура 3х3 и буфер для фигуры
    uint8_t is3x3 = 1, buffer[8] = {0x00};
    
    // Определяем, это фигура 3х3 или нет
    for(int x = 0; x < 8; x++){
        if(curFig[x]&0xC0){
            is3x3 = 0;
            break;
        }
    }
    
    if(curFig[7] != 0 || curFig[6] != 0){
        is3x3 = 0;
    }
    
    // Записываем повернутую фигуру в буфер
    uint8_t maxRow = (is3x3 ? 6 : 8);
    for(int y = 0; y < maxRow; y++){
        for(int x = 0; x < maxRow; x++){
            buffer[x] |= (((curFig[y] >> (maxRow-1-x)) & 0x01) << y);
        }
    }
    
    //---Меняем координату y, если мы рядом с границей ----------
    int temp = curFigY;
    
    // Меняем координату y, если мы рядом с левой границей
    if((!is3x3 && curFigY > 12) || (is3x3 && curFigY > 14)){
        for(int y = 7; y > 3; y-=2){
            for(int x = 0; x < 8; x++){
                if((buffer[x] >> y) & 0x01){
                    curFigY -= 2;
                    break;
                }
            }
            if(curFigY < 12) break;
        }
    }
    
    // Меняем координату y, если мы рядом с правой границей
    if(curFigY < 0){
        for(int y = 0; y < 8; y+=2){
            for(int x = 0; x < 8; x++){
                if((buffer[x] >> y) & 0x01){
                    curFigY += 2;
                    break;
                }
            }
            if(curFigY >= 0) break;
        }
    }
    
    //---Проверяем возможность повернуть фигуру-----------------------------------------
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            if(((buffer[x] >> y) & 0x01) && 
                ((displayBuffer[(curFigY+y)/8*DISPLAY_WIDTH + curFigX+x] >> (curFigY+y)%8) & 0x01)){
                    curFigY = temp;
                    return;
                }
                
        }
    }

    // Проверяем углы
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            if(((buffer[x] >> y) & 0x01) && 
                (curFigY+y > TETRIS_FIELD_WIDTH-1 || curFigX+x > DISPLAY_WIDTH-1)){
                    curFigY = temp;
                    return;
                }
        }
    }
    
    // Записываем повернутую фигуру в буфер
    for(int x = 0; x < 8; x++){
        curFig[x] = buffer[x];
    }
}

/* Сдвигает фигуру влево */
void shiftLeft(void)
{
	// Координаты и кол-во пустых строк
    int x, y, emptylines = 0;
    uint8_t empty = 1, possible = 1;
    
    // Очищаем старую фигуру
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    // Считаем кол-во свободных строк
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
    
    // Проверяем, возможен ли сдвиг
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

/* Сдвигает фигуру вправо */
void shiftRight(void)
{
	// Координаты и кол-во пустых строк
    int x, y, emptylines = 0;
    uint8_t empty = 1, possible = 1;
    
    // Очищаем старую фигуру
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    // Считаем кол-во свободных строк
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
    
    // Проверяем, возможен ли сдвиг
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

/* Сдвиг фигуры вниз 
 * @retval: 0, если сдвиг успешен, 1, если фигура достигла конца */
uint8_t shiftDown(void)
{
	// Координаты и кол-во пустых строк
	int x, y;
    uint8_t empty = 1, possible = 1, emptylines = 0;
    
    // Считаем кол-во свободных строк
    for(x = 7; x >= 0; x--){
        if(curFig[x]){
            empty = 0;
            break;
        }
        if(!empty) break;
        emptylines++;
    }    
    
    // Очищаем старую фигуру
    for(y = 0; y < 8; y++){
        for(x = curFigX; x < curFigX+8; x++){
            if(curFig[x - curFigX] & (1 << y)){
                displayBuffer[((curFigY+y)/8)*DISPLAY_WIDTH + x] &= ~(1 << ((curFigY+y)%8));
            }
        }
    }
    
    // Проверяем, возможен ли сдвиг
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

/* Рисуем меню выбора музыки */
void drawSongMenuTetris(void)
{
    lcdStruct.byteIndex = 20;
    lcdStruct.writeStringToBuffer("Включить музыку?");
    lcdStruct.byteIndex = DISPLAY_WIDTH*2 + 20;
    switch(isTetrisSong){
        case 0:
            lcdStruct.writeStringToBuffer(">нет  да");
            break;
        case 1:
            lcdStruct.writeStringToBuffer(" нет >да");
            break;
    }
    lcdStruct.displayFullUpdate();
}

/* Логика работы меню включения мелодии */
void songSelectTetris(void)
{
	// Очищаем буфер
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    isTetrisSong = 0;
	
	// Рисуем меню включения мелодии
    drawSongMenuTetris();
    
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		 
        if(isButtonPressed(BUTTON_LEFT)) {
            delayUs(buttonDelayUs);
            isTetrisSong -= (isTetrisSong == 0 ? 0 : 1);
            drawSongMenuTetris();
        }
		
        if(isButtonPressed(BUTTON_RIGHT)){
            delayUs(buttonDelayUs);
            isTetrisSong += (isTetrisSong == 1 ? 0 : 1);
            drawSongMenuTetris();
        }
		
        if(isButtonPressed(BUTTON_SELECT)){
            delayUs(buttonDelayUs);
            lcdStruct.clearOrFillDisplay(CLEAR);
            return;
        }
    }
}

/* Главная функция игры */
void startTetrisGame(void)
{
	// Включение музыки и инициализация игры
    songSelectTetris();
    gameInit();
    
	// Включение мелодии
    if(isTetrisSong){
        playBackgroundSong(SPEAKER_TYPE_BIG, TetrisGameSong, TetrisGameSong_Beats, sizeof(TetrisGameSong)/2, 140,1);
    }
    
	// Сдвигается ли фигура вниз и поворачивается ли она
    uint8_t isShiftDown = 0, isShiftOrRotate = 0;
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
        // Обновляем задержку с увеличением счёта
		if(curDelay > 0) {
			curDelay = 30000 - score*10;
        } else {
			curDelay = 0;
		}
        
		// Сдвиг фигуры вправо
        if(isButtonPressed(BUTTON_TOP)) {
			delayUs(40000);
            shiftRight();
            isShiftOrRotate = 1;
        }
        
		// Сдвиг фигуры влево
        if(isButtonPressed(BUTTON_BOTOOM)) {
			delayUs(40000);
            shiftLeft();
            isShiftOrRotate = 1;
        }
        
		// Поворот фигуры
        if(isButtonPressed(BUTTON_LEFT)) {
			delayUs(20000);
            rotate();
            isShiftOrRotate = 1;
            if(curDelay <= 50000){
                delayUs(90000);
            }
        }
        
        // Обновление дисплея, если фигура повернута или сдвинута
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
        
        // Нормальный или быстрый сдвиг фигуры
        do{
            // Сдвиг фигуры вниз
            isShiftDown = shiftDown();
            writeFigToField();
            
            if(!isShiftDown){
                // Проверяем, что игра ещё не закончена
                if(isLost()){
                    gameState = GAME_LOSE;
                    break;
                }
                
                // Проверяем линии
                checkAndShiftLines();
                
                // Генерируем и записываем новую фигуру
                genNextFigType();
                writeFigToField();
                
				// Обновляем счёт и отрисовываем на дисплее
                updateScoreAndFigure();
                lcdStruct.displayFullUpdate();
            }
            
            // Отрисовка экрана
            LCD_DrawPageFromBuffer(PAGE_2);
            if(curFigY <= 8){
                LCD_DrawPageFromBuffer(PAGE_1);
            } else{
                LCD_DrawPageFromBuffer(PAGE_3);
            }
        } while(isButtonPressed(BUTTON_RIGHT) && gameState != GAME_LOSE);
        
        // Выход из игры
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                songStop();
				
                lcdStruct.clearOrFillDisplay(CLEAR);
                break;
            }
        #endif
        
        // Пауза
        #ifdef BUTTON_SELECT
            if(isButtonPressed(BUTTON_SELECT)) {
                lcdStruct.byteIndex = DISPLAY_WIDTH*8-9;
                LCD_writeHorStringToBuffer("Pause");
                lcdStruct.displayFullUpdate();
                songPause();
                delayUs(1000000);
                
                while(!isButtonPressed(BUTTON_SELECT)){irProtocolAnalyze();}
                if(isTetrisSong){
                    song.isSongPlay = 1;
                }
                lcdStruct.byteIndex = DISPLAY_WIDTH*8-9;
                LCD_writeHorStringToBuffer("     ");
                lcdStruct.displayFullUpdate();
                delayUs(500000);
            }
        #endif
            
        //-----Конец игры---------
        if(score >= winScore){ gameState = GAME_WIN;}
        
        if(gameState != GAME_CONTINUE){
            songStop();
            
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
