/** Using UTF8 file encoding
  * @file    game_2048.c
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    26-June-2022
  * @brief   Эта библиотека реализует игру 2048. 
  * =====================
  * В игре присутствует поле 4х4, в котором могут располагаться цифры кратные двойки.
  * Игра начианется с двух цифр (из 2-к или 4-к). Все цифры можно сдвигать в одно из четырёх направлений.
  * При этом, если сдвинулась хотя бы одна цифра, то появляется новая 2 или 4.
  * Если при сдвиге в одном столбце (или строке) находились одинаковые цифры, то они заменяются 
  * одной цифрой равной их сумме.
  * Победа достигается тогда, когда набирается 2048.
  */
  
#include "game_2048.h"

/*============================== Структуры и переменные ==============================*/
// Структура со всеми цифрами и максимальной цифрой
static Matrix gameMatrix = {{0x00}, 4};

// Возможные направления сдвига
typedef enum {
    UP,
    LEFT, 
    RIGHT,
    DOWN,
} Direction;

//текущий счёт
static uint16_t score = 0;

// Библиотека с закодированными изображениями цифр (шрифт 3х5)
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
/*============================== Функции ==============================*/
/* Инициализация игры */
void gameInit(void)
{
	// Значения по-умолчанию - 0
    for(uint16_t i = 0; i < LCD_BUFFER_LENGTH; i++){
        displayBuffer[i] = 0x00;
        if(i < 16){
            gameMatrix.matrix[i] = 0x00;
        }
    }
	
	// Устанавливаем максимальное число и счёт
    gameMatrix.maxNumber = 2;
    score = 0;
    
    //---Записываем рамку поля в буфер---------------------------------------------------
    // Рисуем верхнюю и нижнюю границы
    for(uint16_t i = 1; i < FIELD_WIDTH - 1; i++){
        for(uint16_t j = 0; j < 3*DISPLAY_WIDTH; j += DISPLAY_WIDTH){
            displayBuffer[j + i] |= 0x01;
        }
        displayBuffer[DISPLAY_WIDTH*3 + i] |= 0x81;
    }
    
    // Записываем боковые границы
    for(uint16_t i = 0; i < 4*DISPLAY_WIDTH; i += DISPLAY_WIDTH){
        for(uint16_t j = 0; j < 5*CELL_WIDTH; j += CELL_WIDTH){
            displayBuffer[i+j]  |= 0xFF;
        }
    }
    
    // Генерируем два первых числа
    genNewNumber();
    genNewNumber();

    // Записываем матрицу в буфер дисплея
    writeMatrixToField();
	
	// Записываем счёт в буфер дисплея и отрисовываем его
    writeScore();
    LCD_DisplayFullUpdate();
}

/* Запись счёта в буфер*/
void writeScore(void)
{
	// Запись фразы "счёт"
    lcdStruct.byteIndex = FIELD_WIDTH + 1;
    LCD_WriteStringToBuffer("Счёт:");
    
	// Запись самого счёта в виде строки
    char receiveString[5];
    sprintf(receiveString, "%05u", score);
    lcdStruct.byteIndex = FIELD_WIDTH+DISPLAY_WIDTH + 1;
    LCD_WriteStringToBuffer(receiveString);
}

/* Запись опредеоенное число по таким-то координатам поля
 * number: число, которое надо записать
 * row: горизонтальная координата поля (верхняя левая ячейка - 0, 0 )
 * col: вертикальная коодината поля */
void writeNumberToField(uint16_t number, uint8_t row, uint8_t col)
{
    // Индекс байта, в который надо записать новое число или пробел
    uint16_t index = row*DISPLAY_WIDTH + col*CELL_WIDTH + 1;
    
    // Очистка ячеек
    for(uint8_t i = 0; i < 16; i++){
        displayBuffer[index + i] &= (row == 3 ? 0x81: 0x01);
    }
	
    // Если элемент равен нулю
    if(number == 0) return;
    
    // Вычисляем цифры числа
    uint8_t digits[4] = {number/1000, (number/100)%10, (number/10)%10, number%10};
    uint8_t startDigit = 0;
    
    // Вычисляем смещение координат для стартового пробела и первой цифры числа
    if(digits[0] == 0) {
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
    
    // Записываем цифры в буфер
    for(uint8_t i = 0; i < 4 - startDigit; i++){
        for(uint8_t j = 0; j < 4; j++){
            displayBuffer[index + i*4 + j] |= NUMBERS_2048[digits[i+startDigit]*4+j];
        }
    }
}

/* Записываем всю матрицу в буфер дисплея */
void writeMatrixToField(void)
{
    for(uint16_t i = 0; i < 16; i++){
        writeNumberToField(gameMatrix.matrix[i], i/4, i%4);
    }    
}

/* Получение свободного индекса для записи нового числа*/
uint8_t getAvailableIndex(void)
{
    // Массив со свободными индексами
    int availableCells[16] = {0};
    uint8_t availableCellsCount = 0;
    
    // Ищём свободное место
    for(uint16_t i = 0; i < 16; i++){
        availableCells[i] = (gameMatrix.matrix[i] == 0 ? availableCellsCount++, i : -1);
    }
    
    //Если поле заполнено
    if(!availableCellsCount){ return 250; }
    
	// Иначе генерируем новое положение
    uint8_t ind = (randLcg() >> 16) % 16U;
    
	// Двигаемся по матрице, пока не наёдм свободную клетку
    while(availableCells[ind] == -1){
        ind++;
        ind %= 16;
    }

    return ind;
}

/* Генерируем новое число на поле */
void genNewNumber(void)
{
	// Получаем смещение относительно начала
    uint8_t index = getAvailableIndex();
	
	// Записываем новое число в матрицу (вероятность 2 - 90%)
    if(index != 250){
        *(gameMatrix.matrix + index) = (randLcg() < MAX_RAND_NUM*0.9 ? 2 : 4);
    }
}

/* Сдвиг выбранного столбца (строки), а также проверка его на конец игры 
 * isGameEndCheck: если 1, то надо проверить на проигрыш, если 0, то просто сдвинуть
 * value1-value4: значения, образующие столбец или строку, которые надо сдвинуть 
 * @retval: 1 - проигрыш, 0 - успешный сдвиг*/
uint8_t moveColAndEndCheck(uint8_t isGameEndCheck, uint16_t *value1, uint16_t *value2, uint16_t *value3, uint16_t *value4)
{
    // Новые значения и счётчик сдвигов
    uint16_t values[4] = {*value1, *value2, *value3, *value4};
    uint8_t shiftCount = 0;
    
    // Сдвигаем всё
    for(uint8_t index = 1; index < 4; index++){
        if(values[index] == 0) continue;
        else {
            for(int in_index = index - 1; in_index >= 0; in_index--){
                // Двигаем значение, если там свободное место
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
    
    // Объединяем одинаковые цифры
    for(uint8_t index = 1; index < 4; index++){
        if(values[index] == 0) continue;
        // Складываем значения, если они одинаковы
        if((index !=0) && (values[index - 1] == values[index])){
            values[index - 1] *= 2;
            values[index] = 0;
            shiftCount++;
            // Обновляем счёт и максимальное число
            if(isGameEndCheck == 0){
                score += (isGameEndCheck == 1 ? 0 : values[index - 1]);
                gameMatrix.maxNumber = (values[index - 1] > gameMatrix.maxNumber ? values[index - 1] :  gameMatrix.maxNumber);
            }
        }
    }
    
    // Сдвигаем всё внось
    for(uint8_t index = 1; index < 4; index++){
        if(values[index] == 0) continue;
        else {
            for(int in_index = index - 1; in_index >= 0; in_index--){
                // Двигаем значение, если там свободное место
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
    
    // Если ничего не изменилось
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
/* Проверка окончания игры */
uint8_t checkGameEnd(void)
{
	// Если достигли выигрышного счёта
    if(gameMatrix.maxNumber == WIN_NUMBER) return GAME_WIN;
    
    uint8_t check = 0;
    
    // Проверка всех столбцов на возможность сдвига
    for(uint8_t i = 0; i < 4; i++){
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

/* Сдвиг матрицы и её отрисвовка
 * newDirection: новое направление сдвига*/
void moveAndDrawMatrix(uint8_t newDirection)
{
	// Количество сдвигов
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
    
	// Запись матрицы в поле, а также счёта
    writeMatrixToField();
    writeScore();    
    LCD_DisplayFullUpdate();
    
    // Генерация нового числа, если был сдвиг
    if(shiftCounts != 0){
        genNewNumber();
        writeMatrixToField();
        LCD_DisplayFullUpdate();
    }
}

/* Главная функция с игрой */
void startGame2048()
{
	// Инициализация игры
    gameInit();
    
    // Состояние игры
    uint8_t gameStatus = GAME_CONTINUE;
    while(1)
	{
		// Анализ ИК-протокола и выполнение команды
		irProtocolAnalyze();
		
		// Сдвиг матрицы вверх
        if(isButtonPressed(BUTTON_TOP)) {
			delayUs(buttonDelayUs);
            moveAndDrawMatrix(UP);
        }
		
		// Сдвиг матрицы вниз
        if(isButtonPressed(BUTTON_BOTOOM)) {
			delayUs(buttonDelayUs);
            moveAndDrawMatrix(DOWN);
        }
		
		// Сдвиг матрицы вправо
        if(isButtonPressed(BUTTON_RIGHT)) {
			delayUs(buttonDelayUs);
            moveAndDrawMatrix(RIGHT);
        }
		
		// Сдвиг матрицы влево
        if(isButtonPressed(BUTTON_LEFT)) {
			delayUs(buttonDelayUs);
            moveAndDrawMatrix(LEFT);
        }
        
		// Выход из игры
        #ifdef BUTTON_BACK
            if(isButtonPressed(BUTTON_BACK)){
                break;
            }
        #endif
        
		// Проверка на проигрыш
        if(gameStatus == GAME_CONTINUE){
            gameStatus = checkGameEnd();
        } else{
            lcdStruct.byteIndex = DISPLAY_WIDTH*5 + 20;
            LCD_WriteStringToBuffer((gameStatus == GAME_WIN ? "Вы выиграли!" : "Вы проиграли!" ));
            LCD_DisplayFullUpdate();
            delayUs(2000000);
            break;
        }
    }
}
