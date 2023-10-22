/*              main.c
 * Joseph Montalto (jmonta04) and Manpreet Kaur (mkaur08)
 * date: 4/14/2023
 * assignment: HW6: um 
 * 
 * summary: Runs entire program and does file I/O. 
*/


#include <stdio.h>
#include <stdlib.h>
#include "segment.h"
#include "registers.h"
#include "assert.h"

static FILE *open_file(char *file_name);


/********** main ********
 * Initializer function that reads file name and starts the entire program 
 * to run. 
 * 
 * Inputs:
 *		int argc: number of arguments on command line 
 *              char *argv[]: command line argument strings. 
 * Return: An initializer function to run the program. 
 * Expects
 *     		None.
 * Notes:
 *              None. 
 ************************/
int main(int argc, char *argv[])
{
        FILE *file = NULL;
        char *file_name;
        file_name = (argc == 2) ? argv[1] : NULL;
        file = open_file(file_name); 
        /* This doesn't abort when null is inserted on stdin */

        segmentData prog = start_program(file);
        run_program(prog);



        fclose(file);

        return 0;
}



/********** open_file ********
 * Helper function for opening a file or reading from stdin. 
 * 
 * Inputs:
 *		char *file_name: name of given file on command line
 * Return: A file that can be read and opened. 
 * Expects
 *     		None.
 * Notes:
 *         CRE if FILE is NULL. 
 ************************/
static FILE *open_file(char *file_name)
{
        FILE *file;

        if (file_name != NULL) {
                file = fopen(file_name, "r");
                assert(file != NULL);
        } else {
                file = stdin;
        }
    
        return file;
}