#ifndef _UARRAY2_T_
#define _UARRAY2_T_
/*
 *  uarray2.h
 *  Alec Plano and Joseph Montalto
 *  2/5/2023
 *
 *  CS40 HW2 Part A, A program to create and manipulate a 2D unboxed array.
 *
 *  A file to outline the UArray2_T class.
 * 
 */


#include <uarray.h>
#define T UArray2_T
typedef struct T *T;


struct T {
        int width;
        int height;
        int elem_size;
        UArray_T array;
};


extern T UArray2_new(int width, int height, int ELEMENT_SIZE);

extern void UArray2_free(T *uarr);

extern int UArray2_width(T uarr);

extern int UArray2_height(T uarr);

extern int UArray2_size(T uarr);

extern void *UArray2_at(T uarr, int col, int row);

extern void UArray2_map_col_major(T uarr, 
                void apply(int i, int j, UArray2_T a, void *p1, void *cl),
                void *cl);
                
extern void UArray2_map_row_major(T uarr, 
                void apply(int i, int j, UArray2_T a, void *p1, void *cl), 
                void *cl);



#undef T
#endif