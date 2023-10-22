#include "assert.h"
#include "compress40.h"
#include "helper.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"
#include <stdlib.h>
#include <stdio.h>

/********************************************************
 *               TESTING                   
 *   Current: 2 steps deep (to dct then back)
 *      Memory not getting freed cus of the UArrays. 
 *      Also ppm diff value is 0.08 which is high
 *      Need to clean up the tmpDeclosure vs. Pnm_ppm image stuff
 *      **Map_inverse_dct not working properly.
 ********************************************************/


/* f the supplied file is too short (i.e., the number of codewords is too low for the stated
width and height, or the last one is incomplete), you should fail with a Checked Runtime Error. */

void compress40(FILE *input) /* testing main for now */
{
        A2Methods_T methods = uarray2_methods_blocked;
        struct Pnm_ppm *image;
        image = Pnm_ppmread(input, methods);

        /* *****************************************************
                        Trim the original image 
        ******************************************************** */
        int height = image->height - (image->height % 2);
        int width  = image->width  - (image->width % 2);

        /* the blocksizes of these don't need to be 2 */
        /* could make a trim function? */
        A2Methods_UArray2 trimmed = methods->new_with_blocksize(width, height,
                image->methods->size(image->pixels), 2);
        image->height = height;
        image->width = width;
        image->methods->map_default(trimmed, copy_pixels, image);
        image->methods->free(&(image->pixels));
        image->pixels = trimmed; 

        /* *****************************************************
                        RGB to Component Video 
        ******************************************************** */
        A2Methods_UArray2 comp_vid = methods->new_with_blocksize(width, height,
                                                (sizeof(struct colorSpace)), 2);
        image->methods->map_default(comp_vid, rgb_to_comp_map, image);
        image->methods->free(&(image->pixels));
        image->pixels = comp_vid; /* Not sure that we need to do this
                Or maybe it'd make more sense if we passed image as closure*/


        /* *****************************************************
                        Component Video to DCT 
        ******************************************************** */
        A2Methods_UArray2 dct = methods->new((width / 2), (height / 2), 
                                        (sizeof(struct wordElements)));
        dct_struct tmpClosure;
        tmpClosure.block = UArray_new(4, sizeof(struct colorSpace));
        tmpClosure.methods = uarray2_methods_blocked;
        tmpClosure.dct = dct;
        image->methods->map_default(comp_vid, comp_to_dct, &tmpClosure);
        UArray_free(&(tmpClosure.block));

        /* *****************************************************
                                DCT to word
        ******************************************************** */
        A2Methods_UArray2 words = methods->new((width / 2), (height / 2), 
                                                (sizeof(uint32_t)));
        tmpClosure.methods = uarray2_methods_blocked;
        tmpClosure.dct = words;
        image->methods->map_default(dct, dct_to_word, &tmpClosure);

        
        
        /****************************************************
                        Words to stdout
        *****************************************************/
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        image->methods->map_default(words, print_words, NULL);
        
        Pnm_ppmfree(&image);

}




/* reads compressed image, writes PPM */
void decompress40(FILE *input)
{
        /*******************************************************
                        DECOMPRESSION STEPS 
        ********************************************************/
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                                                        &width, &height);
        assert(read == 2);
        int c = getc(input);
        // printf("c: %c\n", c);
        assert(c == '\n'); /* can we combine these 2, maybe getc raise error */
        struct Pnm_ppm *image;
        NEW(image);
        image->height = height;
        image->width = width;
        image->methods = uarray2_methods_blocked;
        image->denominator = 255; /* We could make a function for this stuff */
        /****************************************************
                Read words in from file / stdin 
        *****************************************************/
        A2Methods_UArray2 wordBits = image->methods->new(width / 2, height / 2,
                                                        sizeof(uint32_t));
        image->methods->map_default(wordBits, store_words, input);

        /****************************************************
                        Convert words to dct
        *****************************************************/
        
        A2Methods_UArray2 word_elements = image->methods->new(width / 2, 
                                           height / 2, sizeof(wordElements));
        dct_struct tmpClosure;
        tmpClosure.methods = uarray2_methods_blocked;
        tmpClosure.dct = word_elements;
        image->methods->map_default(wordBits, word_to_dct, &tmpClosure);

        /****************************************************
                        From dct to comp video 
        *****************************************************/
        /* Could make helper functions to create these UArrays so we don't
                have to disclose struct? */
        A2Methods_UArray2 inverse_dct = image->methods->new_with_blocksize
                                (width, height, (sizeof(struct colorSpace)), 2);
        dct_struct tmpdeClosure;
        tmpdeClosure.methods = uarray2_methods_blocked;
        tmpdeClosure.dct = word_elements;
        image->methods->map_default(inverse_dct, dct_to_comp, &tmpdeClosure);
        image->pixels = inverse_dct;

        /****************************************************
                        From comp video to rgb 
        *****************************************************/
        A2Methods_UArray2 rgb = image->methods->new_with_blocksize(width, height,
                                                (sizeof(struct Pnm_rgb)), 2);
        image->methods->map_default(rgb, comp_to_rgb_map, image);
        image->methods->free(&(image->pixels));
        image->pixels = rgb;
        
        Pnm_ppmwrite(stdout, image);
        /* Haven't freed all the memory */
        image->methods->free(&word_elements);
        image->methods->free(&wordBits);
        Pnm_ppmfree(&image);
}