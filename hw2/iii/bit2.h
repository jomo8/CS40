#ifndef _BIT2_H_
#define _BIT2_H_
/*
 *  bit2.h
 *          Alec Plano and Joseph Montalto
 *          2/5/2023
 *
 *  CS40 HW2 Part B A program to create and manipulate a 2D array of bits.
 *
 *  A file to outline the Bit2_T class.
 * 
 */

#include <stdio.h>
#include "bit.h"
#define T Bit2_T
typedef struct T *T;

struct T {
        int width;
        int height;
        int num_one;
        Bit_T array;
};


extern T Bit2_new(int height, int width);

extern void Bit2_free(T *arr);

extern int Bit2_width(T arr);

extern int Bit2_height(T arr);

extern int Bit2_put(T arr, int col, int row, int bit);

extern int Bit2_get(T arr, int col, int row);

extern void Bit2_map_col_major(T arr, void apply(int i, int j, T arr, int b,
                               void *p1), void *p1);

extern void Bit2_map_row_major(T arr, void apply(int i, int j, T arr, int b,
                               void *p1), void *p1);

extern int getNumOnes(T arr);


#undef T
#endif