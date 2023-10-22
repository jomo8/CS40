/*
 * uarray2b.c
 * by Sam Reed and Joey Montalto
 * 2/22/23
 * locality
 *
 * uarray2b.c creates a 2d blocked array, where blocks are stored by single 
 *      contiguous UArrays, which are stored in a UArray2.
 * 
 * Can be initialized with either UArray2b_new() with a given blocksize, or 
 *      UArray2b_new64K_block(), which initialized a UArray2b with a blocksize
 *       that is as close to 64Kb as possible.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uarray2.h"
#include "uarray.h"
#include "mem.h"
#include "assert.h"
#include "uarray2b.h"
#include "except.h"

const Except_T runtime_error = { "Checked Runtime Error" };

#define T UArray2b_T

struct T {
        int width, height, size, blocksize, blockwidth, blockheight;
        UArray2_T blocks;
};

struct small_closure {
        void (*apply)(int size, int blocksize);
        void *cl;
};


/********** UArray2b_new ********
 *
 * Initializes a new UArray2b with the given deminsions.
 * Inputs:
 *       int height: the height of the 2d array
 *       int width: the width of the 2d array
 *       int size: the size of the elements to be stored in the array
 *       int blocksize: the size of each block in the 2d array
 * Return: The fully initialized UArray2b_T
 * Expects
 *    blocksize is a positive integer.
 ************************/
extern T UArray2b_new (int width, int height, int size, int blocksize)
{
        assert (blocksize >= 0);

        T array;
        NEW(array);
        array->width = width;
        array->height = height;
        array->size = size;
        array->blocksize = blocksize;
        array->blockwidth = width / blocksize;

        array->blockwidth = (width / blocksize);
        if (width % blocksize != 0) {
                array->blockwidth  ++;
        } 
        array->blockheight = (height / blocksize);
        if (height % blocksize != 0) {
                array->blockheight ++;
        }

        array->blocks = UArray2_new(array->blockwidth, array->blockheight, 
                                                        sizeof(UArray_T));

        for (int col = 0; col < array->blockwidth; col++) {
                for (int row = 0; row < array->blockheight; row++) {
                        *(UArray_T *) UArray2_at(array->blocks, col, row) = 
                                UArray_new(blocksize * blocksize, size);
                }
        }
        
        return array;
}

/********** UArray2b_new ********
 *
 * Initializes a new UArray2b with the given deminsions and the max blocksize
 * that can fit into 64kb (assumed to be 64 * 1024 bytes.)
 * Inputs:
 *       int height: the height of the 2d array
 *       int width: the width of the 2d array
 *       int size: the size of the elements to be stored in the array
 * Return: The fully initialized UArray2b_T
 * Expects
 *    size should be non-zero
 ************************/
extern T UArray2b_new_64K_block(int width, int height, int size)
{
        assert (size >= 0);
        return UArray2b_new(width, height, size, sqrt((64 * 1024) / size));
}

/********** UArray2b_free ********
 *
 * Frees the given UArray2b as well as all of the memory allocated by it
 * Inputs:
 *       T *array2b : a pointer to the UArray2b to be freed
 * Return: void
 * Expects
 *    array2b should be an initialized Array2b
 ************************/
extern void UArray2b_free (T *array2b)
{
        if (array2b == NULL) {
                RAISE(runtime_error);
        }
        for (int i = 0; i < (*array2b)->blockwidth; i++){
                for (int j = 0; j < (*array2b)->blockheight; j++){
                        UArray_free(UArray2_at((*array2b)->blocks, i, j));
                }
        }
        UArray2_free(&(*array2b)->blocks);
        FREE(*array2b);
}

/********** UArray2b_width ********
 *
 *  Getter function for the width of the 2d array
 * Inputs:
 *       T array2b : the array to get the width from
 * Return: Int with the width of the array
 * Expects
 *    Array2b should be initalized
 ************************/
extern int UArray2b_width (T array2b)
{
        if (array2b == NULL) {
                RAISE(runtime_error);
        }
        return array2b->width;
}

/********** UArray2b_height ********
 *
 *  Getter function for the height of the 2d array
 * Inputs:
 *       T array2b : the array to get the height from
 * Return: Int with the height of the array
 * Expects
 *    Array2b should be initalized
 ************************/
extern int UArray2b_height (T array2b)
{
        if (array2b == NULL) {
                RAISE(runtime_error);
        }
        return array2b->height;
}

/********** UArray2b_size ********
 *
 *  Getter function for the size of the elements in the UArray2b
 * Inputs:
 *       T array2b : the array to get the size from
 * Return: Int with the size of elements in the array
 * Expects
 *    Array2b should be initalized
 ************************/
extern int UArray2b_size (T array2b)
{
        if (array2b == NULL) {
                RAISE(runtime_error);
        }
        return array2b->size;
}


/********** UArray2b_blocksize ********
 *
 *  Getter function for the blocksize of the 2d array
 * Inputs:
 *       T array2b : the array to get the blocksize from
 * Return: int with the blocksize of the array
 * Expects
 *    Array2b should be initalized
 ************************/
extern int UArray2b_blocksize(T array2b)
{
        if (array2b == NULL) {
                RAISE(runtime_error);
        }
        return array2b->blocksize;
}


/********** UArray2b_at ********
 *
 *  Gets the element at a given index from a UArray2b.
 * Inputs:
 *       T array2b : the array to get the blocksize from.
 *       int column: The column of the desired element.
 *       int row: The row of the desired element.
 * Return: A pointer to the element at the given index.
 * Expects
 *    array2b to be a nonnull value, and the indices to be within range.
 ************************/
extern void *UArray2b_at(T array2b, int column, int row)
{
        if (column < 0 || column >= array2b->width || row < 0 
           || row >= array2b->height) {
                RAISE(runtime_error);
        }
        assert(array2b != NULL);

        int blocksize = array2b->blocksize;
        
        return UArray_at(*(UArray_T *) UArray2_at(array2b->blocks, 
                column/blocksize, row/blocksize), blocksize * (row % blocksize)
                + (column % blocksize));
        
}


/********** UArray2b_map ********
 *
 *  Maps through each element in the UArray2 and calls the given apply function
 * on each
 * Inputs:
 *       T array2b : the array to be mapped through
 *       void apply: a function pointer to be called on every element
 *       void *cl: void pointer to the closure (to be passed into apply)
 * Return: void
 * Expects
 *    Array2b should be initalized
 ************************/
extern void UArray2b_map(T array2b,
void apply(int col, int row, T array2b, void *elem, void *cl), void *cl)
{
        if (array2b == NULL) {
                RAISE(runtime_error);
        }

        int blocksize = array2b->blocksize;

        for (int i = 0; i < array2b->blockheight; i++) {
                for (int j = 0; j < array2b->blockwidth; j++) {

                        UArray_T temp = *(UArray_T *) 
                                UArray2_at(array2b->blocks, j, i);

                        for (int o = 0; o < UArray_length(temp); o++){
                                if (j * blocksize + (o % blocksize) < 
                                        array2b->width && i * blocksize +  
                                        (o / blocksize) < array2b->height) { 
                                        apply(j * blocksize + (o % blocksize), 
                                        i * blocksize +  (o / blocksize), 
                                        array2b, UArray_at(temp, o), cl);
                                }
                        }
                }
        }
}
#undef T