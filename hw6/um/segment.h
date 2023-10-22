/*              segment.h
 * Joseph Montalto (jmonta04) and Manpreet Kaur (mkaur08)
 * date: 4/14/2023
 * assignment: HW6: um 
 * 
 * summary: Interface for creating memory segments within the program. 
*/

#ifndef _SEGMENT_H_
#define _SEGMENT_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include "seq.h"


#define S segmentData
typedef struct S *S;


extern segmentData start_program(FILE *input);
extern void end_program(segmentData seg);
extern Seq_T store_program(Seq_T allSegments, FILE *input);
extern Seq_T create_segment(Seq_T segments, int len);
extern uint32_t map_segment(segmentData seg, uint32_t rC);
extern Seq_T get_program(segmentData prog);
extern int check_unmapped(segmentData prog, int val);
extern segmentData replace_program(segmentData tmp, Seq_T prog);
extern segmentData unmap_segment(segmentData seg, uint32_t rB);
extern segmentData replace_zero_seg(segmentData seg, uint32_t rB);



extern void printInBinary(uint32_t bitPacked); /* This is for testing only */
extern void printSegment(Seq_T segment);


#undef S
#endif