/*
 * a2blocked.c
 * by Sam Reed and Joey Montalto
 * 12/22/2023
 * locality
 *
 * a2blocked creates a 2d array, where each element in the array represents
 *      UArray2, which is another representation of a 2d array.
 *
 * Can be initialized with new() with a 64KB blocksize, or 
 *      new_with_blocksize() when the blocksize needs to be a specified amount.
 */

#include <string.h>
#include <a2blocked.h>
#include "uarray2b.h"

// define a private version of each function in A2Methods_T that we implement

typedef A2Methods_UArray2 A2;   // private abbreviation


/********************************** new ********************************
 *
 * Creates a new A2Methods_UArray2 data structure with each block holding
 * roughly 64KB.
 * Inputs:
 *       int width: The desired width of the new array.
 *       int height: The desired height of the new array.
 *       int size: The size of each element in bytes.
 * Return: A pointer to the new data structure.
 * Expects
 *      All values to be unsigned integers.
 **********************************************************************/
static A2 new(int width, int height, int size)
{
        return UArray2b_new_64K_block(width, height, size);
}


/************************** new_with_blocksize ************************
 *
 * Creates a new A2Methods_UArray2 data structure with its own blocksize.
 * Inputs:
 *       int width: The desired width of the new array.
 *       int height: The desired height of the new array.
 *       int size: The size of each element in bytes.
 * Return: A pointer to the new data structure.
 * Expects
 *      All values to be unsigned integers.
 **********************************************************************/
static A2 new_with_blocksize(int width, int height, int size, int blocksize)
{
        return UArray2b_new(width, height, size, blocksize);
}


/******************************** a2free ******************************
 *
 * Recycles the memory previously allocated by a A2Methods_UArray2.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 * Return: None.
 * Expects
 *      The pointer to be a nonnull value.
 **********************************************************************/
static void a2free(A2 *array2p)
{
        UArray2b_free((UArray2b_T *) array2p);
}


/********************************** width ********************************
 *
 * Returns the width of an A2 data structure.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 * Return: The width of the given A2Methods_UArray2.
 * Expects
 *      The pointer to be a nonnull value.
 ************************************************************************/
static int width(A2 array2)
{
        return UArray2b_width(array2);
}


/********************************** height ********************************
 *
 * Returns the height of an A2 data structure.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 * Return: The height of the given A2Methods_UArray2.
 * Expects
 *      The pointer to be a nonnull value.
 ************************************************************************/
static int height(A2 array2)
{
        return UArray2b_height(array2);
}


/********************************** size ********************************
 *
 * Returns the size of an A2 data structure.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 * Return: The size of the given A2Methods_UArray2.
 * Expects
 *      The pointer to be a nonnull value.
 ************************************************************************/
static int size(A2 array2)
{
        return UArray2b_size(array2);
}


/******************************** blocksize ******************************
 *
 * Returns the blocksize of an A2 data structure.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 * Return: The blocksize of the given A2Methods_UArray2.
 * Expects
 *      The pointer to be a nonnull value.
 ************************************************************************/
static int blocksize(A2 array2)
{
        return UArray2b_blocksize(array2);
}


/********************************** at ********************************
 *
 * Returns the element at the given index of an A2 data structure.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 *       int i: The column of the desired element.
 *       int j: The row of the desired element.
 * Return: The height of the given A2Methods_UArray2.
 * Expects
 *      The pointer to be a nonnull value, and the integer values to 
 *      be within range. 
 ************************************************************************/
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2b_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2b_T array2b, void *elem, void *cl);


/******************************* map_block_major *****************************
 *
 * Applies a given function on each element of an A2 struct following block 
 *        major order.
 * Inputs:
 *       A2 *array2: A pointer to an A2Methods_UArray2 data structure.
 *       A2Methods_applyfun apply: The function to apply across the A2 struct.
 *       void *cl: The closure argument for the user to use.
 * Return: None.
 * Expects
 *      The pointers to be a nonnull value, except for the closure argument.
 ************************************************************************/
static void map_block_major(A2 array2, A2Methods_applyfun apply, void *cl)
{
        UArray2b_map(array2, (applyfun *) apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply;
        void *cl;
};

static void apply_small(int i, int j, UArray2b_T array2, void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)array2;
        cl->apply(elem, cl->cl);
}

static void small_map_block_major(A2 a2, A2Methods_smallapplyfun apply,
                                  void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2b_map(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_blocked_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        NULL,                   // map_row_major
        NULL,                   // map_col_major
        map_block_major,
        map_block_major,        // map_default
        NULL,                   // small_map_row_major
        NULL,                   // small_map_col_major
        small_map_block_major,
        small_map_block_major,  // small_map_default
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_blocked = &uarray2_methods_blocked_struct;
