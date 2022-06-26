/** Using UTF8 file encoding
  * @file    game_snake.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    25-June-2022
  * @brief   Эта библиотека реализует игру Змейка. 
  * =======================
  * Змейка имеет следующий вид:
  *   *        ***
  *    #*********#
  *   *        ***
  * 1. Отслеживается положение и направление только центрального пикселя головы и центрального пикселя хвоста змейки.
  * 2. Следующее положение хвоста определяется по тому, в каком направлении находится ближайший пиксель тела.
  * 3. По-умолчанию змейка не изменяет своего направления и просто перерисовываются её голова и хвост, сдвигаясь на один пиксель.
  * 4. Если пользователь нажал на кнопку, и если указано непротиворечивое направление, то змейка сменит своё направление,
  *    повернув голову. Хвост всегда определяет своё следующее направление по сегменту тела.
  * 5. Возможен выбор одного из трёх режимов сложности, а также включение и отключение музыкального сопровождения.
  *    Также распознаются команды от ИК-пульта.
  * 6. Реализованы два вида еды: маленькая (в один пиксель) и большая в 5 пикселей. Маленькая еда увеличивает змейку на 1 единицу,
  *    а также увеличивает счёт на 1. Большая еда увеличивает змейку на 3 единицы, а счёт на 5 единиц.
  * =======================
  * Особенности и проблемы:
  * 1. Нет способа отличить пиксель тела змейки от пикселя стены, поэтому в среднем и сложном режимах возможны баги.
  * 2. Когда хвост змеи и её голова в одной строке или в одном столбце, то не поворачивается
  */

#include "game_snake.h"

/*============================== Структуры и переменные ==============================*/
// Структура с начальными положениями змейки и положением по умолчанию
static Snake snake, defaultSnake = {{START_SNAKE_ROW, START_SNAKE_COL + START_SNAKE_LENGTH-1, RIGHT}, 
                                    {START_SNAKE_ROW, START_SNAKE_COL, RIGHT}, RIGHT, START_SNAKE_LENGTH, 0
};
// Структура с координатами и размерами еды
static Food food = {0,0,0};

// Текущий счёт; и счёт, необходимый для победы
static uint16_t score = 0, winScore = DISPLAY_HEIGHT*DISPLAY_WIDTH - START_SNAKE_LENGTH - 10;

// Остановлена ли сейчас змейка (врезалась ли в стену) или нет
static uint8_t isSnakeStop = SNAKE_NOT_STOP;

// Проигрывается ли сейчас песня и какой режим игры
uint8_t isSnakeSong = SNAKE_NOT_STOP, gameMode = 0;

/*============================== Функции ==============================*/
/* Функция инициализации змеи (рисует змею) */
void snakeInit(void)
{
	// Координаты и направление по-умолчанию
    snake = defaultSnake;
    
    // Записываем тело змеи в буфер дисплея
    for(uint16_t i = 0; i < START_SNAKE_LENGTH; i++) {
        displayBuffer[START_SNAKE_ROW/8*DISPLAY_WIDTH + i+START_SNAKE_COL] |= (0x01 << (START_SNAKE_ROW % 8));
    }
	
    // Записываем хвост змеи в буфер дисплея
    displayBuffer[(START_SNAKE_ROW+1)/8*DISPLAY_WIDTH + START_SNAKE_COL-1] |= (0x01 << ((START_SNAKE_ROW+1) % 8));
    displayBuffer[(START_SNAKE_ROW-1)/8*DISPLAY_WIDTH + START_SNAKE_COL-1] |= (0x01 << ((START_SNAKE_ROW-1) % 8));
    
    // Записываем голову змеи в буфер дисплея
    displayBuffer[START_SNAKE_ROW/8*DISPLAY_WIDTH + START_SNAKE_LENGTH-2 +START_SNAKE_COL]   |= (0x07 << ((START_SNAKE_ROW-1) % 8));
    displayBuffer[START_SNAKE_ROW/8*DISPLAY_WIDTH + START_SNAKE_LENGTH-2 +START_SNAKE_COL+1] |= (0x07 << ((START_SNAKE_ROW-1) % 8));
}

// Инициализация игры (отрисовка змеи и установка начальных значений)
void gameInit(void)
{
    // Начальные значения
    lcdStruct.byteIndex = 0;
    score = 0;
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    isSnakeStop = SNAKE_NOT_STOP;
    winScore = DISPLAY_WIDTH*DISPLAY_HEIGHT- 500;
    
	// Рисуем рамку поля для среднего и сложного режимов
    if(gameMode != GAME_EASY_MODE) 
	{
		// Верхняя и нижняя границы
        for(uint16_t i = 1; i< DISPLAY_WIDTH; i++) {
            displayBuffer[i] |= 0x01;
            displayBuffer[(DISPLAY_HEIGHT/8-1)*DISPLAY_WIDTH + i] |= 0x80;
        }
		
		// Левая и правая границы
        for(uint16_t i = 0; i < LCD_BUFFER_LENGTH;) {
            displayBuffer[i] |= 0xFF;
            i += (i%DISPLAY_WIDTH == 0 ? DISPLAY_WIDTH-1 : 1 );
        }
    }
    
    // Рисуем крест для среднего режима
    if(gameMode == GAME_MID_MODE) 
	{
		// Устанавливаем счёт для победы
        winScore = DISPLAY_WIDTH*DISPLAY_HEIGHT- 1500;
		
		// Максимальная длина частей крестов
        uint8_t maxLen = (DISPLAY_HEIGHT >=64 ? 50 : 40);
		
		// Рисуем горизонтальные полоски
        for(uint16_t i = 10; i< maxLen; i++) {
            displayBuffer[(DISPLAY_HEIGHT/16-1)*DISPLAY_WIDTH + i] |= 0x80;
            displayBuffer[(DISPLAY_HEIGHT/16-1)*DISPLAY_WIDTH + DISPLAY_WIDTH/2 + i] |= 0x80;
        }
		
		// Рисуем вертикальные полоски
        maxLen = (DISPLAY_HEIGHT >=64 ? 20 : 10);
        for(uint16_t i = (DISPLAY_HEIGHT >=64 ? 10 : 8); i < maxLen ; i++) {
            displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2] |= (1 << (i%8));
            displayBuffer[((i+3+maxLen)/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2] |= (1 << ((i+3+maxLen)% 8));
        }
    }
    
	// Рисуем препятствия в сложном режиме
    if(gameMode == GAME_HARD_MODE)
	{
		// Счёт для победы
        winScore = 2000;
		
        // Горизонтальные линии с отверстиями
        for(uint16_t i = 0; i< DISPLAY_WIDTH; i++) {
            if((i > 10 && i < 15) || (i > 100 && i < 105) || (i > DISPLAY_WIDTH/2-3 && i < DISPLAY_WIDTH/2+3)) continue;
            displayBuffer[(DISPLAY_HEIGHT/16)*DISPLAY_WIDTH + i] |= (1 << (DISPLAY_HEIGHT/16));
        }
		
        // Первая вертикальная линия с отверстиями
        for(uint16_t i = 0; i< DISPLAY_HEIGHT; i++) {
            if((i > 5 && i < 10) || (i > DISPLAY_HEIGHT-15 && i < DISPLAY_HEIGHT-10)) continue;
            displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/4] |= (1 << (i%8));
        }
		
        // Вторая вертикальная линия с отверстиями
        for(uint16_t i = 0; i< DISPLAY_HEIGHT; i++){
            if((i > 10 && i < 15) || (i > DISPLAY_HEIGHT-15 && i < DISPLAY_HEIGHT-10)) continue;
            displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2+DISPLAY_WIDTH/4] |= (1 << (i%8));
        }
		
        // Горизонтальный тунель
        for(uint16_t i = DISPLAY_WIDTH/4-1; i< DISPLAY_WIDTH/2+DISPLAY_WIDTH/4+2; i++){
            if(DISPLAY_HEIGHT >= 64){
                displayBuffer[((DISPLAY_HEIGHT-15)/8)*DISPLAY_WIDTH+i] |= (1 << ((DISPLAY_HEIGHT-15)%8));
            }
            if(i > DISPLAY_WIDTH/2+3 && i < DISPLAY_WIDTH/2+8)continue;
            displayBuffer[((DISPLAY_HEIGHT-10)/8)*DISPLAY_WIDTH+i] |= (1 << ((DISPLAY_HEIGHT-10)%8));
            
        }
		
		// Мини-горизонтальный тунель для экранов с меньшими размерами
        if(DISPLAY_HEIGHT >= 64)
		{
            for(uint16_t i = 10; i< 18; i++){
                displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-15] |= (1 << (i%8));
                if(i >=15){
                    displayBuffer[(i/8)*DISPLAY_WIDTH + DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-10] |= (1 << (i%8));
                }
            }
            for(uint16_t i = DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-15; i< DISPLAY_WIDTH/2+DISPLAY_WIDTH/4+2; i++){
                displayBuffer[(10/8)*DISPLAY_WIDTH+i] |= (1 << (10%8));
                if(i > DISPLAY_WIDTH/2+DISPLAY_WIDTH/4-10){
                    displayBuffer[(15/8)*DISPLAY_WIDTH+i] |= (1 << (15%8));
                }
            }
        }
    }
    
    // Рисуем змейку по-умолчанию
    snakeInit();
    
    // Генерируем еду в случайном месте
    genFood();
    
    // Обновляем экран
    lcdStruct.displayFullUpdate();
}

/* Генерация еды в случайном месте */
void genFood(void)
{
    // Генерация случайных координат еды
    food.row = (2 +(randLcg() % (DISPLAY_HEIGHT - 2))) % (DISPLAY_HEIGHT - 2);
    food.col = (2 +(randLcg() % (DISPLAY_WIDTH - 2))) % (DISPLAY_WIDTH - 2);
	
	// Генерация маленькой еды с вероятностью 80%
    food.isBig = (randLcg() < MAX_RAND_NUM*0.8 ? 0 : 1);
	
    // Двигаем координаты еды, пока не найдём свободную клетку
    uint16_t stopCount = 0;
	
    // Для маленькой еды
    if(!food.isBig)
	{
		// Условия, чтобы еда не генерировалась вплотную к змее или стене
        while((((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] >> (food.row % 8)) & 0x01) == 1) ||
            ((displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] >> ((food.row-1) % 8)) & 0x01) == 1 ||
            ((displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] >> ((food.row+1) % 8)) & 0x01) == 1 ||
            ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1] >> (food.row % 8)) & 0x01) == 1 ||
            ((displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1] >> (food.row % 8)) & 0x01) == 1 ||
            (abs(food.row - snake.head.row) < 2) || (abs(food.col - snake.head.col) < 2) ||
             food.row < 3 || food.row > DISPLAY_HEIGHT-3 || food.col < 3 || food.col > DISPLAY_WIDTH-3)
		{
			// Увеличиваем столбец
            food.col++;
			
			// Если достигли конца, то переходим на новую строку
            if(food.col == DISPLAY_WIDTH) {
                food.col = 0;
                food.row++;
                food.row %= DISPLAY_HEIGHT;
            }
			
			// Двигаемся по полю до тех пор, пока не прошли всё поле по кругу
            stopCount++;
            if(stopCount > DISPLAY_WIDTH*DISPLAY_HEIGHT) return;
        }
        
        // Рисуем новую еду на поле
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] |= (1 << (food.row % 8));
    } else{
        // Условия появления для большой еды
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
               food.row < 3 || food.row > DISPLAY_HEIGHT-3 || food.col < 3 || food.col > DISPLAY_WIDTH-3)
		{
			// Увеличиваем столбец
            food.col++;
			
			// Если достигли конца, то переходим на новую строку
            if(food.col == DISPLAY_WIDTH){
                food.col = 0;
                food.row++;
                food.row %= DISPLAY_HEIGHT;
            }
			
			// Двигаемся по полю до тех пор, пока не прошли всё поле по кругу
            stopCount++;
            if(stopCount > DISPLAY_WIDTH*DISPLAY_HEIGHT) return;
        }
        
        // Записываем новую еду в буфер
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col] |= (1 << (food.row % 8));
        displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] |= (1 << ((food.row-1) % 8));
        displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] |= (1 << ((food.row+1) % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1] |= (1 << (food.row % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1] |= (1 << (food.row % 8));
    }
}

/* Расчёт нового направления для хвоста
 * @retval: новое направления для хвоста */
Direction getNewTailDir(void)
{
	// Смотрим, с какой стороны от хвоста есть пиксель тела, значит там и новое направление
	
	// Если справа от конца хвоста есть пиксель
    if(((displayBuffer[snake.tail.row/8*DISPLAY_WIDTH + snake.tail.col + 1] >> (snake.tail.row)%8) & 0x01) == 1){
        return RIGHT;
    }
	
	// Если слева от конца хвоста есть пиксель
    if(((displayBuffer[snake.tail.row/8*DISPLAY_WIDTH + snake.tail.col - 1] >> (snake.tail.row)%8) & 0x01) == 1){
        return LEFT;
    }
	
	// Если сверху от конца хвоста есть пиксель
    if(((displayBuffer[(snake.tail.row-1)/8*DISPLAY_WIDTH + snake.tail.col] >> (snake.tail.row-1)%8) & 0x01) == 1){
        return UP;
    }
	
	// Если снизу от конца хвоста есть пиксель
    if(((displayBuffer[(snake.tail.row+1)/8*DISPLAY_WIDTH + snake.tail.col] >> (snake.tail.row+1)%8) & 0x01) == 1){
        return DOWN;
    }
    
	// Условия для лёгкого режима, когда хвост на границе экрана
    if(snake.tail.row == 0) return UP;
    if((uint8_t)snake.tail.row == DISPLAY_HEIGHT-1) return DOWN;
    if((uint8_t)snake.tail.col == DISPLAY_WIDTH-1) return RIGHT;
    if(snake.tail.col == 0) return LEFT;
    
    return RIGHT;
}

/* Получение типа пикселя, на которой наползла змея
 * @reatval: один из следующих типов пикселей
 *           FOOD_PX - пиксель еды
 *           EMPTY_PX - пустой пиксель
 *           WALL_OR_BODY_PX - пиксель стены или тела */
uint8_t getNextHeadPx(void)
{
    // Определяем текущее положение уже сдвинутой головы
    int8_t tempCol = snake.head.col, tempRow = snake.head.row;
    switch(snake.head.dir) {
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
	
	// Если сейчас сгенерирована не большая еда
    if(!food.isBig) 
	{
		// Проверяем, что текущий пиксель - это пиксель еды
        switch(snake.head.dir) {
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
    
		// Проверяем, что текущий пиксель - это пиксель большой еды
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
    
    // Проверяем, что пиксель - это кусок стены или тела змеи
    switch(snake.head.dir) {
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
	
	// Иначе - это пустой пиксель
    return EMPTY_PX;
}
//
//------- Основные функции движения -------
/* Двигаем хвост змеи */
void moveSnakeTail(void)
{
	// Получаем новое направление для хвоста змеи
    Direction newTailDir = getNewTailDir();
	
	// Вычисляем один раз текущие координаты
    uint16_t index = (snake.tail.row/8)*DISPLAY_WIDTH + snake.tail.col;
    
    // Стираем старый хвост
    switch(snake.tail.dir)
	{
        case RIGHT:
			// Стираем центральный пиксель хвоста
            displayBuffer[index]   &= ~(1 << (snake.tail.row % 8));
            
			// Стираем старый хвост в зависимости от того, перешёл ли он экран или нет
			if(snake.tail.col != 0)
			{
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
    
    // Записываем новое направление хвоста и изменяем его координаты
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
    
    // Рисуем новый хвост (три пикселя)
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

/* Двигаем голову змеи
 * @reatval: один из следующих типов пикселей
 *           FOOD_PX - пиксель еды
 *           EMPTY_PX - пустой пиксель
 *           WALL_OR_BODY_PX - пиксель стены или тела */
uint8_t moveSnakeHead(void)
{
	// Вычисляем один раз текущие координаты
    uint16_t index = (snake.head.row/8)*DISPLAY_WIDTH + snake.head.col;
    uint8_t nextPx;
    
    // Очищаем старую голову (5 пикселей)
    switch(snake.head.dir){
        case RIGHT:
			// Три передних пикселя
            displayBuffer[index]   &= ~(1 << (snake.head.row % 8));
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + snake.head.col]   &= ~(1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + snake.head.col]   &= ~(1 << ((snake.head.row-1) % 8));
            
			// Два задних пикселя
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
    
    // определяем новые координаты головы
    switch(snake.newDir) {
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
    
    // Записываем новое направление головы, определяем следующий пиксель и текущие координаты
    snake.head.dir = snake.newDir;
    nextPx = getNextHeadPx();
    index = (snake.head.row/8)*DISPLAY_WIDTH +(uint8_t) snake.head.col;
    
    // Очищаем большую еду с поля
    if(nextPx == FOOD_PX && food.isBig){
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col]     &= ~(1 << (food.row % 8));
        displayBuffer[(food.row-1)/8*DISPLAY_WIDTH + food.col] &= ~(1 << ((food.row-1) % 8));
        displayBuffer[(food.row+1)/8*DISPLAY_WIDTH + food.col] &= ~(1 << ((food.row+1) % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col+1]   &= ~(1 << (food.row % 8));
        displayBuffer[food.row/8*DISPLAY_WIDTH + food.col-1]   &= ~(1 << (food.row % 8));
    }
    
    
    // Записываем голову змеи в буфер дисплея
    switch(snake.head.dir){
        case RIGHT:
            displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + ((uint8_t) snake.head.col)-1]   |= (1 << ((snake.head.row+1) % 8));
            displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + ((uint8_t) snake.head.col)-1]   |= (1 << ((snake.head.row-1) % 8));
            displayBuffer[index-1]   |= (1 << (snake.head.row % 8));
        
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
            } else{
                displayBuffer[((snake.head.row+1)/8)*DISPLAY_WIDTH + ((uint8_t)snake.head.col)+1]   |= (1 << ((snake.head.row+1) % 8));
                displayBuffer[((snake.head.row-1)/8)*DISPLAY_WIDTH + ((uint8_t)snake.head.col)+1]   |= (1 << ((snake.head.row-1) % 8));
        
                displayBuffer[index+1]   |= (1 << (snake.head.row % 8));
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
	
	// Возвращаем текущий пиксель, на который наехала змея
    return nextPx;
}

/* Главная функция, двигающая змею */
void snakeMove(void)
{
    // Двигаем голову змеи и проверяем пиксель, на который она наехала
    switch(moveSnakeHead())
	{
        // Если пиксель пустой, то просто двигаем змею
        case EMPTY_PX:
            // змея растёт (хвост не двигается), если до этого съела еду
            if(snake.growCount){
                snake.growCount--;
            } else{
                moveSnakeTail();
            }
            
			// Змея не останавливается
            isSnakeStop = SNAKE_NOT_STOP;
            break;
        
			// Змея врезалась в стену или тело (конец игры)
        case WALL_OR_BODY_PX:
            isSnakeStop = SNAKE_STOP;
            break;
        
        // Змея съела еду
        case FOOD_PX:
			// Увеличиваем счёт
            score += (food.isBig ? 3 : 1);
			
			// Увеличиваем длину змеи
            snake.length += (food.isBig ? 3 : 1);
            
			// Генерируем новую еду
            genFood();
			
			// Проверяем счёт
            isSnakeStop = (score == winScore ? SNAKE_STOP : SNAKE_NOT_STOP);
            break;
    }
}

//
//------- Функции работы с меню -------
/* Рисуем иконки змей */
void writeSnakePic(void)
{
    displayBuffer[31] |= 0x14;
    displayBuffer[32] |= 0x08;
    displayBuffer[33] |= 0x08;
    displayBuffer[34] |= 0x08;
    displayBuffer[35] |= 0x08;
    displayBuffer[36] |= 0x08;
    displayBuffer[37] |= 0x08;
    displayBuffer[38] |= 0x08;
    displayBuffer[39] |= 0x1C;
    displayBuffer[40] |= 0x1C;        

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

/* Рисуем меню выбора сложности игры */
void drawDiffMenu(void)
{
	// Рисуем иконки змей
    writeSnakePic();
	
    // Устанавливаем позицию курсора и рисуем надписи
    lcdStruct.byteIndex = 42;
    lcdStruct.writeStringToBuffer("Змейка");
    lcdStruct.byteIndex = (DISPLAY_HEIGHT >=64 ? DISPLAY_WIDTH*3 + 12 : DISPLAY_WIDTH*2 + 12);
    lcdStruct.writeStringToBuffer("Выберите сложность");
    lcdStruct.byteIndex = (DISPLAY_HEIGHT >=64 ? DISPLAY_WIDTH*5 + 12 : DISPLAY_WIDTH*3 + 12);
	
	// В зависимости от выбранной сложности рисуем стрелку
    switch(gameMode)
	{
        case GAME_EASY_MODE:
            lcdStruct.writeStringToBuffer(">легко  сред  слож");
            break;
        case GAME_MID_MODE:
            lcdStruct.writeStringToBuffer(" легко >сред  слож");
            break;
        case GAME_HARD_MODE:
            lcdStruct.writeStringToBuffer(" легко  сред >слож");
    }
    lcdStruct.displayFullUpdate();
}

/* Цикл выбора сложности игры */
void difficultySelection(void)
{
	// Очищаем дисплей и обнуляем сложность
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    gameMode = 0;
	
	// Рисуем еню
    drawDiffMenu();
    
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
		// Двигаемся к более сложному режиму
        if(isButtonPressed(BUTTON_LEFT)){
            delayUs(10000);
            gameMode -= (gameMode == 0 ? 0 : 1);
            drawDiffMenu();
        }
		
		// Двигаемся к более простому режиму
        if(isButtonPressed(BUTTON_RIGHT)) {
            delayUs(10000);
            gameMode += (gameMode == 2 ? 0 : 1);
            drawDiffMenu();
        }
		
		// Выбор режима
        if(isButtonPressed(BUTTON_SELECT)){
			delayUs(10000);
			lcdStruct.clearOrFillDisplay(CLEAR);
			return;
		}
        delayUs(100000);
    }
}
/* Перерисовка меню выбора мелодии */
void drawSongMenu(void)
{
	// Рисуем надписи
    lcdStruct.byteIndex = 20;
    lcdStruct.writeStringToBuffer("Включить музыку?");
    lcdStruct.byteIndex = DISPLAY_WIDTH*2 + 30;
    switch(isSnakeSong){
        case 0:
            lcdStruct.writeStringToBuffer(">нет  да");
            break;
        case 1:
            lcdStruct.writeStringToBuffer(" нет >да");
            break;
    }
    lcdStruct.displayFullUpdate();
}
/* Меню выбора проигрывать ли музыку */
void songSelect(void)
{
	// Очищаем буфер и перерисовываем меню
    memset(displayBuffer, 0x00, LCD_BUFFER_LENGTH);
    isSnakeSong = 0;
    drawSongMenu();
    
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
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
/* Главная функция игры */
void startSnakeGame(void)
{
	// Выбор сложности и мелодии
    difficultySelection();
    songSelect();
    
    // Инициализация игры
    gameInit();
    
    // Проигрывание музыки
    if(isSnakeSong) {
        playBackgroundSong(SPEAKER_TYPE_BIG, SnakeGameSong, SnakeGameSong_Beats, sizeof(SnakeGameSong)/2, 200, 1);
    }
    
    // Новое направление змеи
    Direction newDir = snake.newDir;
	
	// Главный цикл игры
    while(!isSnakeStop)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
        // Получение нового направления змеи от пользователя
		if(isButtonPressed(BUTTON_TOP)){
			if(gameMode == GAME_EASY_MODE){
				// Защита, чтобы змея не врезалась в себя при повороте и не могла поворачивать на границе экрана
				if( snake.head.col < DISPLAY_WIDTH-2 && snake.head.col > 1 && 
					((abs(snake.head.col - snake.tail.col)>=4  || snake.head.col == snake.tail.col ) || snake.head.row == snake.tail.row)){
					newDir = UP;
				}
			} else{
				newDir = UP;
			}
		}
		
        if(isButtonPressed(BUTTON_BOTOOM)){
            if(gameMode == GAME_EASY_MODE){
                if( snake.head.col < DISPLAY_WIDTH-2 && snake.head.col > 1 && 
                    ((abs(snake.head.col - snake.tail.col)>=4 || snake.head.col == snake.tail.col) || snake.head.row == snake.tail.row)){
                     newDir = DOWN;
                }
            } else{
                newDir = DOWN;
            }
        }
        if(isButtonPressed(BUTTON_LEFT)){
            if(gameMode == GAME_EASY_MODE){
                if( snake.head.row < DISPLAY_HEIGHT-2 && snake.head.row > 1 && 
                    ((abs(snake.head.row - snake.tail.row)>=4 || snake.head.row == snake.tail.row) || snake.head.col == snake.tail.col)){
                     newDir = LEFT;
                }
            } else{
                newDir = LEFT;
            }
        }
		
        if(isButtonPressed(BUTTON_RIGHT)){
            if(gameMode == GAME_EASY_MODE){
                if( snake.head.row < DISPLAY_HEIGHT-2 && snake.head.row > 1 && 
                    ((abs(snake.head.row - snake.tail.row)>=4 || snake.head.row == snake.tail.row) || snake.head.col == snake.tail.col)){
                     newDir = RIGHT;
                }
            } else{
                newDir = RIGHT;
            }
        }
		
		// Выход из игры и остановка музыки
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                songStop();
                lcdStruct.clearOrFillDisplay(CLEAR);

                return;
            }
        #endif
            
		// Пауза в игре, до тех пор, пока не будет нажата кнопка
        #ifdef BUTTON_SELECT
            if(isButtonPressed(BUTTON_SELECT))
			{
				// Пиксель в углу экрана для индикации
                displayBuffer[0] = (gameMode == GAME_EASY_MODE ? 0x01 : 0xFE);
                lcdStruct.displayFullUpdate();
				
				// Приостановка музыки
                songPause();
                delayUs(1000000);
                
				// Ожидание нажатия кнопки
                while(!isButtonPressed(BUTTON_SELECT)){irProtocolAnalyze();}
				
				// Воспроизведение музыки
				if(isSnakeSong) {
					song.isSongPlay = 1;
				}
				
                displayBuffer[0] = (gameMode == GAME_EASY_MODE ? 0x00 : 0xFF);
                lcdStruct.displayFullUpdate();
                delayUs(500000);
            }
        #endif
        
		// Меняем направление змеи только, если оно не противоречиво
        if(newDir != ((~snake.newDir) & 0x03)){
            snake.newDir = newDir;
        }
            
        // двигаем и рисуем змею
        snakeMove();
        lcdStruct.displayFullUpdate();
    }
    
    //-----------Конец игры--------
	// Останавливаем музыку
    songStop();
	
    // Рисуем сообщение о проигрыше или выиграше
    lcdStruct.byteIndex = DISPLAY_WIDTH + 20;
    lcdStruct.writeStringToBuffer((score != winScore ? "Вы проиграли!" : "Вы выиграли!"));
    
    // Рисуем счёт
    char receiveString[5];
    sprintf(receiveString, "%05u", score);
    lcdStruct.byteIndex = DISPLAY_WIDTH*2 + 20;
    lcdStruct.writeStringToBuffer("Счёт: ");
    lcdStruct.writeStringToBuffer(receiveString);
    
    lcdStruct.displayFullUpdate();
    
    // Ждём и возвращаемся в главное меню
    delayUs(2500000);
    lcdStruct.clearOrFillDisplay(CLEAR);
}
