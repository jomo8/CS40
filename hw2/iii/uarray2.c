/*
 *  uarray2.c
 *  Alec Plano and Joseph Montalto
 *  2/5/2023
 *
 *  CS40 HW2 Part A, A program to create and manipulate a 2D unboxed array.
 *
 *  A file to define the UArray2_T class.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include <assert.h>
#define T UArray2_T


/***************** UArray2_new *******************
 *
 * Creates a new unboxed 2D array object with the given height, 
 * width, and element_size.
 * 
 * Inputs:
 *              int height: The height of the 2D array.
 *              int width: The width of the 2D array.
 *              int element_size: The size of the elements to insert
 * Return: A pointer to the allocated memory space on the heap.
 * Expects: 
 *              All parameters entered are unsigned integers.
 * 
 * Notes:  
 *              Will CRE if negative numbers are entered.
 ************************/
T UArray2_new(int width, int height, int element_size)
{

        assert(width >= 0 && height >= 0 && element_size > 0);
        T newArr = malloc(sizeof(struct T));
        assert(newArr != NULL);
        newArr->height = height;
        newArr->width = width;
        newArr->elem_size = element_size;
        newArr->array = UArray_new(height*width, element_size);

        return newArr;
}


/***************** UArray2_free *******************
 *
 * Deallocates and clears *uarr.
 * 
 * Inputs:
 *              Uarray2_T *uarr: A pointer to a UArray2_T.
 * Expects: uarr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if uarr is null.
 ************************/
void UArray2_free(T *uarr)
{
        assert(uarr != NULL && *uarr != NULL);
        UArray_free(&((*uarr)->array));
        free(*uarr);
}


/***************** UArray2_width *******************
 *
 * Gets the int value for the width of the given UArray2_T.
 * 
 * Inputs:
 *              Uarray2_T uarr: The UArray2_T to find the width of.
 * Expects: uarr is a nonnull value.
 * Return: The width of the given UArray2_T
 * 
 * Notes:  Will CRE if uarr is null.
 ************************/
int UArray2_width(T uarr)
{
        assert(uarr != NULL);
        return uarr->width;
}


/***************** UArray2_height *******************
 *
 * Gets the int value for the height of the given UArray2_T.
 * 
 * Inputs:
 *              Uarray2_T uarr: The UArray2_T to find the height of.
 * Expects: uarr is a nonnull value.
 * Return: The height of the given UArray2_T
 * 
 * Notes:  Will CRE if uarr is null.
 ************************/
int UArray2_height(T uarr)
{
        assert(uarr != NULL);
        return uarr->height;
}


/***************** UArray2_size *******************
 *
 * Gets the int value for the element size of the given UArray2_T.
 * 
 * Inputs:
 *              Uarray2_T uarr: The UArray2_T to find the element size of.
 * Expects: uarr is a nonnull value.
 * Return: The int value for the element size of the given UArray2_T.
 * 
 * Notes:  Will CRE if uarr is null.
 ************************/
int UArray2_size(T uarr)
{
        assert(uarr != NULL);
        return uarr->elem_size;
}


/***************** UArray2_at *******************
 *
 * Gets the int value for the height of the given UArray2_T.
 * 
 * Inputs:
 *            Uarray2_T uarr: The UArray2_T that contains the element.
 *                      int col: The column of the desired element.
 *                      int row: The row of the desired element.
 * Expects: uarr is a nonnull value and the index is within range of the array.
 * Return: A void pointer of the element at the given index.
 * 
 * Notes:  Will CRE if either col or row are out of range, or if uarr is null.
 ************************/
void *UArray2_at(T uarr, int col, int row)
{
        assert(col >= 0 && col < uarr->width);
        assert(row >= 0 && row < uarr->height);
        assert(uarr != NULL);
        return UArray_at(uarr->array, (row*(uarr->width))+col); 
}


/***************** UArray2_map_col_major *******************
 *
 * Applies a given function to each member in the 2D array by going down 
 * the columns first. 
 * 
 * Inputs:
 *              Uarray2_T uarr: The UArray2_T that contains the array.
 *              void apply(int i, int j, 
 *                      UArray2_T a, void *p1, void *cl): A function created
 *                      by the user and is applied to every element.
 *                                      
 *              void *cl: A closure argument.
 * Expects: i and j are within range and uarr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if uarr is null.
 ************************/
void UArray2_map_col_major(T uarr, void apply(int i, int j, 
                           T a, void *p1, void *cl), void *cl)
{
        assert(uarr != NULL);
        for (int i = 0; i < uarr->width; i++) {
                for (int j = 0; j < uarr->height; j++) {
                        apply(i,j,uarr,UArray2_at(uarr,i,j),cl);
                }
        }
}


/***************** UArray2_map_row_major *******************
 *
 * Applies a given function to each member in the 2D array by going across 
 * the rows first. 
 * 
 * Inputs:
 *              Uarray2_T uarr: The UArray2_T that contains the array.
 *              void apply(int i, int j, 
 *                      UArray2_T a, void *p1, void *cl): A function created
 *                      by the user and is applied to every element.
 *                                      
 *              void *cl: A closure argument.
 * Expects: uarr is a nonnull value.
 * Return: None.
 * 
 * Notes:  Will CRE if uarr is null.
 ************************/
void UArray2_map_row_major(T uarr, void apply(int i, int j, 
                           T a, void *p1, void *cl), void *cl)
{
        assert(uarr != NULL);
        for (int j = 0; j < uarr->height; j++) {
                for (int i = 0; i < uarr->width; i++) {
                        apply(i,j,uarr,UArray2_at(uarr,i,j),cl);
                }
        }
        
}

#undef T