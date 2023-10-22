#include <stdlib.h>
#include <stdio.h>
#include "a2blocked.h"
#include "pnm.h"
#include "assert.h"
#include "uarray.h"
#include "helper.h"
#include "arith40.h"
#include "mem.h"
#include "bitpack.h"
#include <math.h>

/* What happens if file is like not available or */



/*                      copy_pixels
 *
 * Purpose: Copies the pixels from the original image to a trimmed version of
 *          the original.
 *
 * Inputs: The column and row indices, a UArray2 of the trimmed image, the 
 *         unfilled pixel at the current index, and a closure value (which 
 *         used as the original image).
 *  
 * Outputs: None.
 *      
 * Errors/Assertions: 
 */
void copy_pixels(int col, int row, A2Methods_UArray2 trimmed, 
                        A2Methods_Object *pixel, void *closure) 
{
        (void) trimmed;
        Pnm_ppm originalImagePtr = (Pnm_ppm) closure;
        Pnm_rgb newPixel = (Pnm_rgb)(originalImagePtr->methods->at
                                        (originalImagePtr->pixels, col, row));
        ((Pnm_rgb)pixel)->blue  = newPixel->blue;
        ((Pnm_rgb)pixel)->red   = newPixel->red;
        ((Pnm_rgb)pixel)->green = newPixel->green;
}


/*                      rgb_to_comp_map 
 *
 * Purpose: Converts the pixel at a given index from RGB to component video and
 *          places the new pixel into a new UArray2.
 *
 * Inputs: The column and row indices, a UArray2 of the unfilled CV image, the
 *         unfilled pixel at the current index, and a closure value (which 
 *         used as the trimmed RGB image).
 * 
 * Outputs: None.
 *      
 * Errors/Assertions:
 */
/* IMPORTANT: Might be better to use a small map here instead of voiding */
void rgb_to_comp_map(int col, int row, A2Methods_UArray2 comp_video, 
                                A2Methods_Object *pixel, void *closure) 
{
        (void) comp_video;
        Pnm_ppm originalImagePtr = (Pnm_ppm) closure;
        Pnm_rgb originalpixel = (Pnm_rgb)(originalImagePtr->methods->at
                                        (originalImagePtr->pixels, col, row));
        float denominator = (1.0 * originalImagePtr->denominator);                                                      
        float r = (originalpixel->red   / denominator);
        float g = (originalpixel->green / denominator);
        float b = (originalpixel->blue  / denominator);
        /* Comment */
        colorSpace newPixel;
        /* Calculations from spec */
        newPixel.y  =  (0.299     *  r  + 0.587    * g +  0.114    * b);
        if (newPixel.y < 0) {
                newPixel.y = 0;
        } else if (newPixel.y > 1) {
                newPixel.y = 1;
        }
        newPixel.pb =  (-0.168736 *  r  - 0.331264 * g +  0.5      * b);
        if (newPixel.pb < -0.5) {
                newPixel.pb = -0.5;
        } else if (newPixel.pb > 0.5) {
                newPixel.pb = 0.5;
        }
        newPixel.pr =  (0.5       *  r  - 0.418688 * g -  0.081312 * b);
        if (newPixel.pr < -0.5) {
                newPixel.pr = -0.5;
        } else if (newPixel.pr > 0.5) {
                newPixel.pr = 0.5;
        }
        *(colorSpace*)pixel = newPixel;
}



/*                  comp_to_rgb_map 
 *
 * Purpose: Converts a pixel from component video to RGB and places the new
 *          pixel into the unfilled UArray2 of the RGB pixels. 
 *
 * Inputs: The column and row indices, a UArray2 of the unfilled RGB image, the
 *         unfilled pixel at the current index, and a closure value (which 
 *         used as the CV image).
 *  
 * Outputs: None.
 *      
 * Errors/Assertions:
 */
/* Might be better to use a small map here instead of voiding */
void comp_to_rgb_map(int col, int row, A2Methods_UArray2 rgb, 
                                A2Methods_Object *pixel, void *closure) 
{
        (void) rgb;
        Pnm_ppm originalImagePtr = (Pnm_ppm) closure;
        colorSpace originalpixel = *(colorSpace*)(originalImagePtr->methods->at
                                        (originalImagePtr->pixels, col, row));
        float Y = originalpixel.y;
        float pB = originalpixel.pb;
        float pR = originalpixel.pr;
        Pnm_rgb newPixel = (Pnm_rgb)pixel;
        /* Calculations from spec */
        newPixel->red   = roundBounds(255 * (1.0 * Y + 0.0         * pB + 1.402    * pR), 255);
        newPixel->green = roundBounds(255 * (1.0 * Y - 0.344136    * pB - 0.714136 * pR), 255);
        newPixel->blue  = roundBounds(255 * (1.0 * Y + 1.772       * pB + 0.0      * pR), 255);
}


/*                      round_bounds 
 *
 * Purpose: Rounds a number into the bounds of the max value and 0.
 *
 * Inputs: A float of the number to check and the max value of the input.
 *  
 * Outputs: An integer of the rounded float.
 *      
 * Errors/Assertions: None.
 */
int roundBounds(float num, float maxVal) 
{
        if (num > maxVal) {
                return maxVal;
        } else if (num < 0) {
                return 0;
        } else {
                return num;
        }
}


/*                      comp_to_dct 
 *
 * Purpose: Converts a component video pixel to DCT at a specific index of a 
 *          component video image.
 *
 * Inputs: The col and row of the index, the component video image, a 
 *         colorspace struct from the unfilled DCT image, and a dct_struct 
 *         as the closure element.
 *  
 * Outputs: None.
 *      
 * Errors/Assertions: The col and row are within range of the image dimensions.
 */
void comp_to_dct(int col, int row, A2Methods_UArray2 comp_video, 
                                A2Methods_Object *pixel, void *closure)
{
        (void) comp_video;
        /* Closure holds the UArray "block" and the dct UArray2 */
        dct_struct tmpClosure = *(dct_struct*)closure;
        UArray_T block = tmpClosure.block;

        /* If we're at the end of 2x2 block, we transfer data into dct array */
        if ((row % 2 != 0) && (col % 2 != 0)) {
                /* Insert bottom right at 3 */
                *(colorSpace*)UArray_at(block, 3) = *(colorSpace*)pixel;
                *(wordElements*)(tmpClosure.methods->at(tmpClosure.dct, 
                                (col / 2), (row / 2))) = perform_dct(block);
                /* Do we free and then reallocate? */
        } else {
                /* Insert top left at 0, top right at 2, 
                bottom left at 1, and bottom right at 3 */
                *(colorSpace*)UArray_at(block, ((2 * (col % 2)) + (row % 2))) =
                                                        *(colorSpace*) pixel;
        }
}


/*                      dct_to_comp 
 *
 * Purpose: Converts a DCT pixel to component video at a specific index of a 
 *          DCT image.
 *
 * Inputs: The col and row of the index, a DCT image, a 
 *         colorspace struct from the unfilled DCT image, and a dct_struct 
 *         as the closure element.
 *  
 * Outputs: None.
 *      
 * Errors/Assertions: The col and row are within range of the image dimensions.
 */
void dct_to_comp(int col, int row, A2Methods_UArray2 comp_video, 
                                A2Methods_Object *pixel, void *closure)
{
        (void) comp_video;
        /* Closure holds the UArray "block" and the dct UArray2 */
        dct_struct tmpClosure = *(dct_struct*)closure;
        struct wordElements tmp = *(wordElements*)(tmpClosure.methods->at
                                (tmpClosure.dct, (col / 2), (row / 2)));
        *(colorSpace*) pixel = inverse_dct(tmp, (2 * (col % 2)) + (row % 2));
        // colorSpace test = *(colorSpace*) pixel;
        // printf("dct_to_comp : pixel values after decompression: y %f, pb %f, pr %f\n", test.y, test.pb, test.pr);
}


/*                      perform_dct 
 *
 * Purpose: Performs inverse functions on a UArray of structs that contain the
 *          CV pixel values.
 *
 * Inputs: A UArray block of colorSpace structs that contain the values for a
 *         component video pixel.
 *  
 * Outputs: A wordElements struct that contains elements of a compressed block
 *          of CV pixels.
 *      
 * Errors/Assertions:
 */
wordElements perform_dct(UArray_T block)
{
        assert(UArray_length(block) == 4);
        wordElements newWord;
        /* y1 is topLeft, y2 is topRight, y3 is bottomLeft, y4 is bottomRight */
        colorSpace y1 = (*(colorSpace*)UArray_at(block, 0));
        colorSpace y3 = (*(colorSpace*)UArray_at(block, 1)); 
        colorSpace y2 = (*(colorSpace*)UArray_at(block, 2));
        colorSpace y4 = (*(colorSpace*)UArray_at(block, 3));
        
        float a = (y4.y + y3.y + y2.y + y1.y) / 4.0;
        float b = (y4.y + y3.y - y2.y - y1.y) / 4.0;
        float c = (y4.y - y3.y + y2.y - y1.y) / 4.0;
        float d = (y4.y - y3.y - y2.y + y1.y) / 4.0;
        
        newWord.a = (a * 511.0); /* What rounding occurs naturally here? */
        newWord.b = coefficient_to_index(b);
        newWord.c = coefficient_to_index(c);
        newWord.d = coefficient_to_index(d);
        

        float avg = (y1.pb + y2.pb + y3.pb + y4.pb) / 4.0;
        newWord.pBavg = Arith40_index_of_chroma(avg);

        avg = (y1.pr + y2.pr + y3.pr + y4.pr) / 4.0;
        newWord.pRavg = Arith40_index_of_chroma(avg);

        return newWord;
}


/*                      inverse_dct 
 *
 * Purpose: Performs functions to invert a DCT object into values for a
 *          component video pixel.
 *
 * Inputs: A wordElements struct, which contains the values of a compressed CV
 *         pixel (after the DCT functions are applied).
 *  
 * Outputs: A colorSpace struct, which contains the values of a component
 *          video pixel.
 *      
 * Errors/Assertions:
 */
colorSpace inverse_dct(wordElements word, int index)
{
        float a = (word.a / 511.0); /* What rounding occurs naturally here */
        float b = index_to_coefficient(word.b);
        float c = index_to_coefficient(word.c);
        float d = index_to_coefficient(word.d);
        float pB = Arith40_chroma_of_index(word.pBavg);
        float pR = Arith40_chroma_of_index(word.pRavg);

        colorSpace curr;
        curr.pb = pB;
        curr.pr = pR;
        /* Calculations outlined in spec */
        if (index == 0) {
                curr.y = (a - b - c + d); /* y1 */ /* make sure these are floats*/
        } else if (index == 2) {
                curr.y = (a - b + c - d); /* y2 */
        } else if (index == 1) {
                curr.y = (a + b - c - d); /* y3 */     
        } else if (index == 3) {
                curr.y = (a + b + c + d); /* y4 */
        }

        return curr;
}


/*                      index_to_coefficient 
 *
 * Purpose: Finds the index from the given coefficient. The function is used to
 *          convert from the [-15,15] range to the [-0.3,0.3] range.
 *
 * Inputs: An integer value of the index.
 *  
 * Outputs: A float of the coefficient.
 *      
 * Errors/Assertions:
 */
float index_to_coefficient(int index)
{
        float coefficient = (index / 50.0);
        return coefficient;
}


/*                      coefficient_to_index 
 *
 * Purpose: Finds the coefficient from the given index. The function is used to
 *          convert from the [-0.3, 0.3] range to the [-15, 15] range.
 *
 * Inputs: A float of the coefficient.
 *  
 * Outputs: An integer value of the index.
 *      
 * Errors/Assertions:
 */
signed coefficient_to_index(float var)
{
        if (var > 0.3) {
                var = 15;
        } else if (var < -0.3) {
                var = -15;
        } else {
                var *= 50.0;
        } 
        signed var_s = var;
        if (var - var_s > 0.5) {
                var_s += 1;
        }
        return var_s;
}


/*                      store_words 
 *
 * Purpose: Packs the bytes from the given DCT information from the
 *          closure value into a single word.
 *
 * Inputs: The col and row of the index, a copy of the DCT image, a single 
 *         DCT object as an A2Methods_Object, and the closure is used to read
 *         the file input. 
 *  
 * Outputs: None.
 *      
 * Errors/Assertions:
 */
void store_words(int col, int row, A2Methods_UArray2 dct, 
                                A2Methods_Object *pixel, void *closure)
{
        (void) col;
        (void) row;
        (void) dct;
        char byte;
        uint32_t word = 0;
        for (int i = 24; i >= 0; i -= 8) {
                byte = fgetc((FILE*)closure);
                word = Bitpack_news(word, 8, i, byte);
        }
        *(uint32_t*) pixel = word;
}


/*                      print_words 
 *
 * Purpose: Puts the resulting word as a char into the output file, 
 *          which is stdout.
 *
 * Inputs: The col and row of the element, a UArray2 of the array of words, 
 *         a pointer to the current word at the index, and the closure element 
 *         is not used.
 *  
 * Outputs: All output is printed to the specified output.
 *      
 * Errors/Assertions:
 */
void print_words(int col, int row, A2Methods_UArray2 wordArray, 
                        A2Methods_Object *word, void *closure) 
{
        (void) wordArray;
        (void) closure;
        (void) col;
        (void) row;

        uint32_t curr = *(uint32_t*) word;

        for (int i = 24; i >= 0; i -= 8) {
                putchar((curr >> i) & 0xFF); /* Make sure it's writing in Big-E order */
        }
}


/*                         word_to_dct
 *
 * Purpose: Converts a word to a DCT object, and updates the word elements.
 *
 * Inputs: The col and row of the index, a UArray2 of the word array,
 *         a pointer to the desired word, and the closure is used to 
 *         include the DCT array.
 *  
 * Outputs: None.
 *      
 * Errors/Assertions:
 */
void word_to_dct(int col, int row, A2Methods_UArray2 tmp, 
                                A2Methods_Object *word, void *closure)
{
        (void) tmp;
        /* rename dis variable */
        dct_struct myClosure = *(dct_struct*)closure;
        *(wordElements*)(myClosure.methods->at(myClosure.dct, col, row))
                                                = unpack_word(*(uint32_t*)word);
}


/*                      unpack_word 
 *
 * Purpose: Unpacks a given packed word and places the separated values into 
 *          a wordElements struct.
 *
 * Inputs: A uint32_t of the packed word.
 *  
 * Outputs: A wordElements struct that contains the extracted numerical values.
 *      
 * Errors/Assertions:
 */
wordElements unpack_word(uint32_t input)
{
        /* What should we do if we're given an invalid word, i.e. one that has
                -16 for b, what should we do? 
                What are other edge cases for words (invalid ones)? */
        wordElements unpacked;
        uint64_t tmpWord = (uint64_t) input;
        unpacked.pRavg = Bitpack_getu(tmpWord, 4, 0);
        unpacked.pBavg = Bitpack_getu(tmpWord, 4, 4);
        unpacked.a = Bitpack_getu(tmpWord, 9, 23);
        unpacked.b = Bitpack_gets(tmpWord, 5, 18);
        unpacked.c = Bitpack_gets(tmpWord, 5, 13);
        unpacked.d = Bitpack_gets(tmpWord, 5, 8);

        return unpacked;
}


/*                         dct_to_word 
 *
 * Purpose: Transforms DCT objects into a bit-packed word object.
 *
 * Inputs: The col and row of the index, a UArray2 of the word values, a 
 *         pointer to the current DCT object, and the closure is used to 
 *         include the DCT array.
 *  
 * Outputs: None
 *      
 * Errors/Assertions:
 */
void dct_to_word(int col, int row, A2Methods_UArray2 tmp, 
                                A2Methods_Object *fields, void *closure)
{
        (void) tmp;
        dct_struct myClosure = *(dct_struct*)closure;
        uint32_t word = build_word(*(wordElements*)fields);
        *(uint32_t*)myClosure.methods->at(myClosure.dct, col, row) = word;
}


/*                      build_word 
 *
 * Purpose: Build a bit-packed 32bit word given the desired elements.
 *
 * Inputs: A wordElements struct that contains all of the needed 
 *         values to pack.
 *  
 * Outputs: An unsigned 32bit integer that contains all of the packed bits with
 *          their respective LSBs and widths.
 *      
 * Errors/Assertions:
 */
uint32_t build_word(wordElements fields)
{
        uint32_t word = 0; /* 0000...0000 */
        /* is there a more clean/efficient way of doing this */
        word = Bitpack_newu(word, 9, 23, fields.a);      /* Packing a */
        word = Bitpack_news(word, 5, 18, fields.b);      /* Packing b */
        word = Bitpack_news(word, 5, 13, fields.c);      /* Packing c */
        word = Bitpack_news(word, 5, 8, fields.d);       /* Packing d */
        word = Bitpack_newu(word, 4, 4, fields.pBavg);   /* Packing index(pB) */
        word = Bitpack_newu(word, 4, 0, fields.pRavg);   /* Packing index(pR) */

        return word;
}