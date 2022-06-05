#ifndef HEADER_GUARD_FIFO
#define HEADER_GUARD_FIFO

#define FIFO_SIZE(data)   (sizeof(data)/sizeof(data[0]))

/* creates and initializes an anonymous _fifofast_t structure.
*  _id:     C conform identifier
*  _depth:  maximum amount of elements, which can be stored in the FIFO. The value must be 2^n,
*           n=2..8 for the normal version.
*           The actual depth is always 1 count less than specified in as this byte
*           is required to distinguish a "full" and "empty" state*/
#define FIFO_STRUCT_CREATE(size) \
    struct MYFIFO{uint16_t read; uint16_t write; uint8_t data[size];}
typedef struct MYFIFO MYFIFO;
    
#define FIFO_CREATE(_id) MYFIFO _id = {0,0,{0}}
    
#define FIFO_MASK(_id)  (FIFO_SIZE(_id.data)-1)

// current fill level of the fifo (the amount of elements that can be read)
#define FIFO_COUNT_ELEMENTS(_id)    ((_id.write - _id.read) & FIFO_MASK(_id))

// returns the current free space of the fifo (the amount of elements that can be written)
#define FIFO_FREE_COUNT(_id)   ((_id.read-_id.write-1) & FIFO_MASK(_id))

#define FIFO_IS_FULL(_id)      (_id.write == ((_id.read-1) & FIFO_MASK(_id)))

#define FIFO_IS_EMPTY(_id)     (_id.write == _id.read)

// flushes/ clears buffer completely
// _id:     C conform identifier
#define FIFO_FLUSH(_id)         do{_id.read=0; _id.write=0;} while (0)

// returns the next element from the fifo and removes it from the memory
// MUST be used only when fifo is NOT empty, useful for repeated reads
#define FIFO_POP(_id)                                           \
({                                                              \
    __typeof__(_id.data[0]) _return = _id.data[_id.read];       \
    _id.read = (_id.read+1) & FIFO_MASK(_id);                   \
    _return;                                                    \
})

// returns the next element from the fifo and removes it from the memory
// If no elements are stored in the fifo, the last one is repeated.
#define FIFO_POP_SAFE(_id)                                      \
({                                                              \
    __typeof__(_id.data[0]) _return;                                \
    if(FIFO_IS_EMPTY(_id))                                      \
        _return = _id.data[(_id.read-1) & FIFO_MASK(_id)];      \
    else                                                        \
    {                                                           \
        _return = _id.data[_id.read];                           \
        _id.read = (_id.read+1) & FIFO_MASK(_id);               \
    }                                                           \
    _return;                                                    \
})

// adds an element to the fifo
// MUST be used only when fifo is NOT full, useful for repeated writes
#define FIFO_PUSH(_id, newdata)                                 \
do{                                                             \
    _id.data[(_id.write) & FIFO_MASK(_id)] = (newdata);         \
    _id.write = (_id.write+1) & FIFO_MASK(_id);                 \
}while(0)

// adds an element to the fifo
// If fifo is full, the element will be dismissed instead
#define FIFO_PUSH_SAFE(_id, newdata)                            \
do{                                                             \
    __typeof__(_id.write) _next = (_id.write+1)&FIFO_MASK(_id);     \
    if(_next != _id.read)                                       \
    {                                                           \
        _id.data[_next] = (newdata);                            \
        _id.write = _next;                                      \
    }                                                           \
}while(0)

// other functions (remove and add)
/* allows accessing the data of a fifo as an array without removing any elements
*  _id:     C conform identifier
*  index:   Offset from the first element in the buffer
//#define DATA(_id, index)   _id.data[(_id.read+(index))&MASK_MAX_ELEMENTS(_id)]

// removes a certain number of elements
// MUST be ONLY used when enough data to remove is in the buffer!
#define FIFO_REMOVE(_id, amount)        (_id.read = (_id.read+(amount)) & FIFO_MASK(_id))

// removes a certain number of elements or less, if not enough elements is available
#define FIFO_REMOVE_SAFE(_id, amount)                           \
do{                                                             \
    if(COUNT_ELEMENTS(_id) >= (amount))                         \
        FIFO_REMOVE(_id, (amount));                             \
    else                                                        \
        FIFO_FLUSH(_id);                                        \
}while(0)*/
#endif
