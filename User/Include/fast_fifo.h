/** Using file encoding - UTF8
  * @file    fast_fifo.h
  * @author  Shalaev Egor
  * @version V0.0.1
  * @date    13-June-2022
  * @brief   Очень быстрое FIFO на макросах 
  * ===========================
  * Как использовать fifo:
  * --------------------------------
  * * Если fifo надо использовать только в одном файле:
  * 1. Объявить fifo с помощью макроса FIFO_SHORT_DEFINITION(_type, _size, _id), 
  *    указав тип, размер и идентификатор в файле .c (в заголовочном файле использовать этот макрос нельзя!!!)
  * --------------------------------
  * * Если fifo надо использовать в нескольких файлах
  * 1. Объявить структуру с помощью макроса FIFO_STRUCT_DECLARATION(_type, _size, _structName) в заголовочном файле,
  *    передав тип даннхы, размер и имя объявляемой структуры
  * 2. В этом же заголовочном файле обяъвить само fifo FIFO_EXTERN_DECLARATION(_structName, _id), 
  *    передав то же имя структуры и произвольный идентификатор fifo
  * 3. В .c файле определить fifo с помощью макроса FIFO_DEFINITION(_structName, _id), 
  *    передав то же самое имя структуры и тот же идентификатор fifo.
  * --------------------------------
  * Дальнейшее использование не зависит от того, используется ли fifo в одном или нескольких файлах.
  * Простое помещение элемента в fifo: FIFO_PUSH(_id)
  * Простое вытаскивание элемента из fifo: FIFO_POP(_id)
  * --------------------------------
  * Особенности:
  * 1. Fifo может быть произвольного типа данных (указывается при создании).
  * 2. Можно создавать произвольное количество fifo, как для использования в одном файле, так и в нескольких.
  * 3. Максимальный размер fifo на данный момент: 65535. Чтобы его увеличить, 
  *    надо изменить тип переменных popIndex и pushIndex.
  * 4. Объявление fifo для нескольких файлов пришлось разнести на 3 макроса, чтобы была возможность 
  *    создавать несколько глобальных fifo с разными размерами буфера.
  * =========================== */
#ifndef __FAST_FIFO_H
#define __FAST_FIFO_H

/* Короткое объявление и определение для использования только в одном файле (только для .c файлов)
 * _type - тип данных внутреннего массива (uint8_t, uint16_t и т.д.)
 * _size - размер fifo в байтах (должен быть кратен 2^n)
 * _id - идентификатор конкретного fifo
 *---------------------
 * Поля струткуры:
 * popIndex - индекс элемента, по которому будет производиться чтение
 * pushIndex - индекс элемента, по которому будет производиться запись 
 * data - массив с данными */
#define FIFO_SHORT_DEFINITION(_type, _size, _id) \
                               struct {uint16_t popIndex; uint16_t pushIndex; _type data[_size];} _id = {0,0, {0}}
							   
/* Создание структруты fifo (для использования в нескольких файлах, вызывается в .h файле)
 * Необходимо, чтобы можно было создавать несколько структур с различными размерами массивов.
 * _type - тип данных внутреннего массива (uint8_t, uint16_t и т.д.)
 * _size - размер fifo в байтах (должен быть кратен 2^n)
 * _structName - идентификатор структуры fifo
 *---------------------
 * Поля струткуры:
 * popIndex - индекс элемента, по которому будет производиться чтение
 * pushIndex - индекс элемента, по которому будет производиться запись 
 * data - массив с данными */
#define FIFO_STRUCT_DECLARATION(_type, _size, _structName) \
                         typedef struct {uint16_t popIndex; uint16_t pushIndex; _type data[_size];} _structName

/* Определение fifo в глобальное области (для использования в нескольких файлах, вызывается в .h файле)					 
 * _structName - идентификатор структуры fifo
 * _id - идентификатор fifo */
#define FIFO_EXTERN_DECLARATION(_structName, _id) extern _structName _id;
						 
/* Определение fifo (для использования в нескольких файлах, вызывается в .c файле)
 * _structName - идентификатор структуры fifo
 * _id - идентификатор fifo */
#define FIFO_DEFINITION(_structName, _id) _structName _id = {0,0,{0}}

/* Маска для размеров fifo */
#define FIFO_MASK(_id)  ((sizeof(_id.data)/sizeof(_id.data[0]))-1)

/* Текущий уровень заполнености fifo (сколько элементов ещё можно прочитать) */
#define FIFO_ELEMENTS_COUNT(_id)    ((_id.pushIndex - _id.popIndex) & FIFO_MASK(_id))

/* Количество свободных элементов в fifo (сколько элементов ещё можно записать) */
#define FIFO_FREE_COUNT(_id)   ((_id.popIndex-_id.pushIndex) & FIFO_MASK(_id))

/* Вернет 1, если fifo заполнено и 0, если оно не полно */
#define FIFO_IS_FULL(_id)      (_id.pushIndex == ((_id.popIndex-1) & FIFO_MASK(_id)))

/* Вернет 1, если fifo пусто и 0, если оно не пусто */
#define FIFO_IS_EMPTY(_id)     (_id.pushIndex == _id.popIndex)

/* Очистка fifo (сами элементы fifo не меняются, только лишь зануляются указатели) */
#define FIFO_FLUSH(_id)         do{_id.popIndex=0; _id.pushIndex=0;} while (0)

/* Возвращает следующий элемент из fifo и удаляет его из памяти (не защищенный режим)
 * Должно вызываться только тогда, когда fifo не пусто */
#define FIFO_POP(_id)                                           \
({                                                              \
    __typeof__(_id.data[0]) _return = _id.data[_id.popIndex];  \
    _id.popIndex = (_id.popIndex+1) & FIFO_MASK(_id);         \
    _return;                                                    \
})

/* Возвращает следующий элемент из fifo и удаляет его из памяти (защищенный режим) 
 * Если fifo пустое, то возвращает последний элемент */
#define FIFO_POP_SAFE(_id)                                      \
({                                                              \
    __typeof__(_id.data[0]) _return;                            \
    if(FIFO_IS_EMPTY(_id))                                      \
        _return = _id.data[(_id.popIndex-1) & FIFO_MASK(_id)]; \
    else                                                        \
    {                                                           \
        _return = _id.data[_id.popIndex];                      \
        _id.popIndex = (_id.popIndex+1) & FIFO_MASK(_id);     \
    }                                                           \
    _return;                                                    \
})

/* Добавляет элемент в fifo (не защищенный режим) 
 * Должно вызываться только тогда, когда fifo не полно */
#define FIFO_PUSH(_id, newdata)                                 \
do{                                                             \
    _id.data[(_id.pushIndex) & FIFO_MASK(_id)] = (newdata);    \
    _id.pushIndex = (_id.pushIndex+1) & FIFO_MASK(_id);       \
}while(0)

/* Добавляет элемент в fifo (защищенный режим) 
 * Если fifo полное, то элемент будет потерян */
#define FIFO_PUSH_SAFE(_id, newdata)                                      \
do{                                                                       \
    __typeof__(_id.pushIndex) _next = (_id.pushIndex+1)&FIFO_MASK(_id); \
    if(_next != _id.popIndex)                                            \
    {                                                                     \
        _id.data[(_id.pushIndex) & FIFO_MASK(_id)] = (newdata);          \
        _id.pushIndex = _next;                                           \
    }                                                                     \
}while(0)

/*------ Прочие функции для работы с fifo ------*/
/* Позволяет прочитать элемент из fifo, не удаляя его
*  _id:     идентификатор fifo
*  index:   Смещение относительно первого элемента в fifo */
#define FIFO_READ_DATA(_id, index)   _id.data[(_id.popIndex+(index))&MASK_MAX_ELEMENTS(_id)]

/* Удаляет из fifo указанное количество элемнтов
 * Должно использоваться только тогда, когда в fifo достаточно элементов для удаления */
#define FIFO_REMOVE_ELEMENTS(_id, amount)  (_id.popIndex = (_id.popIndex+(amount)) & FIFO_MASK(_id))

/* Безопасное удаление указанного количества элементов или меньше из fifo */
#define FIFO_REMOVE_SAFE(_id, amount)                           \
do{                                                             \
    if(COUNT_ELEMENTS(_id) >= (amount))                         \
        FIFO_REMOVE(_id, (amount));                             \
    else                                                        \
        FIFO_FLUSH(_id);                                        \
}while(0)*/
#endif
