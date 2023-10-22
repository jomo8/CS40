/*              segment.c
 * Joseph Montalto (jmonta04) and Manpreet Kaur (mkaur08)
 * date: 4/14/2023
 * assignment: HW6: um 
 * 
 * summary: Creates memory segments within the program. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h> 
#include <stdint.h> 
#include <seq.h>
#include <mem.h>
#include <except.h>
#include <bitpack.h>
#include "segment.h"

/* Private data structure with sequences for unmapped segment and program.*/
struct segmentData {
        Seq_T program;
        Seq_T unmapped;
};

/********** start_program ********
 *
 * Creates a 2D array of sequences in order to store the 
 * program code and any memory allocated by the program.
 * Inputs:
 *		File *input: input after reading the given um file. 
 * Return: A pointer to the new data structure segmentData. 
 * Expects
 *     		None.
 * Notes:
 * CRE if struct program is NULL and file input is NULL.  
 ************************/
segmentData start_program(FILE *input)
{
        segmentData newProg = malloc(sizeof(*newProg));
        int len = 50;
        newProg->program = Seq_new(len);
        newProg->unmapped = Seq_new(len - 1);

        assert(newProg->program != NULL);
        assert(input != NULL);

        for (int i = 0; i < len; i++) {
                Seq_addhi(newProg->program, Seq_new(len));
                if (i > 0) {
                        Seq_addhi(newProg->unmapped, (void *)(uintptr_t)i);
                }
        }

        newProg->program = store_program(newProg->program, input);



        return newProg;
}


/********** store_program ********
 *
 * Initializes the zero segment and length and initializes given input to
 * to define and create all the segments. 
 * Inputs:
 *		Seq_T allSegments: every segment  from the initial input
 *              FILE *input: file information being read. 
 * Return: Stores every segment together within a sequence. 
 * Expects
 *     		None.
 * Notes:
 * CRE if file input is NULL.  
 ************************/
Seq_T store_program(Seq_T allSegments, FILE *input)
{
        assert(input != NULL); 
        Seq_T zeroSeg = Seq_get(allSegments, 0);
        int length = 32, width = 8;
        uint32_t id = 0;
        int tmp = fgetc(input);
        for (int i = 0, j = 1; tmp != EOF; i+=8, j++) {
                id |= Bitpack_newu(id, sizeof(tmp) * 8, length - width - 
                                                                (i % 32), tmp);


                if (j % 4 == 0) {
                        Seq_addhi(zeroSeg, (void *)(uintptr_t)id);
                        id = 0;
                }
                tmp = fgetc(input);
        }

        return allSegments;
}

/********** create_segment ********
 *
 * Create a new segment within the sequence of segments. 
 * Inputs:
 *		Seq_T segments: every segment being intialized. 
 *              int len: length of the newly initialize segment sequence. 
 * Return: A new segment within the program. 
 * Expects
 *     		None.
 * Notes:
 *              None.  
 ************************/
Seq_T create_segment(Seq_T segments, int len)
{
        Seq_addhi(segments, Seq_new(len));

        return segments;
}

/********** map_segment ********
 *
 * Create a new segment with a number of words equal to value in r[C]
 * Inputs:
 *		segmentData seg: the segment based on the program data 
 *              unnt32_t rC: register C defined to place segments into.  
 * Return: An index of which segment it is within the program. 
 * Expects
 *     		None.
 * Notes:
 *              None.  
 ************************/
uint32_t map_segment(segmentData seg, uint32_t rC)
{
        int index = (int)(uintptr_t)Seq_remlo(seg->unmapped);
        Seq_T tmp = Seq_new(rC);
        for (uint32_t i = 0; i < rC; i++) {
                Seq_addhi(tmp, (void *)(uintptr_t)0);
        }
        Seq_put(seg->program, index, tmp);


        return (uint32_t)index; 
}

/********** unmap_segment ********
 *
 * Segment current at r[C] becomes unmapped. 
 * Inputs:
 *		segmentData seg: the segment based on the program data 
 *              unnt32_t rB: register B defined to place new segments into.  
 * Return: A new segment within the program. 
 * Expects
 *     		None.
 * Notes:
 *              None.  
 ************************/
segmentData unmap_segment(segmentData seg, uint32_t rB)
{
        Seq_addhi(seg->unmapped, (void *)(uintptr_t)rB);

        return seg;
}


/********** end_program ********
 *
 * Frees all the intialized data from the sequences created and malloc. 
 * Inputs:
 *		segmentData seg: the segment based on the program data  
 * Return: None. 
 * Expects
 *     		None.
 * Notes:
 *              None.  
 ************************/
void end_program(segmentData seg)
{
        int len = Seq_length(seg->program);
        
        for (int i = 0; i < len; i++) {
                // fprintf(stderr, "entering if statement\n");
                Seq_T tmp = Seq_get(seg->program, i);
                Seq_free((&tmp));
        }
        // fprintf(stderr, "last free\n");
        
        Seq_free(&seg->program);
        Seq_free(&seg->unmapped);
        free(seg);
}

/********** get_program ********
 *
 * Retrieves information from the segment data for the entire program.  
 * Inputs:
 *		segmentData prog: the segment based on the program data  
 * Return: All segments based on the words. 
 * Expects
 *     		None.
 * Notes:
 *              None.  
 ************************/
Seq_T get_program(segmentData prog)
{
        return prog->program;
}

/********** check_unmapped ********
 *
 * Check if a segment has been unmapped.   
 * Inputs:
 *		segmentData prog: the segment based on the program data 
 *              int val: value of the word 
 * Return: 0 or 1, true or false to indicate unmapped. 
 * Expects
 *     		None.
 * Notes:
 *              None.  
 ************************/
int check_unmapped(segmentData prog, int val)
{
        int len = Seq_length(prog->unmapped);
        for (int i = 0; i < len; i++) {
                int tmp = (int)(uintptr_t)Seq_get(prog->unmapped, i);
                if (val == tmp) {
                        return 1;
                }
        }
        return 0;
}

/********** replace_program ********
 *
 * Retrieves information from the segment data and places within temporary
 * storage. May be needed for initalizing segments.  
 * Inputs:
 *		segmentData tmp: the segment based on the program data  
 *              Seq_T prog: The original data that is stored from the segments. 
 * Return:  Program stored within temporary segment.  
 * Expects
 *     		There must be initially segment data that is stored.
 * Notes:
 *              None.  
 ************************/
segmentData replace_program(segmentData tmp, Seq_T prog)
{
        tmp->program = prog;

        return tmp;
}

/********** replace_zero_seg ********
 *
 * Replaces the zero segment with the following word information after 
 * retrieving the first word. 
 * Inputs:
 *		segmentData seg: the segment based on the program data  
 *              uint32_t rB:  register B which is where new segment is mapped. 
 * Return: Next word that replaces zero segment 
 * Expects
 *     		There must be initially segment data within zero seg
 * Notes:
 *              None.  
 ************************/
segmentData replace_zero_seg(segmentData seg, uint32_t rB)
{
        Seq_T program = seg->program;

        Seq_put(program, 0, Seq_get(program, rB));
        replace_program(seg, program);

        return seg;
}

/********** printInBinary ********
 *
 * Testing function used to see if binary representation of values for 
 * each operation. 
 * Inputs: 
 *              uint32_t bitPacked: iteration based on the segment. 
 * Return: None.  
 * Expects
 *     		None. 
 * Notes:
 *              None.  
 ************************/
void printInBinary(uint32_t bitPacked)
{       
        uint32_t i;
        printf("\n");
        for (i = 1ull << 31; i > 0; i = i / 2) {
                (bitPacked & i) ? printf("1") : printf("0");
        }
        printf("\n");
}

/********** printSegment ********
 *
 * Testing function to make sure segment is being printed. 
 * Inputs: 
 *              Seq_T segment: segment being stored. 
 * Return: None.  
 * Expects
 *     		None. 
 * Notes:
 *              None.  
 ************************/
void printSegment(Seq_T segment)
{
        int len = Seq_length(segment);
        for (int i = 0; i < len; i++) {
                printInBinary((uint32_t)(uintptr_t)Seq_get(segment, i));
                
        }
}

