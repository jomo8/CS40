/*              registers.h
 * Joseph Montalto (jmonta04) and Manpreet Kaur (mkaur08)
 * date: 4/14/2023
 * assignment: HW6: um 
 * 
 * summary: Interface for creating registers and running the program. 
 * Uses functionality from segment.h 
*/


#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 
#include <seq.h>
#include "segment.h"


extern Seq_T create_registers();
extern void run_program(segmentData prog);
extern struct um read_instruction(uint32_t inst, struct um final);
extern void read_segment(Seq_T segment);
extern struct um conditional_move(struct um final, uint32_t vals);
extern struct um segmented_load(struct um final, uint32_t vals);
extern struct um segmented_store(struct um final, uint32_t vals);
extern struct um addition(struct um final, uint32_t vals);
extern struct um multiplication(struct um final, uint32_t vals);
extern struct um division(struct um final, uint32_t vals);
extern struct um bitwiseNAND(struct um final, uint32_t vals); 
extern struct um map(struct um final, uint32_t vals);
extern struct um unmap(struct um final, uint32_t vals);
extern void output_instruction(struct um final, uint32_t vals);
extern struct um input(struct um final, uint32_t vals);
extern struct um load_program(struct um final, uint32_t vals); 
extern struct um load_value(struct um final, uint32_t inst); 
extern void destroy_program(struct um final);

#endif