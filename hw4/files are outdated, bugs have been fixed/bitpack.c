#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "a2blocked.h"
#include "pnm.h"
#include "assert.h"
#include "uarray.h"
#include "bitpack.h"
#include "arith40.h"
#include "mem.h"
#include "except.h" //Do we need to include this?

Except_T Bitpack_Overflow = { "Overflow packing bits" };
/*      
 *  -What happens when we shift by 64?    
 *  -Hanson assertions (from assert.h) to ensure that shift values 
 *                      are for ≤ 64 bits, that widths are ≤ 64
 *  -Where do we need to use width test functions?
 *  -Fields of width zero are defined to contain the value zero
 *  -It should be a checked run-time error to call Bitpack getu or Bitpack gets
 *      with a width w that does not satisfy 0 ≤ w ≤ 64. Similarly, it 
 *      should be a checked run-time error to call Bitpack getu
 *      or Bitpack gets with a width w and lsb that do not satisfy w + lsb ≤ 64
 *  -We should definitely test Bitpack with 64 bit stuff
 * 
 *  Edge cases:
 *      
 * 
 * From spec we might wanna test this:
 * If no exception is raised and no checked run-time error occurs, 
 * then Bitpack getu and Bitpack newu satisfy the mathematical laws you would expect, for example,
 *      Bitpack_getu(Bitpack_newu(word, w, lsb, val), w, lsb) == val
 * A more subtle law is that if lsb2 >= w + lsb, then
 *      getu(newu(word, w, lsb, val), w2, lsb2) == getu(word, w2, lsb2)
 */

/*                      Bitpack_fitsu 
 *
 * Purpose: Check if the given 64-bit unsigned integer can fit in the 
 *          provided width.
 *
 * Inputs: A 64-bit unsigned integer and an unsigned width to check with. 
 *  
 * Outputs: True if the unsigned integer fits in the width, and false 
 *          otherwise.
 *      
 * Errors/Assertions: Both parameters to be unsigned, or else there will
 *                    be undefined behavior.
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        uint64_t numInts = ((uint64_t)1 << width);
        // printf("Unsigned numInts: %lu\n", numInts);
        if (n >= numInts || (width == 0 && n != 0)) {
                return false;
        } else {
                return true;
        }
}

/*                     Bitpack_fitss 
 *
 * Purpose: Check if the given 64-bit signed integer can fit in the 
 *          provided width.
 *
 * Inputs: A 64-bit signed integer and an unsigned width to check with. 
 *  
 * Outputs: True if the signed integer fits in the width, and false otherwise.
 *      
 * Errors/Assertions: The width must be unsigned, or else there will be 
 *                    undefined behavior.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        /* Shoudl we use unsigned fits here? or vice versa */
        assert(width <= 64);
        int64_t numInts = ((int64_t)1 << (width - 1));
        // printf("Signed numInts: %lu\n", numInts);
        if (n >= numInts || n < (numInts * -1) || (width == 0 && n != 0)) {
                return false;
        } else {
                return true;
        }
}


/*                      Bitpack_getu 
 *
 * Purpose: Get the bits of the given width starting at the specified least
 *          significant bit.
 *
 * Inputs: A uint64_t "word" that contains the desired bits, the width of the
 *         "word" that needs to be extracted, and the index of the least 
 *         significant bit
 *  
 * Outputs: A uint64_t representation of the extracted "word".
 *      
 * Errors/Assertions: 
 */
/* Currently unsure of how we should return the field */
/* Should we return it shifted all the way right, so its lsb is 0? */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        /* Do we need to use the width test functions? */
        assert(width <= 64); /* since w is unsigned, do we need to check if it's negative? */
        assert((width + lsb) <= 64); /* Some machine designs, such as the late, unlamented HP PA-RISC, provided these operations using one machine
instruction apiece ? <-- from spec */
        if (width == 0) {
                return 0;
        }
        uint64_t mask = ~0;
        uint64_t field = mask >> (64 - width);
        field = field << lsb;
        field = field & word;
        field = field >> lsb;
        return (field);
}


/*                  Bitpack_gets
 *
 * Purpose: Get the bits of the given width starting at the specified least
 *          significant bit.
 *
 * Inputs: A uint64_t "word" that contains the desired bits, the width of the
 *         "word" that needs to be extracted, and the index of the least 
 *         significant bit
 *  
 * Outputs: A uint64_t representation of the extracted "word".
 *      
 * Errors/Assertions: 
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        int64_t field = Bitpack_getu(word, width, lsb);
        /* Checks signed bit of the field */
        if (Bitpack_getu(word, 1, (lsb + width) - 1) == 1) {
                /* Not really sure if this is what they want us to do */
                int64_t mask = ~0 >> width;
                mask = mask << width;
                field = mask | field;
        }
        return field;
}


/*                      Bitpack_newu 
 *
 * Purpose: Creates a new word with the given indices of a packed word. The
 *          bits of the given width and least significant bit index are 
 *          extracted and returned.
 *
 * Inputs: A "word" to replace a bit field of the given width and lsb with 
 *         information from value.
 *  
 * Outputs: The word with a replaced bit field.
 *      
 * Errors/Assertions: Will CRE if the value does not fit within the width
 *                    unsigned bits.
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                        uint64_t value) 
{
        if (Bitpack_fitsu(value, width) == false) {
                RAISE(Bitpack_Overflow);
        }
        assert(width <= 64);
        assert((width + lsb) <= 64);

        /* Get a word that has all 1's except where field is */
        uint64_t mask = ~0 >> (lsb - 1);
        mask = mask << (64 - width);
        mask = mask >> (65 - width - lsb); /* >> 64 - width << (lsb - 1) */
        mask = ~mask;
        word = (mask & word);

        value = value << lsb;

        return (word | value);
}


/*                      Bitpack_news 
 *
 * Purpose: Creates a new word with the given indices of a packed word. The
 *          bits of the given width and least significant bit index are 
 *          extracted and returned.
 *
 * Inputs: A "word" to replace a bit field of the given width and lsb with 
 *         information from value.
 *  
 * Outputs: The word with a replaced bit field.
 *      
 * Errors/Assertions: Will CRE if the value does not fit within the width
 *                    signed bits.
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                        int64_t value)
{
        /* Not sure if this is the right way to do this */
        if (Bitpack_fitss(value, width) == false) {
                RAISE(Bitpack_Overflow);
        }
        /* do we assert here */
        // printf("Value %li, width %u\n", value, width);
        /* when we call this, we also call fitsU, does that matter */
        uint64_t newVal = signed_to_unsigned(value, width);

        // printf("New Value %lu, width %u\n", newVal, width);
        return Bitpack_newu(word, width, lsb, newVal);
}


/*                  signed_to_unsigned
 *
 * Purpose: Converts a signed 64-bit word into an unsigned 64-bit word.
 *
 * Inputs: The signed word to transform and the width of its bit field.
 *  
 * Outputs: An unsigned word with the same bits and width.
 *      
 * Errors/Assertions: If width is greater than 64, there will be undefined
 *                    behavior.
 */
uint64_t signed_to_unsigned(int64_t value, unsigned width)
{
        /* This assumes that the value is shifted all the way right */
        uint64_t mask = ~0;
        mask = mask >> (64 - width);
        value = mask & value; /* Turns signed bits off that aren't in field */
        return value;
}


/*                  unsigned_to_signed
 *
 * Purpose: Converts an unsigned 64-bit word into a signed 64-bit word.
 *
 * Inputs: The unsigned word to transform and the width of its bit field.
 *  
 * Outputs: A signed word with the same bits and width.
 *      
 * Errors/Assertions: If width is greater than 64, there will be undefined
 *                    behavior.
 */
int64_t unsigned_to_signed(uint64_t value, unsigned width)
{
        int64_t mask = ~0 >> width;
        mask = mask << width;
        value = mask | value;
        return value;
}






/*************** THE FUNCTION BELOW IS NEVER USED *************************/


/*                      
 *
 * Purpose:
 *
 * Inputs:
 *  
 * Outputs:
 *      
 * Errors/Assertions:
 */
int64_t leftshifts(uint64_t value, unsigned width)
{
        int64_t mask = ~0 >> width;
        mask = mask << width;
        value = mask | value;
        return value;
}