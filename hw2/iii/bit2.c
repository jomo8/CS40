/*
 *  bit2.c
 *          Alec Plano and Joseph Montalto
 *          2/5/2023
 *
 *  CS40 HW2 Part B A program to create and manipulate a 2D array of bits.
 *
 *  A file to define the Bit2_T class.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "bit.h"
#include <assert.h>
#define T Bit2_T


/***************** Bit2_new *******************
 *
 * Allocates a new 2D array with the given height and width. Since the size
 * of the elements are known to be one bit.
 * 
 * Inputs:
 *              int height: The height of the 2D array.
 *              int width: The width of the 2D array.
 * Return: A pointer to the newly allocated Bit2_T struct.
 * Expects: All parameters are unsigned.
 * 
 * Notes:  
 *              Will CRE if negative numbers are entered.
 **********************************************/
T Bit2_new(int width, int height)
{
        assert(width > 0 && height > 0); /* Note the 0,0 edge case! */
        T newArr = malloc(sizeof(struct T));
        assert(newArr != NULL);
        newArr->height = height;
        newArr->width = width;
        newArr->array = Bit_new(height*width);
        newArr->num_one = 0;
        return newArr;
}


/***************** Bit2_free *******************
 *
 * Deallocates and clears *arr.
 * 
 * Inputs:
 *              Bit2_T *arr: A pointer to the Bit2_T struct to be recycled.
 * Expects: arr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if arr or *arr is null.
 *********************************************/
void Bit2_free(T *arr)
{
        assert(arr != NULL && *arr != NULL);
        Bit_free(&((*arr)->array));
        free(*arr);
}


/***************** Bit2_width *******************
 *
 * Returns the integer size of the width of the given Bit2_T struct.
 * 
 * Inputs:
 *              Bit2_T arr: A Bit2_T struct.
 * Expects: arr is a nonnull value.
 * Return: An integer value of the width of the array.
 * 
 * Notes:  Will CRE if arr is null.
 ***********************************************/
int Bit2_width(T arr)
{
        assert(arr != NULL);
        return arr->width;
}


/***************** Bit2_height *******************
 *
 * Returns the integer size of the height of the given Bit2_T struct.
 * 
 * Inputs:
 *              Bit2_T arr: A Bit2_T struct.
 * Expects: arr is a nonnull value.
 * Return: An integer value of the width of the array.
 * 
 * Notes:  Will CRE if arr is null.
 ************************************************/
int Bit2_height(T arr)
{
        assert(arr != NULL);
        return arr->height;
}


/***************** Bit2_put *******************
 *
 * Places a given bit value in the Bit2 array.
 * 
 * Inputs:
 *              Bit2_T arr: A Bit2_T struct.
 *              int height: The height of the desired index to change.
 *              int width: The width of the desired index to change.
 *              int bit: The value of the bit to insert.
 * Expects: The input bit is either 0 or 1, the height and width are within
 *          range, arr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if bit exceeds 0 and 1, the index is out of range, or 
 *         arr is null.
 **********************************************/
int Bit2_put(T arr, int col, int row, int bit)
{
        assert(arr != NULL);
        assert(col >= 0 && col < arr->width);
        assert(row >= 0 && row < arr->height);
        assert(bit == 0 || bit == 1);
        if (bit == 1 && Bit2_get(arr, col, row) == 0) {
                arr->num_one++;
        }
        return Bit_put(arr->array, (row*(arr->width))+col, bit);
}


/************************ Bit2_get **************************
 *
 * Returns the bit value at a given index.
 * 
 * Inputs:
 *              Bit2_T arr: A Bit2_T struct.
 *              int height: The height of the desired index.
 *              int width: The width of the desired index.
 * Expects: The height and width are within range and arr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if the index is out of range, or arr is null.
 ***********************************************************/
int Bit2_get(T arr, int col, int row){
        assert(arr != NULL);
        assert(col >= 0 && col < arr->width);
        assert(row >= 0 && row < arr->height);
        return Bit_get(arr->array, (row*(arr->width))+col);
}


/***************** Bit2_map_col_major *******************
 *
 * Applies a given function to each member in the 2D array by going down 
 * the columns first. 
 * 
 * Inputs:
 *              Bit2_T arr: The Bit2_T struct to be recycled.
 *              void apply(int i, int j, 
 *                      T arr, int b, void *p1): A function created
 *                      by the user and is applied to every element.
 *                                      
 *              void *p1: A closure argument.
 * Expects: i and j are within range and arr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if uarr is null.
 *********************************************************/
void Bit2_map_col_major(T arr, void apply(int i, int j, T arr, int b,
                               void *p1), void *p1)
{
        assert(arr != NULL); 
        for (int i = 0; i < arr->width; i++) {
                for (int j = 0; j < arr->height; j++) {
                        apply(i,j,arr,Bit2_get(arr,i,j),p1);
                }
        }
}


/***************** Bit2_map_row_major *******************
 *
 * Applies a given function to each member in the 2D array by going across 
 * the rows first. 
 * 
 * Inputs:
 *              Bit2_T arr: The Bit2_T struct to be recycled.
 *              void apply(int i, int j, 
 *                      T arr, int b, void *p1): A function created
 *                      by the user and is applied to every element.
 *                                      
 *              void *p1: A closure argument.
 * Expects: i and j are within range and arr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if uarr is null.
 ********************************************************/
void Bit2_map_row_major(T arr, void apply(int i, int j, T arr, int b,
                               void *p1), void *p1)
{
        assert(arr != NULL); 
        for (int j = 0; j < arr->height; j++) {
                for (int i = 0; i < arr->width; i++) {
                        apply(i,j,arr,Bit2_get(arr,i,j),p1);
                }
        }
}


/***************** getNumOnes *******************
 *
 * Returns the number of bits with value 1 from the given array.
 * 
 * Inputs:
 *              Bit2_T arr: The Bit2_T struct to be recycled.
 *
 * Expects: arr is a nonnull value.
 * Return: An integer value of the number of bits with value 1 in the array.
 * 
 * Notes:  Will CRE if arr is null.
 ***********************************************/
int getNumOnes(T arr)
{
        return arr->num_one;
}

#undef T