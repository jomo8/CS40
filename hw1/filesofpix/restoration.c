/*
 *  restoration.c
 *  Joseph Montalto and Alec Plano
 *  1/21/2023
 *
 *  CS 40 HW 1 filesofpix A program to reverse PNM corruption.
 *
 *  A file to define the restoration class and perform functions to remove
 *  injected lines and sequences within a corrupted plain PGM file.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "table.h"
#include "atom.h"
#include "seq.h"
#include <except.h>
#include <assert.h>

/*
 * Datum is a structure that holds the int and non-int strings and sizes
 * that are needed in the sortaline function.
 */
struct Datum {
        char *char_str;
        unsigned char *int_str;
        int char_size, int_size;
};

void sortaline(char **datapp, size_t bytes, struct Datum *d);
int isDigit(char character);
void countDigits(char **datapp, size_t bytes, struct Datum *d);
void checkArgs(int args);
void vfree (const void *key, void **value, void *cl);
void vfree(const void *key, void **value, void *cl);
void runProgram(size_t bytes, char *datapp, FILE *inputFile);
void printImage(Seq_T s, int width, int length);
void closeProgram(Table_T t, Seq_T s);


/* 
 * main
 *    Purpose: Calls the needed functions to restore a given corrupted file.
 *    Parameters: The number of arguments and char array of arguments used
 *                with the command.
 */
int main(int argc, char *argv[])
{      
        assert(argc < 3);
        FILE * inputFile;
        /* Retrieve the input file */
        if (argc == 1) {
                inputFile = stdin;
        } else {
                inputFile = fopen(argv[1], "r");
        }

        char *datapp = "";  
        size_t bytes = readaline(inputFile, &datapp);

        /* Run the program with the information collected */
        runProgram(bytes, datapp, inputFile);
        fclose(inputFile);

        return 0;
}


/* 
 * runProgram
 *    Purpose: Runs functions and loops that read the line from an
 *             inputFile, sort those lines into int and non-int strings, 
 *             recycles injected lines, and calls a function to print the
 *             image.
 *    Parameters: The number of bytes and a c string of the first line; and
 *                a FILE* of the input file.
 *    Returns: None.
 */
void runProgram(size_t bytes, char *datapp, FILE *inputFile)
{
        Table_T t = Table_new(1000, NULL, NULL);
        Seq_T s = Seq_new(1000);
        char *finalKey;
        int firstRun = 1, width = 0, keySize = 0;

        while (bytes > 0) {
                struct Datum d;
                sortaline(&datapp, bytes, &d);
                char *line = Table_put(t, Atom_new(d.char_str, d.char_size),
                                       d.int_str);
                if (line && firstRun) {/* A special case for the first line */
                        Seq_addhi(s, line);
                        finalKey = malloc(d.char_size);
                        keySize = d.char_size;
                        for (int i = 0; i < d.char_size; i++) {
                                finalKey[i] = d.char_str[i];    
                        }
                        width = d.int_size;
                        firstRun = 0;
                } else if (line) { /* Else, add the line to seq normally */
                        Seq_addhi(s, line);
                }
                free(datapp);
                free(d.char_str);
                bytes = readaline(inputFile, &datapp);
        }
        /* If there were lines in the file, we get last line and free mem. */
        if (firstRun != 0) {
                Seq_addhi(s, 
                        (char *)Table_get(t, Atom_new(finalKey, keySize)));
                free(finalKey);
        }
        printImage(s, width, Seq_length(s));
        closeProgram(t, s);
}

/* 
 * closeProgram
 *    Purpose: Free up the allocated memory in the table and sequence.
 *    Parameters: Hanson's Table_T and Seq_T data structures.
 *    Returns: None.
 * 
 *              NOTE: Hanson's Atom structures cannot be recycled.
 */
void closeProgram(Table_T t, Seq_T s)
{
        Table_map(t, vfree, NULL);
        Table_free(&t);
        for (int i = 0; i < Seq_length(s)-1; i++) {
                free(Seq_get(s,i));
        }
        Seq_free(&s);
}


/* 
 * printImage
 *    Purpose: Prints the lines of the "raw" image PGM to stdout.
 *    Parameters: A Sequence of the original image lines, the length and
 *                width of the image.
 *    Returns: None.
 */
void printImage(Seq_T s, int width, int length)
{
        FILE *outputFile = stdout;
        fprintf(outputFile, "P5\n%d %d\n255\n", width, length);
        for (int i = 0; i < length; i++) {
                for (int j = 0; j < width; j++) {
                        fprintf(outputFile, "%c", ((char*)Seq_get(s, i))[j]);
                }
        }
        fclose(outputFile);
}


/* 
 * vfree
 *    Purpose: Frees up individual heap memory within table.
 *    Parameters: A void pointer to the key 
 *    Returns: None.
 */
void vfree(const void *key, void **value, void *cl)
{
        (void) key;
        (void) cl;
        free(*value);
}


/* 
 * sortaline
 *    Purpose: Sorts a given pointer to c string into c strings of ints
 *             and non in bytesbytests.
 *    Parameters: A pointer to the c string of the line, the number of bytes
 *                in the line, and a pointer to a Datum struct of line info. 
 *    Returns: None.
 */
void sortaline(char **datapp, size_t bytes, struct Datum *d)
{
        countDigits(datapp, bytes, d);
        d->int_str = malloc(d->int_size+1);
        d->char_str = malloc(d->char_size+1);

        int char_itr = 0, int_itr = 0;
        char prevChar = 'a';
        for(unsigned i = 0; i < bytes; i++) {
                if (!isDigit((*datapp)[i])) {
                        d->char_str[char_itr] = (*datapp)[i];
                        char_itr++;
                } else if (!isDigit(prevChar) && isDigit((*datapp)[i])) {
                        d->int_str[int_itr] = (*datapp)[i] - '0';
                        int_itr++;
                } else if (isDigit(prevChar) && isDigit((*datapp)[i])) {
                        d->int_str[int_itr-1] *= 10;
                        d->int_str[int_itr-1] += ((*datapp)[i]) - '0';
                }
                prevChar = (*datapp)[i];
        }

        d->int_str[int_itr] = '\0';
        d->char_str[char_itr] = '\0';
}


/* 
 * countDigits
 *    Purpose: Counts the number of ints and non-ints within a line and
 *             updates the information within the Datum d.
 *    Parameters: A pointer to the c string of the line, the number of bytes
 *                in the line, and a pointer to a Datum struct of line info. 
 *    Returns: None.
 */
void countDigits(char **datapp, size_t bytes, struct Datum *d)
{
        d->char_size = 0, d->int_size = 0;
        char prevChar = 'a';
        for(unsigned i = 0; i < bytes; i++) {
                if (!isDigit((*datapp)[i]) && isDigit(prevChar)) {
                        d->char_size++;
                        d->int_size++;
                } else if (!isDigit((*datapp)[i]) && !isDigit(prevChar)){
                        d->char_size++;
                }
                
                prevChar = (*datapp)[i];
        }      
        if (isDigit((*datapp)[bytes-1])){
                d->int_size++;
        }  
}


/* 
 * isDigit
 *    Purpose: Returns 1 if a given character is an int digit, otherwise 0.
 *    Parameters: A char.
 *    Returns: 1 (true) if the char is a digit, and 0 if not.
 */
int isDigit(char character)
{
        if (character > 47 && character < 58) {
                return 1;
        }
        return 0;
}