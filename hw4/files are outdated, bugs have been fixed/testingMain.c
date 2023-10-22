/*
 *
 *
 * 
 * 
 * 
 */

#include <stdlib.h>
#include <stdbool.h>
#include "helper.h"
#include "assert.h"
#include "a2plain.h"
#include "pnm.h"
#include "mem.h"
#include <math.h>
#include "bitpack.h"

void printInBinary(uint64_t bitPacked);
void printTinyBinary(uint8_t bitPacked);
void print32Binary(uint32_t bitPacked);

// void rgbConversion();
// void rgbCompInversesTest();
// void inverse_quantize_test();

void test_binary_widths();
void test_binary_extraction_unsigned();
void test_binary_extraction_signed();
void test_binary_update_field_unsigned();
void test_bitpacking_ASSERTIONS();
void test_binary_update_field_signed();

void test_building_word();

int main (int argc, char *argv[])
{
        (void) argc;
        (void) argv;
        // uint64_t i;
        // uint32_t j = 0x3f4;
        // // i = j;
        // printInBinary(j);
        // uint64_t test = 0x3f4;
        // printInBinary(test);

        // rgbConversion();
        // rgbCompInversesTest();
        // inverse_quantize_test();

        test_binary_widths();
        test_binary_extraction_unsigned();
        test_binary_extraction_signed();
        test_binary_update_field_unsigned();
        test_binary_update_field_signed();
        
        test_building_word();

        exit(EXIT_SUCCESS);
}


/*****************************************************************************
 *              Testing for the Bitpacking interface
 ****************************************************************************/
void test_binary_widths()
{
        printf("\n\n******Entering binary widths******\n");
        assert(Bitpack_fitsu(3,2) == true);
        assert(Bitpack_fitsu(4, 2) == false);
        assert(Bitpack_fitsu(4, 3) == true);
        assert(Bitpack_fitsu(0, 3) == true);
        printf("        Tests on fitsu passed\n");
        assert(Bitpack_fitss(-1, 2) == true);
        assert(Bitpack_fitss(-4, 3) == true);
        assert(Bitpack_fitss(3, 3) == true);
        assert(Bitpack_fitss(4, 3) == false);
        printf("        Tests on fitss passed\n");
}

void test_binary_extraction_unsigned()
{
        printf("\n\n******Entering binary extraction******\n");
        uint64_t test = 0xaf;
        /* 1010 1111 */
        printInBinary(test);
        printInBinary(Bitpack_getu(test, 4, 4));
        assert(Bitpack_getu(test, 8, 0) == 175);
        assert(Bitpack_getu(test, 7, 1) == 87);
        assert(Bitpack_getu(test, 6, 2) == 43);
        assert(Bitpack_getu(test, 5, 3) == 21);
        assert(Bitpack_getu(test, 4, 4) == 10);
        assert(Bitpack_getu(test, 3, 5) == 5);
        assert(Bitpack_getu(test, 0, 0) == 0);
        printf("        Tests on 0xaf passed\n");
        test = 0xF0; /* 1111 0000 */
        assert(Bitpack_getu(test, 0, 7) == 0);
        assert(Bitpack_getu(test, 1, 7) == 1);
        assert(Bitpack_getu(test, 2, 6) == 3);
        assert(Bitpack_getu(test, 3, 5) == 7);
        assert(Bitpack_getu(test, 4, 4) == 15);
        assert(Bitpack_getu(test, 5, 3) == 30);
        assert(Bitpack_getu(test, 6, 2) == 60);
        assert(Bitpack_getu(test, 7, 1) == 120);
        assert(Bitpack_getu(test, 8, 0) == 240);
        printf("        Tests on 0xf0 passed\n");
}

void test_binary_extraction_signed()
{
        printf("\nEntering binary extraction test\n");
        uint64_t test = 0xaf; /* 1010 1111 */    
        assert(Bitpack_gets(test, 4, 4) == -6);
        assert(Bitpack_getu(test, 4, 4) == 10);
        printf("        Test -6 vs 10 passed (signed vs. unsigned) \n");
        assert(Bitpack_gets(0x3f, 4, 0) == -1);
        assert(Bitpack_getu(0x3f, 4, 0) == 15);
        printf("        Test -1 vs. 15 passed\n");
        assert(Bitpack_gets(0xfafaface, 4, 0) == -2);
        assert(Bitpack_gets(0xfafaface, 4, 4) == -4);
        assert(Bitpack_gets(0xfafaface, 8, 28) == -6);
        // Bitpack_gets(0x3f, 10, 0);
        
}

void test_bitpacking_ASSERTIONS()
{
        /* need to figure out edge cases w assertions, where should func fail */
        printf("\n\n******Entering assertions testing for bitpacking interface"
                "********\n");
        
}

void test_binary_update_field_unsigned()
{
        printf("\n\n******Entering field bitpacking unsigned******\n");
        uint64_t word = 0xf0; /* 0000...0000 1111 0000 */
        unsigned width = 4; 
        unsigned lsb = 4; 
        uint64_t fieldvalue = 0xa; /* 1010 */
        uint64_t newWord = Bitpack_newu(word, width, lsb, fieldvalue);
        assert(newWord == 0xa0);
}

void test_binary_update_field_signed()
{
        printf("\n\n******Entering field bitpacking SIGNED******\n");
        uint64_t word = 0xa0; /* 0000...0000 1010 0000 */
        unsigned width = 4; 
        unsigned lsb = 4;
        int64_t fieldvalue = -1; /* 1111 */
        uint64_t newWord = Bitpack_news(word, width, lsb, fieldvalue);
        assert(newWord == 0xf0);
}

void test_building_word()
{
        printf("\n\n******Entering initial full test******\n");
        uint64_t word = 0x0; /* 0000...0000 */
        unsigned a = 140;
        unsigned width_a = 9;
        unsigned lsb_a = 23;
        word = Bitpack_newu(word, width_a, lsb_a, a);

        int8_t b = 3;
        unsigned width_b = 5;
        unsigned lsb_b = 18;
        word = Bitpack_newu(word, width_b, lsb_b, b);

        int8_t c = 7;
        unsigned width_c = 5;
        unsigned lsb_c = 13;
        word = Bitpack_newu(word, width_c, lsb_c, c);

        int8_t d = 0;
        unsigned width_d = 5;
        unsigned lsb_d = 8;
        word = Bitpack_newu(word, width_d, lsb_d, d);

        uint8_t pB = 15;
        unsigned width_pB = 5;
        unsigned lsb_pB = 4;
        word = Bitpack_newu(word, width_pB, lsb_pB, pB);

        /* Try making these numbers not able to fit */
        uint8_t pR = 13;
        unsigned width_pR = 5;
        unsigned lsb_pR = 0;
        word = Bitpack_newu(word, width_pR, lsb_pR, pR);
        
        print32Binary(word);
}

/*****************************************************************************
 *              Testing for the Helper interface
 ****************************************************************************/

// void rgbConversion()
// {
//         struct Pnm_rgb test1, test2, test3, test4;
//         test1.red = 20;
//         test1.green = 30;
//         test1.blue = 40;
        
//         test2.red = 50;
//         test2.green = 60;
//         test2.blue = 70;

//         test3.red = 80;
//         test3.green = 90;
//         test3.blue = 100;
        
//         test4.red = 110;
//         test4.green = 120;
//         test4.blue = 130;
        
//         UArray_T testBlock = UArray_new(4, sizeof(struct Pnm_rgb));
//         *((Pnm_rgb)UArray_at(testBlock, 0)) = test1;
//         *((Pnm_rgb)UArray_at(testBlock, 1)) = test2;
//         *((Pnm_rgb)UArray_at(testBlock, 2)) = test3;
//         *((Pnm_rgb)UArray_at(testBlock, 3)) = test4;

//         UArray_T testColorSpace = NULL;
//         testColorSpace = rgb_to_comp(testBlock);
//         struct colorSpace color1 = *((struct colorSpace *)UArray_at(testColorSpace, 0));
//         struct colorSpace color2 = *((struct colorSpace *)UArray_at(testColorSpace, 1));
//         struct colorSpace color3 = *((struct colorSpace *)UArray_at(testColorSpace, 2));
//         struct colorSpace color4 = *((struct colorSpace *)UArray_at(testColorSpace, 3));

//         assert(color1.y  == (float)(0.299 * 20.00000000 + 0.587 * 30.0000000 + 0.114 * 40.0000000));
//         assert(color1.pb == (float)(-0.168736 * 20 - 0.331264 * 30 + 0.5 * 40));
//         assert(color1.pr == (float)(0.5 * 20 - 0.418688 * 30 - 0.081312 * 40));
//         assert(color2.y  == (float)(0.299 * 50 + 0.587 * 60 + 0.114 * 70));
//         assert(color2.pb == (float)(-0.168736 * 50 - 0.331264 * 60 + 0.5 * 70));
//         assert(color2.pr == (float)(0.5 * 50 - 0.418688 * 60 - 0.081312 * 70));
//         assert(color3.y  == (float)(0.299 * 80 + 0.587 * 90 + 0.114 * 100));
//         assert(color3.pb == (float)(-0.168736 * 80 - 0.331264 * 90 + 0.5 * 100));
//         assert(color3.pr == (float)(0.5 * 80 - 0.418688 * 90 - 0.081312 * 100));        
//         assert(color4.y  == (float)(0.299 * 110 + 0.587 * 120 + 0.114 * 130));
//         assert(color4.pb == (float)(-0.168736 * 110 - 0.331264 * 120 + 0.5 * 130));
//         assert(color4.pr == (float)(0.5 * 110 - 0.418688 * 120 - 0.081312 * 130));
        
//         UArray_free(&testColorSpace);
// }

// void rgbCompInversesTest()
// {
//         struct Pnm_rgb test1, test2, test3, test4;
//         test1.red = 20;
//         test1.green = 30;
//         test1.blue = 40;
        
//         test2.red = 50;
//         test2.green = 60;
//         test2.blue = 70;

//         test3.red = 80;
//         test3.green = 90;
//         test3.blue = 100;
        
//         test4.red = 110;
//         test4.green = 120;
//         test4.blue = 130;
//         //Would it work with
//         UArray_T testBlock = UArray_new(4, sizeof(struct Pnm_rgb));
//         *((Pnm_rgb)UArray_at(testBlock, 0)) = test1;
//         *((Pnm_rgb)UArray_at(testBlock, 1)) = test2;
//         *((Pnm_rgb)UArray_at(testBlock, 2)) = test3;
//         *((Pnm_rgb)UArray_at(testBlock, 3)) = test4;

//         UArray_T testColorSpace = NULL;
//         testColorSpace = rgb_to_comp(testBlock);

//         UArray_T testBlockTwo = NULL;
//         testBlockTwo = comp_to_rgb(testColorSpace);

//         Pnm_rgb temp1 = (Pnm_rgb)UArray_at(testBlockTwo, 0);
//         assert(temp1->blue == test1.blue);
//         printf("temp1g: %u, test1g: %u\n", temp1->green, test1.green);
//         // /* Assert failing due to rounding error (What is origin of rounding) */
//         // assert(temp1->green == test1.green);
//         // assert(temp1->red == test1.red);
//         // Pnm_rgb temp2 = (Pnm_rgb)UArray_at(testBlockTwo, 1);
//         // assert(temp2->blue == test2.blue);
//         // assert(temp2->green == test2.green);
//         // assert(temp2->red == test2.red);
//         // Pnm_rgb temp3 = (Pnm_rgb)UArray_at(testBlockTwo, 3);
//         // assert(temp3->blue == test3.blue);
//         // assert(temp3->green == test3.green);
//         // assert(temp3->red == test3.red);
//         // Pnm_rgb temp4 = (Pnm_rgb)UArray_at(testBlockTwo, 4);
//         // assert(temp4->blue == test4.blue);
//         // assert(temp4->green == test4.green);
//         // assert(temp4->red == test4.red);

//         UArray_free(&testBlockTwo);
// }

// void inverse_quantize_test()
// {
//         /* Rohunn said never use "struct" in your file */
//         UArray_T testColorSpace = UArray_new(4, sizeof(struct colorSpace));

//         struct colorSpace test1, test2, test3, test4;
//         test1.y  = 0.05;
//         test1.pb = 0.10;
//         test1.pr = 0.15;
        
//         test2.y = 0.2;
//         test2.pb= 0.25;
//         test2.pr = 0.3;

//         test3.y = 0.35;
//         test3.pb= 0.40;
//         test3.pr = 0.45;
        
//         test4.y = 0.5;
//         test4.pb= 0.55;
//         test4.pr = 0.6;

//         *((struct colorSpace*)UArray_at(testColorSpace, 0)) = test1;
//         *((struct colorSpace*)UArray_at(testColorSpace, 1)) = test2;
//         *((struct colorSpace*)UArray_at(testColorSpace, 2)) = test3;
//         *((struct colorSpace*)UArray_at(testColorSpace, 3)) = test4;

//         struct wordElements wordTest;
//         wordTest = perform_dct(testColorSpace);
//         UArray_T inverse = inverse_dct(wordTest);

//         struct colorSpace inverseTest1, inverseTest2, inverseTest3, inverseTest4;
//         inverseTest1 = *(struct colorSpace*)UArray_at(inverse, 0);
//         inverseTest2 = *(struct colorSpace*)UArray_at(inverse, 1);
//         inverseTest3 = *(struct colorSpace*)UArray_at(inverse, 2);
//         inverseTest4 = *(struct colorSpace*)UArray_at(inverse, 3);

//         printf("a %i\n", wordTest.a);
//         printf("b %i\n", wordTest.b);
//         printf("c %i\n", wordTest.c);
//         printf("d %i\n", wordTest.d);

//         printf("inverseTest1: y, pb, pr: %f, %f, %f\n", inverseTest1.y, inverseTest1.pb, inverseTest1.pr); 
//         printf("inverseTest2: y, pb, pr: %f, %f, %f\n", inverseTest2.y, inverseTest2.pb, inverseTest2.pr);
//         printf("inverseTest3: y, pb, pr: %f, %f, %f\n", inverseTest3.y, inverseTest3.pb, inverseTest3.pr);
//         printf("inverseTest4: y, pb, pr: %f, %f, %f\n", inverseTest4.y, inverseTest4.pb, inverseTest4.pr);

//         UArray_free(&inverse);
// }



void printInBinary(uint64_t bitPacked)
{       
        uint64_t i;
        for (i = 1ull << 63; i > 0; i = i / 2) {
                (bitPacked & i) ? printf("1") : printf("0");
        }
        printf("\n");
}

void printTinyBinary(uint8_t bitPacked)
{
        uint64_t i;
        for (i = 1ull << 7; i > 0; i = i / 2) {
                (bitPacked & i) ? printf("1") : printf("0");
        }
        printf("\n");
}

void print32Binary(uint32_t bitPacked)
{
        uint64_t i;
        for (i = 1ull << 31; i > 0; i = i / 2) {
                (bitPacked & i) ? printf("1") : printf("0");
        }
        printf("\n");
}