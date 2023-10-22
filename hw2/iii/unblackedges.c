/*    
 *  unblackedges.c
 *         Alec Plano and Joseph Montalto
 *         2/7/2023
 *
 *              
 *         This program takes an input pbm file either from the command 
 *         line or from stdin and turns the blackened edges into white space.
 *
 *         NOTE: The resulting PBM is printed to stdout
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bit2.h"
#include "pnmrdr.h"

void fillData(Bit2_T raw, Bit2_T flag, Pnmrdr_mapdata data, Pnmrdr_T map);
int flagData(Bit2_T raw, Bit2_T flags);
void topSweep(Bit2_T raw, Bit2_T flag);
void botSweep(Bit2_T raw, Bit2_T flag);
void leftSweep(Bit2_T raw, Bit2_T flag);
void rightSweep(Bit2_T raw, Bit2_T flag);
void deleteFlags(Bit2_T raw, Bit2_T flags);


/***************** main *******************
 *
 * Calls the functions needed to replace the black edges with whitespace.
 * 
 * Inputs:
 *              int argc: the number of arguments given.
 *              char *argv[]: a cstring array of the arguments.
 * Return: 0 if successful.
 * Expects: The input file is a pbm.
 *****************************************/
int main(int argc, char *argv[])
{
        assert(argc < 3);
        FILE *page;
        if (argc == 2) {
                page = fopen(argv[1], "r");
                assert(page != NULL);
        } else {
                page = stdin;
        }

        Pnmrdr_T map = Pnmrdr_new(page);
        Pnmrdr_mapdata data = Pnmrdr_data(map);

        int width = data.width;
        int height = data.height;

        Bit2_T raw = Bit2_new(width, height);
        Bit2_T flags = Bit2_new(width, height);

        fillData(raw, flags, data, map);
        while (flagData(raw, flags) != 0) {}
        deleteFlags(raw, flags);

        Bit2_free(&raw);
        Bit2_free(&flags);
        Pnmrdr_free(&map);
        fclose(page);
        return 0;
}


/***************** deleteFlags *******************
 *
 * Deletes the flagged elements and prints the resulting image to stdout.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains the flagged
 *                           elements to delete.
 * Return: None.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 ************************************************/
void deleteFlags(Bit2_T raw, Bit2_T flags) 
{
        printf("P1\n%d %d\n", raw->width, raw->height);
        for (long i = 0; i < Bit2_height(raw); i++) {
                for (long j = 0; j < Bit2_width(raw); j++) {
                        if (Bit2_get(flags, j, i) == 1) {
                                Bit2_put(raw, j, i, 0);
                        }
                        printf("%d", Bit2_get(raw, j, i));
                        if (j != Bit2_width(raw)-1) {
                                printf(" ");
                        }
                }
                printf("\n");
        }
}


/******************* fillData *********************
 *
 * Fills the raw 2D array with the elements from the given pbm, and
 * initializes the flag 2D array with 0.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains flagged 
 *                            elements 
 * Return: None.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 *************************************************/
void fillData(Bit2_T raw, Bit2_T flag, Pnmrdr_mapdata data, Pnmrdr_T map)
{
        for (long i = 0; i < data.height; i++) {
                for (long j = 0; j < data.width; j++) {
                        Bit2_put(raw,j,i,Pnmrdr_get(map));
                        Bit2_put(flag,j,i,0);
                }
        }
}


/***************** flagData *******************
 *
 * Flags the data that needs to be deleted later on.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains the flagged
 *                           elements.
 * Return: The number of newly flagged elements.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 ************************************************/
int flagData(Bit2_T raw, Bit2_T flag)
{
        int newFlags = getNumOnes(flag);

        /* top down sweep */
        topSweep(raw, flag);

        /* left to right */
        leftSweep(raw, flag);

        /* bottom to up */
        botSweep(raw, flag);

        /* right to left */
        rightSweep(raw, flag);

        return getNumOnes(flag) - newFlags;
}


/***************** topSweep *******************
 *
 * Performs a sweep from top to bottom on the image to find new 
 * elements to flag.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains the flagged
 *                           elements.
 * Return: None.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 ************************************************/
void topSweep(Bit2_T raw, Bit2_T flag)
{
        for (int i = 0; i < Bit2_height(raw); i++) {
                for (int j = 0; j < Bit2_width(raw); j++) {
                        if ((i == 0 || j == 0) && Bit2_get(raw,j,i) == 1) {
                                Bit2_put(flag,j,i,1);
                        } else if (i != 0 && Bit2_get(flag,j,i-1) && 
                        Bit2_get(raw,j,i) == 1) {
                                Bit2_put(flag,j,i,1);     
                        }
                }
        }
}


/***************** leftSweep *******************
 *
 * Performs a sweep from left to right on the image to find new 
 * elements to flag.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains the flagged
 *                           elements.
 * Return: None.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 ************************************************/
void leftSweep(Bit2_T raw, Bit2_T flag)
{
        for (int i = 0; i < Bit2_width(raw); i++) {
                for (int j = 0; j < Bit2_height(raw); j++) {
                        if ((i == 0 || j == 0) && Bit2_get(raw,i,j) == 1) {
                                Bit2_put(flag,i,j,1);
                        } else if (i != 0 && Bit2_get(flag,i-1,j) && 
                        Bit2_get(raw,i,j) == 1) { 
                                Bit2_put(flag,i,j,1);    
                        }
                }
        }
}


/***************** botSweep *******************
 *
 * Performs a sweep from bottom to top on the image to find new 
 * elements to flag.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains the flagged
 *                           elements.
 * Return: None.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 ************************************************/
void botSweep(Bit2_T raw, Bit2_T flag)
{
        for (int i = Bit2_height(raw)-1; i > 0; i--) {
                for (int j = 0; j < Bit2_width(raw); j++) {
                        if ((i == Bit2_height(raw)-1 || j == 0) && 
                        Bit2_get(raw,j,i) == 1) {
                                Bit2_put(flag,j,i,1);
                        } else if (i != Bit2_height(flag)-1 && 
                        Bit2_get(flag,j,i+1) && Bit2_get(raw,j,i) == 1) {
                                Bit2_put(flag,j,i,1); 
                        }
                }
        }
}


/***************** rightSweep *******************
 *
 * Performs a sweep from right to left on the image to find new 
 * elements to flag.
 * 
 * Inputs:
 *              Bit2_T raw: The 2D bit array of the original image.
 *              Bit2_T flags: A 2D array that solely contains the flagged
 *                           elements.
 * Return: None.
 * Expects: The raw and flags bit array to be nonnull values.
 * 
 * Notes:  
 *              Will CRE if either array is null.
 ************************************************/
void rightSweep(Bit2_T raw, Bit2_T flag)
{
        for (int i = Bit2_width(raw)-1; i > 0; i--) {
                for (int j = Bit2_height(raw)-1; j > 0; j--) {
                        if ((i == Bit2_width(raw)-1 || 
                        j == Bit2_height(raw)-1) && Bit2_get(raw,i,j) == 1) {
                                
                                Bit2_put(flag,i,j,1);

                        } else if (i != Bit2_width(raw)-1 && 
                        Bit2_get(flag,i+1,j) && Bit2_get(raw,i,j) == 1) {
                                
                                Bit2_put(flag,i,j,1);
                                
                        }
                }
        }
}