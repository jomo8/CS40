/*
 * ppmtrans.c
 * by Sam Reed and Joey Montalto
 * 2/22/23
 * locality
 *
 * ppmtrans.c offers a variety of image manipulation tools for ppm image file, 
 *      and can be performed using row-major, column-major, or block-major 
 *      mapping.
 * 
 *  These transformations can be timed using the -time flag and outputting to a
 *  text file.
 * 
 * Files can be inputted through a ppm file in the function call, or through 
 *      standard input
 *  Valid manipulations include: -rotate 0 -rotate 90 -rotate 180 -rotate 270
 *      -flip horizontal -flip vertical -transpose
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

struct closure {
        A2Methods_T methods;
        A2Methods_UArray2 final;
};

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(EXIT_FAILURE);
}

A2Methods_UArray2 rotater (int rotateVal, A2Methods_UArray2 reader, 
                           A2Methods_T methods, char *time_file_name);
void rotate90 (int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
               void *cl);
void rotate180(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
               void *cl);
void rotate270(int i, int j, A2Methods_UArray2 array2, A2Methods_Object *ptr,
               void *cl);
A2Methods_UArray2 flip(char *direction, A2Methods_UArray2 reader,
                       A2Methods_T methods, char *time_file_name);
void flip_horizontal_map (int i, int j, A2Methods_UArray2 array2, 
                          A2Methods_Object *ptr, void *cl);
void flip_vertical_map (int i, int j, A2Methods_UArray2 array2,
                        A2Methods_Object *ptr, void *cl);
void transpose_map(int i, int j, A2Methods_UArray2 array2, 
                   A2Methods_Object *ptr, void *cl);
static FILE *open_file(char *file_name, char *tpye);


/*******************************  main  *******************************
 *
 * Calls the needed function to perform given transformations on the ppm file.
 *
**********************************************************************/
int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        char *direction      = NULL;
        int   i              = 0;
        
        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        FILE *file = NULL;
        
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-flip")  == 0 ) {
                        direction = argv[++i];
                        if (strcmp(direction, "horizontal") == 1 && 
                                strcmp(direction, "vertical") == 1) {
                                fprintf(stderr, 
                                      "Flip Must be Horizontal or vertical\n");
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0){
                        direction = "transpose";
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        //If arguments are left, open file.  If not, use standard input
        if (i < argc) {
                file = open_file(argv[i], "r");
        } else {
                file = stdin;
        }

        /* opens the file utilizing pnmread */
        Pnm_ppm pixelmap = Pnm_ppmread(file, methods);
        A2Methods_UArray2 reader = pixelmap->pixels;

        /* always calls rotate, if no command given roates 0 */
        A2Methods_UArray2 temp = rotater(rotation, reader, methods,
                                        time_file_name);
        pixelmap->pixels = temp;
        pixelmap->height = methods->height(temp);
        pixelmap->width = methods->width(temp);

        if (direction != NULL){
                pixelmap->pixels = flip(direction, reader, methods,
                                        time_file_name);
        }

        Pnm_ppmwrite(stdout, pixelmap);
        Pnm_ppmfree(&pixelmap);
        fclose(file);
        exit(EXIT_SUCCESS);
}

/**********rotater********
 *
 * Rotates the given 2D array by the given degree by returning a second 2d 
 *      array for the rotation
 * Inputs:
 *       int rotateVal: The number of degrees for the image to be rotated by
 *       A2Methods_UArray2 reader: A 2d array containing the pixels in the
 *              original image
 *       A2Methods_T methods: The struct of methods for the 2d array
 *       char *time_file_name: the name of the time output file
 * Return: a A2Methods_UArray2 containing the rotated image.
 * Expects
 *      rotateVal should be 0, 90, 180, or 270.
 ************************/
A2Methods_UArray2 rotater (int rotateVal, A2Methods_UArray2 reader, 
                           A2Methods_T methods, char *time_file_name)
{       
        FILE *time_file = NULL;
        CPUTime_T timer;
        double time_used;
        
        
        int width = methods->width(reader);
        int height = methods->height(reader);

        struct closure *rotate_cl;
        NEW(rotate_cl);
        rotate_cl->methods = methods;
        
        A2Methods_UArray2 final = reader;


        //Starts CPU Timer right before roations
        if (time_file_name != NULL){
                timer = CPUTime_New();
                CPUTime_Start(timer);
                time_file = open_file(time_file_name, "w");
        }
        if (rotateVal == 90) {
                final = methods->new(height, width, sizeof(struct Pnm_rgb));
                rotate_cl->final = final;
                methods->map_default(reader, rotate90, rotate_cl);
        } else if (rotateVal == 180) {
                final = methods->new(width, height, sizeof(struct Pnm_rgb));
                rotate_cl->final = final;
                methods->map_default(reader, rotate180, rotate_cl);
        } else if (rotateVal == 270) {
                final = methods->new(height, width, sizeof(struct Pnm_rgb));
                rotate_cl->final = final;
                methods->map_default(reader, rotate270, rotate_cl);
        }

        //Ends CPU Timer right after roations
        if (time_file_name != NULL){
                time_used = CPUTime_Stop(timer);
                fprintf(time_file, "%f",time_used);
                CPUTime_Free(&timer);
                fclose(time_file);
        }

        FREE(rotate_cl);
        if (rotateVal == 0){
                return reader;
        }
        methods->free(&reader);

        return final;
}

/********** open_file ********
 *
 * Opens the given file name, checks if it exists, and returns a pointer to
 *       the opened file
 * Inputs:
 *       char *file_name: the name of the file to be opened
 *       char *type: the method fopen should use to open the file (in this 
 *              program only read or write)
 * Return: a FILE pointer to the opened file
 * Expects
 *      file_name should be an existing file if type is "R", otherwise it will
 *       exit with EXIT_FAILURE
 ************************/
static FILE *open_file(char *file_name, char *type)
{
        FILE *temp = fopen(file_name, type);

        if (temp == NULL) {
                        fprintf(stderr, "File Not Accessible\n");
                        exit(EXIT_FAILURE);
                }
        return temp;
}

/********** flip ********
 *
 * Flips the given 2d array in the given direction by returning a new 2d array 
 *      with the completed transformation
 * Inputs:
 *       char *direction: The direction for the image to be flipped
 *       A2Methods_UArray2 reader: A 2d array containing the pixels in the
 *              original image
 *       A2Methods_T methods: The struct of methods for the 2d array
 *       char *time_file_name: the name of the time output file
 * Return: a A2Methods_UArray2 containing the flipped image.
 * Expects
 *      direction can only be "vertical", "horizontal", or "transpose"
 ************************/
A2Methods_UArray2 flip (char *direction, A2Methods_UArray2 reader, 
                                  A2Methods_T methods, char *time_file_name)
{       
        FILE *time_file = NULL;
        CPUTime_T timer;
        double time_used;
        
        
        int width = methods->width(reader);
        int height = methods->height(reader);

        struct closure *rotate_cl;
        NEW(rotate_cl);
        rotate_cl->methods = methods;
        
        A2Methods_UArray2 final = reader;

        //Starts CPU Timer right before roations
        if (time_file_name != NULL){
                timer = CPUTime_New();
                CPUTime_Start(timer);

                time_file = open_file(time_file_name, "w");
        }

        if (strcmp(direction, "vertical") == 0) {
                final = methods->new(width, height, sizeof(struct Pnm_rgb));
                rotate_cl->final = final;
                methods->map_default(reader, flip_vertical_map, rotate_cl);
        } else if (strcmp(direction, "horizontal") == 0) {
                final = methods->new(width, height, sizeof(struct Pnm_rgb));
                rotate_cl->final = final;
                methods->map_default(reader, flip_horizontal_map, rotate_cl);
        } else if (strcmp(direction, "transpose") == 0){
                final = methods->new(width, height, sizeof(struct Pnm_rgb));
                rotate_cl->final = final;
                methods->map_default(reader, transpose_map, rotate_cl);
        }
      //Ends CPU Timer right after roations
        if (time_file_name != NULL){
                time_used = CPUTime_Stop(timer);
                fprintf(time_file, "%f",time_used);
                CPUTime_Free(&timer);
                fclose(time_file);
        }
        FREE(rotate_cl);
        methods->free(&reader);

        return final;
}

/************* MAPPING FUNCTION ************/

/********** rotate90 ********
 *
 * Mapping function for A2Methods_UArray2, that takes pixels from array2 and
 *       inserted them into a new UArray2 with a 90 degree rotation
 * Inputs:
 *       int col, row: the current index of the map
 *       A2Methods_UArray2 array2: The 2d array that is being mapped
 *       A2Methods_Object: (not used) An A2Methods_Object pointer
 *       void *cl: A void pointer to a closure struct containing:
 *                       - The methods struct for the UArray2s
 *                       - A UArray2, final, for the rotated image to be stored
 * Return: void
 * Expects
 *     array2 is nonvoid at col, row, and final is initialiazed to size: 
 *                                                              (height, width)
 ************************/

void rotate90 (int col, int row, A2Methods_UArray2 array2, 
                                            A2Methods_Object *ptr, void *cl)
{
        (void) ptr;
        struct closure *temp = cl;
        A2Methods_T methods = temp->methods;
        A2Methods_UArray2 final = temp->final;
        void *location = methods->at(final, methods->height(array2) - 
                                                                row - 1, col);
        *(struct Pnm_rgb *)location = *(struct Pnm_rgb *)methods->at(array2,
                                                                     col,row);
}

/**********rotate180********
 *
 * Mapping function for A2Methods_UArray2, that takes pixels from array2 and
 *       inserted them into a new UArray2 with a 180 degree rotation
 * Inputs:
 *       int col, row: the current index of the map
 *       A2Methods_UArray2 array2: The 2d array that is being mapped
 *       A2Methods_Object: (not used) An A2Methods_Object pointer
 *       void *cl: A void pointer to a closure struct containing:
 *                       - The methods struct for the UArray2s
 *                       - A UArray2, final, for the rotated image to be stored
 * Return: void
 * Expects
 *     array2 is nonvoid at col, row, and final is initialiazed to size: 
 *                                                              (width, height)
 ************************/
void rotate180 (int col, int row, A2Methods_UArray2 array2, 
                                              A2Methods_Object *ptr, void *cl)
{
        (void) ptr;
        struct closure *temp = cl;
        A2Methods_T methods = temp->methods;
        A2Methods_T final = temp->final;
        void *location = methods->at(final, methods->width(array2) - col - 1, 
                                             methods->height(array2) - row - 1);
        *(struct Pnm_rgb *)location = *(struct Pnm_rgb *)methods->at(array2,
                                                                col, row);
}

/**********rotate270********
 *
 * Mapping function for A2Methods_UArray2, that takes pixels from array2 and
 *       inserted them into a new UArray2 with a 270 degree rotation
 * Inputs:
 *       int col, row: the current index of the map
 *       A2Methods_UArray2 array2: The 2d array that is being mapped
 *       A2Methods_Object: (not used) An A2Methods_Object pointer
 *       void *cl: A void pointer to a closure struct containing:
 *                       - The methods struct for the UArray2s
 *                       - A UArray2, final, for the rotated image to be stored
 * Return: void
 * Expects
 *     array2 is nonvoid at col, row, and final is initialiazed to size: 
 *                                                              (height, width)
 ************************/
void rotate270 (int col, int row, A2Methods_UArray2 array2, 
                                                A2Methods_Object *ptr, void *cl)
{
        (void) ptr;
        struct closure *temp = cl;
        A2Methods_T methods = temp->methods;
        A2Methods_T final = temp->final;
        void *location = methods->at(final, row, methods->width(array2) - 
                                                                col - 1);
        *(struct Pnm_rgb *)location = *(struct Pnm_rgb *)methods->at(array2, 
                                                                  col, row);
}

/**********flip_horizontal_map********
 *
 * Mapping function for A2Methods_UArray2, that takes pixels from array2 and
 *       inserted them into a new UArray2 with a horizontal flip
 * Inputs:
 *       int col, row: the current index of the map
 *       A2Methods_UArray2 array2: The 2d array that is being mapped
 *       A2Methods_Object: (not used) An A2Methods_Object pointer
 *       void *cl: A void pointer to a closure struct containing:
 *                       - The methods struct for the UArray2s
 *                       - A UArray2, final, for the rotated image to be stored
 * Return: void
 * Expects
 *     array2 is nonvoid at col, row, and final is initialiazed to size: 
 *                                                              (width, height)
 ************************/
void flip_horizontal_map (int col, int row, A2Methods_UArray2 array2,
                                               A2Methods_Object *ptr, void *cl)
{
        (void) ptr;
        struct closure *temp = cl;
        A2Methods_T methods = temp->methods;
        A2Methods_T final = temp->final;
        void *location = methods->at(final, methods->width(array2) - col - 1, 
                                                                          row);
        *(struct Pnm_rgb *)location = *(struct Pnm_rgb *)methods->at(array2, 
                                                                    col, row);
}

/**********flip_vertical_map********
 *
 * Mapping function for A2Methods_UArray2, that takes pixels from array2 and
 *       inserted them into a new UArray2 with a vertical flip
 * Inputs:
 *       int col, row: the current index of the map
 *       A2Methods_UArray2 array2: The 2d array that is being mapped
 *       A2Methods_Object: (not used) An A2Methods_Object pointer
 *       void *cl: A void pointer to a closure struct containing:
 *                       - The methods struct for the UArray2s
 *                       - A UArray2, final, for the rotated image to be stored
 * Return: void
 * Expects
 *     array2 is nonvoid at col, row, and final is initialiazed to size: 
 *                                                              (width, height)
 ************************/
void flip_vertical_map (int col, int row, A2Methods_UArray2 array2, 
                                            A2Methods_Object *ptr, void *cl)
{
        (void) ptr;
        struct closure *temp = cl;
        A2Methods_T methods = temp->methods;
        A2Methods_T final = temp->final;
        void *location = methods->at(final, col, methods->height(array2) - row 
                                                                          - 1);
        *(struct Pnm_rgb *)location = *(struct Pnm_rgb *)methods->at(array2,
                                                                     col, row);
}

/**********transpose_map********
 *
 * Mapping function for A2Methods_UArray2, that takes pixels from array2 and
 *       inserted them into a new UArray2 with transposed indecies
 * Inputs:
 *       int col, row: the current index of the map
 *       A2Methods_UArray2 array2: The 2d array that is being mapped
 *       A2Methods_Object: (not used) An A2Methods_Object pointer
 *       void *cl: A void pointer to a closure struct containing:
 *                       - The methods struct for the UArray2s
 *                       - A UArray2, final, for the rotated image to be stored
 * Return: void
 * Expects
 *     array2 is nonvoid at col, row, and final is initialiazed to size: 
 *                                                              (width, height)
 ************************/
void transpose_map(int col, int row, A2Methods_UArray2 array2, 
                                        A2Methods_Object *ptr, void *cl)
{
        (void) ptr;
        struct closure *temp = cl;
        A2Methods_T methods = temp->methods;
        A2Methods_T final = temp->final;
        void *location = methods->at(final, methods->width(array2) - col - 1,
                                            methods->height(array2) - row - 1);
        *(struct Pnm_rgb *)location = *(struct Pnm_rgb *)methods->at(array2,
                                                                    col, row);
}
