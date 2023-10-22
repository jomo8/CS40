/*
 *  sudoku.c
 *         Alec Plano and Joseph Montalto
 *         2/6/2023
 * 
 * 
 *         This program determines if a sudoku bord, moddeled as a pgm is,
 *         is a valid solution or not. If the sudoku bord is a valid solution
 *         the program returns 0 and if the file is not a valid solution the
 *         program returns 1.
 *
 *
 *         NOTE: The program returns a CRE if the provided PGM is not a valid
 *               PGM.
 *
 */

#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include "uarray2.h"
#include "pnmrdr.h"

void fillUarray(FILE *board, struct UArray2_T *uarr);
void fillTestArray(int i, int *test_array, struct UArray2_T *uarr);
int checkTestArray(int *test_array);


/***************** main *******************
 *
 * Calls the functions needed to check the sudoku pgm.
 * 
 * Inputs:
 *              int argc: the number of arguments given.
 *              char *argv[]: a cstring array of the arguments.
 * Return: 0 if successful, and 1 otherwise.
 * Expects: The pgm to only contain values between 1 and 9 inclusive.
 ************************/
int main(int argc, char *argv[])
{
        assert(argc < 3);
        FILE *board;
        if (argc == 2) {
                board = fopen(argv[1], "r");
                assert(board != NULL);
        } else {
                board = stdin;
        }

        UArray2_T uarr = UArray2_new(9, 9, 4);
        fillUarray(board, uarr);



        int test_array[9];
        int is_valid = 1;
        for(int i = 0; i < 27 && is_valid; i++){
                fillTestArray(i, test_array, uarr);
                is_valid = checkTestArray(test_array);
        }

        UArray2_free(&uarr);
        fclose(board);
        return !is_valid;
}


/***************** fillUarray *******************
 *
 * Fills a UArray2_T with the given 
 * 
 * Inputs:
 *              FILE *board: The file that contains the sudoku PGM to read.
 *              UArray2_T *uarr: A 2D array to fill with sudoku values.
 * Return: None.
 * Expects: Board to be a valid PGM file.
 * 
 * Notes:  
 *              Will CRE if board is not a valid PGM file.
 ***********************************************/
void fillUarray(FILE *board, struct UArray2_T *uarr)
{

        Pnmrdr_T map = Pnmrdr_new(board);

        for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                        *(int*)UArray2_at(uarr, j, i) = Pnmrdr_get(map);
                }
        }

        Pnmrdr_free(&map);
}


/***************** fillTestArray *******************
 *
 * Fills an array of size 9 with the value from the sudoku puzzle
 * 
 * Inputs:
 *      int i: Representing which column, row or box you want to 
 *             check from the puzzle.
 *      int *test_array: an array of size 9 that gets filled with
 *                       the desired values from the puzzle
 *      struct UArray2_T *uarr: unboxed array holding the values
 *                              of the sudoku puzzle
 *              
 * Return: Indirect return of the test_array filled with value from 
 *         sudoku board
 * Expects: A UArray of size 9 by 9 and a test array of size 9
 * 
 ************************/
void fillTestArray(int i, int *test_array, struct UArray2_T *uarr)
{
        if (i < 9) {
                for (int j = 0; j < 9; j++) {
                        test_array[j] = *(int*)UArray2_at(uarr,j,i);
                }
                
        } else if (i < 18) {
                for (int j = 0; j < 9; j++) {
                        test_array[j] = *(int*)UArray2_at(uarr,(i % 9),j);
                }
        } else {
                int num = i % 9;
                int col = (num % 3) * 3;
                int row = (num / 3) * 3;
                int idx = 0;
                for (int w = row; w < row + 3; w++) {
                        for (int j = col; j < col + 3; j++) {
                               test_array[idx] = *(int*)UArray2_at(uarr,j,w);
                               idx++;
                        }
                }
        }
}


/***************** checkTestArray *******************
 *
 * Checks whether the given test array contains any duplicates.
 * 
 * Inputs:
 *              int *test_array: A pointer to the array of integers in a
 *                              given section of the sudoku board.
 * Return: 1 if the array passes, and 0 if the array fails the test.
 * Expects: All parameters are between 0 and 9 inclusive.
 * 
 * Notes:  
 *              Will have undefined behavior if values exceed 1 to 9.
 ************************/
int checkTestArray(int *test_array)
{
        int check = 0;
        for (int i = 0; i < 9; i++) {
                check ^= 1<<test_array[i];
        }

        if (check == 0x3FE) {
                return 1;
        }
        return 0;
}