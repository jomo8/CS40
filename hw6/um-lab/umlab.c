/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

Um_instruction output(Um_register c);

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}

/* Implemented function defs -- three_register and loadval*/
Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction final = 0;
        final = (uint32_t)Bitpack_newu(final, 4, 28, op);
        final = (uint32_t)Bitpack_newu(final, 3, 6, ra);
        final = (uint32_t)Bitpack_newu(final, 3, 3, rb);
        final = (uint32_t)Bitpack_newu(final, 3, 0, rc);

        return final;
}



Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction final = 0;
        final = (uint32_t)Bitpack_newu(final, 4, 28, 13);
        final = (uint32_t)Bitpack_newu(final, 3, 25, ra);
        final = (uint32_t)Bitpack_newu(final, 25, 0, val);
        
        return final;
}


Um_instruction output(Um_register c)
{
        Um_instruction tmp = three_register(OUT, 0, 0, c);
        return tmp;
}



/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void add_test(Seq_T stream)
{
        append(stream, loadval(r1, 0));
        append(stream, loadval(r2, 48));
        append(stream, loadval(r3, 2));
        append(stream, add(r1, r2, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void print_six(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}