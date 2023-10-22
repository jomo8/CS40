/*
 *  readaline.c
 *  Joseph Montalto and Alec Plano
 *  1/21/2023
 *
 *  CS 40 HW 1 filesofpix A program to reverse PNM corruption.
 *
 *  A file to define the readaline class and read lines from a given file.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include <except.h>
#include <assert.h>

/* 
 * readaline
 *    Purpose: Reads individual lines from a given file.
 *    Parameters: A pointer to the file and a double pointer to the first byte.
 *    Returns: A size_t int of the number of bytes in the line.
 */
size_t readaline(FILE *inputfd, char **datapp)
{       
        /* Checks that both values are not null */
        assert(inputfd != NULL && (*datapp) != NULL);
        int curr = 0;
        long int line_length = 0;
        curr = fgetc(inputfd);
        if (curr == EOF) { /* Checking for empty file */
                *datapp = NULL;
                return 0;     
        }

        line_length = 1;
        /* Getting the length of the line */
        while ((char)curr != '\n' && curr != EOF) {
                curr = fgetc(inputfd);
                if (curr != EOF) {
                        line_length++;
                }          
        }

        *datapp = (char*)malloc(line_length);
        assert(*datapp);
        fseek(inputfd, -line_length, SEEK_CUR);
        /* Adding each character into the datapp char array */
        for(int i = 0; i < line_length; i++){
                (*datapp)[i] = (char)fgetc(inputfd);
        }   

        return line_length;
}