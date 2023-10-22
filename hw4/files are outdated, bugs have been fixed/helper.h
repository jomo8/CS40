#ifndef HELPER_INCLUDED
#define HELPER_INCLUDED

#include "a2blocked.h"
#include "uarray.h"
#include <stdint.h>


#define C colorSpace
typedef struct C {
        float y, pb, pr;
} C;


/* This is for an abstraction barrier */
// typedef struct W *W;
#define W wordElements
typedef struct W {
        unsigned a;
        int8_t   b, c, d;
        uint8_t  pBavg, pRavg;
} W;

// #define W wordElements
/* Change name of this */
typedef struct dct_struct {
        UArray_T block;
        A2Methods_T methods;
        /* Change this to "arr" */
        A2Methods_UArray2 dct; 
} dct_struct;


/* Can prolly improve function names */
void copy_pixels(int col, int row, A2Methods_UArray2 trimmed, 
                A2Methods_Object *pixel, void *closure);
void rgb_to_comp_map(int col, int row, A2Methods_UArray2 trimmed, 
                        A2Methods_Object *pixel, void *closure);
void comp_to_rgb_map(int col, int row, A2Methods_UArray2 trimmed, 
                        A2Methods_Object *pixel, void *closure);

void comp_to_dct(int col, int row, A2Methods_UArray2 comp_video, 
                                A2Methods_Object *pixel, void *closure);
void dct_to_comp(int col, int row, A2Methods_UArray2 inverse_dct, 
                                A2Methods_Object *pixel, void *closure);
void store_words(int col, int row, A2Methods_UArray2 dct, 
                                A2Methods_Object *pixel, void *closure);
void word_to_dct(int col, int row, A2Methods_UArray2 tmp, 
                                A2Methods_Object *word, void *closure);
void dct_to_word(int col, int row, A2Methods_UArray2 tmp, 
                                A2Methods_Object *fields, void *closure);
void print_words(int col, int row, A2Methods_UArray2 wordArray, 
                        A2Methods_Object *word, void *closure);
void word_to_string(uint32_t word, char *str);
uint32_t build_word(W fields);

int roundBounds(float num, float maxVal);
W perform_dct(UArray_T block);
C inverse_dct(W word, int index);
float index_to_coefficient(int index);
signed coefficient_to_index(float var);
W unpack_word(uint32_t input);


#undef C
#undef W
#endif