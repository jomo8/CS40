/*
 * Interface for the segmented memory of the UM implementation 
 *
 */

#include <stdint.h>
#include "seq.h"

#ifndef MEMORY_H_
#define MEMORY_H_

/* Pointer to a struct that contains the data structure for this module */
/* Struct definition of a Memory_T which 
   contains two sequences: 
   - one holding pointers to UArray_T's representing segments
   - one holding pointers to uint32_t's representing free segments */
typedef struct {
        uint32_t *words;
        uint32_t numWords;
}Row_T;

typedef struct {
        Row_T *segments;
        uint32_t colNum;
        uint32_t fullIndex;
        Seq_T free;
}Memory_T;


/* Creates/frees memory associated with a Memory_T */
extern Memory_T *memory_new(uint32_t length);
extern void memory_free(Memory_T *m);

/* Allows user to interact with struct Memory_T data */
extern void memory_put(Memory_T *m, uint32_t seg, uint32_t off, uint32_t val);
extern uint32_t memory_get(Memory_T *m, uint32_t seg, uint32_t off);

/* Maps and Unmaps segments to struct Memory_T sequence */
extern uint32_t memory_map(Memory_T *m, uint32_t length);
extern void memory_unmap(Memory_T *m, uint32_t seg_num);



/* I added these */
extern void expand_program(Memory_T *m);
void edit_free_array(Memory_T *m, uint32_t index, int expand);

#endif