#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"

void sortaline(char **datapp, size_t int_bytes, char **char_string, char **int_string, 
int *Size_char_string, int *Size_int_string);
int isDigit(char character);
void printASCII();
void bigboiReadaline();
void tinyReadaline();
void tinySortaline();
void tinySortaline2();


/* 
 * main
 *    Purpose: Calls test functions to test their outputs.
 *    Parameters: None.
 *    Returns: 0.
 */
int main() {
        /* printASCII(); */
        tinySortaline2();
        //tinyReadaline();
        //tinySortaline();
        //bigboiReadaline();
        return 0;
}


/* 
 * printASCII
 *    Purpose: Prints every ASCII value once into an output file.
 *    Parameters: None.
 *    Returns: None.
 */
void printASCII() {
        FILE *f = fopen("allASCII.txt", "w");
        for(int i = 0; i<=255; i++){
                fprintf(f, "%c", (char)i);
        }
        fclose(f);
}


/* 
 * tinyReadaline
 *    Purpose: Tests readaline function on a small, single line of chars.
 *    Parameters: None.
 *    Returns: None.
 */
void tinyReadaline() {
        FILE *test = fopen("tinyTest.txt", "r");
        FILE *output = fopen("output.txt", "w");
        char *datapp = "";
        int j=readaline(test, &datapp);
        for (int i=0; i<j; i++) {
                fprintf(output, "%c", datapp[i]);
        }
        fclose(test);
        fclose(output);
        free(datapp);
}


/* 
 * bigboiReadaline
 *    Purpose: Tests readaline function on a the bigboi-corrupt.pgm 
 *             to test on large files and lines.
 *    Parameters: None.
 *    Returns: None.
 */
void bigboiReadaline() {
        FILE *test = fopen("/h/jmonta04/cs40/hw1/filesofpix/testFiles/bigboi-corrupt.pgm", "r");
        FILE *output = fopen("output.txt", "w");
        char *datapp = "";
        
        while (!feof(test)) {
                int j=readaline(test, &datapp);
                for (int i=0; i<j; i++) {
                        fprintf(output, "%c", datapp[i]);
                } 
                free(datapp);
        }
        fclose(test);
        fclose(output);
}


/* 
 * tinySortaline
 *    Purpose: Tests sortaline function on a small, single line of chars.
 *    Parameters: None.
 *    Returns: None.
 */
void tinySortaline() {
        FILE *tmp = fopen("tinyTest.txt", "r");

        char *datapp;
        size_t size_of_line = readaline(tmp, &datapp);
        char *char_string;
        char *int_string;
        int size_of_char_string;
        int size_of_int_string;
        sortaline(&datapp, size_of_line, &char_string, &int_string, &size_of_char_string, &size_of_int_string);
        free(datapp);
        free(char_string);
        free(int_string);
        
        fclose(tmp);
}


/* 
 * tinySortaline2
 *    Purpose: Tests sortaline function on a small, single line of chars
 *             and prints output to fprint().
 *    Parameters: None.
 *    Returns: None.
 */
void tinySortaline2() {
        FILE *tmp = fopen("tinyTest.txt", "r");

        char *datapp;
        size_t size_of_line = readaline(tmp, &datapp);
        //printf("%s", datapp);
        char *char_string;
        char *int_string;
        int size_of_char_string;
        int size_of_int_string;
        sortaline(&datapp, size_of_line, &char_string, &int_string, &size_of_char_string, &size_of_int_string);
        printf("%s\n", char_string);
        printf("%s\n", int_string);
        free(datapp);

        
        fclose(tmp);
        free(char_string);
        free(int_string);
}



/*
 *
 * The following functions are included for testing purposes and will can
 * deleted once they are no longer needed.
 * 
 */
void sortaline(char **datapp, size_t int_bytes, char **char_string, char **int_string, 
int *Size_char_string, int *Size_int_string) {
        *Size_char_string = 0;
        *Size_int_string = 0;

        char prevChar = 'a';
        for(unsigned i = 0; i<int_bytes; i++) {
                if (!isDigit((*datapp)[i]) && isDigit(prevChar)) {
                        (*Size_int_string)++;
                        (*Size_char_string)++;
                } else if (!isDigit((*datapp)[i]) && !isDigit(prevChar)){
                        (*Size_char_string)++;
                }
                
                prevChar = (*datapp)[i];
        }      
        if(isDigit((*datapp)[int_bytes-1])){
                (*Size_int_string)++;
        }  
        *int_string = malloc((*Size_int_string)+1);
        *char_string = malloc((*Size_char_string)+1);

        int char_iterator = 0;
        int int_iterator = 0;
        prevChar = 'a';
        for(unsigned i = 0; i < int_bytes; i++) {
                if (!isDigit((*datapp)[i])) {
                        (*char_string)[char_iterator] = (*datapp)[i];
                        char_iterator++;
                } else if (!isDigit(prevChar) && isDigit((*datapp)[i])) {
                        (*int_string)[int_iterator] = (*datapp)[i] - '0';
                        int_iterator++;
                } else if (isDigit(prevChar) && isDigit((*datapp)[i])) {
                        (*int_string)[int_iterator-1] *= 10;
                        (*int_string)[int_iterator-1] += ((*datapp)[i]) - '0'; // Check if syntax works.
                }
                prevChar = (*datapp)[i];
        }

        (*int_string)[int_iterator] = '\0';
        (*char_string)[char_iterator] = '\0';
}

int isDigit(char character){
        if (character > 47 && character < 58) {
                return 1;
        }
        return 0;
}