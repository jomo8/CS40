/*
 * Implementation for the UM segmented memory, which includes functions for
 * allocation/deallocation of memory, mapping/unmapping segments, and
 * interacting with segments (get/put)
 *
 */

#include <assert.h>
#include <stdlib.h>


#include "memory.h"
#include "assert.h"
#include "seq.h"
#include "uarray.h"


/* THIS IS FOR TESTING */
#include <stdio.h>




#define HINT 100000

/* Name: memory_new
 * Input: a uint32_t representing the length of segment zero
 * Output: A newly allocated Memory_T struct
 * Does: * Creates a Sequence of UArray_T's, sets all segments to NULL besides
 *         segment 0, and segment 0 is initialized to be "length" long
         * Creates a Sequence of uint32_t pointers and 
           sets them to be the index of the unmapped segments
 * Error: Asserts if memory is not allocated
 */
Memory_T *memory_new(uint32_t length)
{
        Memory_T *m_new = (Memory_T *)malloc(sizeof(Memory_T));
        m_new->segments = (Row_T *) malloc(HINT * sizeof(Row_T));
        assert(m_new->segments != NULL);
        m_new->colNum = HINT;
        m_new->fullIndex = 0;

        /* ARRAY MAY NOT BE THE PROPER SIZE... ALLOCATE SPACE FOR POINTERS */

        /* Creating the sequence to keep track of free segments */
        m_new->free = Seq_new(HINT);
        assert(m_new->free != NULL);

        /* Populates free segment sequence */
        for (uint32_t col = 0; col < HINT; col++) {
        //         // m_new->segments[col].words = NULL;
                m_new->segments->numWords = 0;
        }



        m_new->segments[0].words = (uint32_t *) malloc(length * sizeof(uint32_t));
        for (uint32_t row = 0; row < length; row++) {
                m_new->segments[0].words[row] = 0;
                m_new->segments->numWords++;
        }

        m_new->colNum = HINT;

        return m_new;
}

/* Name: memory_free
 * Input: A pointer to a Memory_T struct 
 * Output: N/A 
 * Does: Frees all memory associated with the struct
 * Error: Asserts if struct is NULL
 */
void memory_free(Memory_T *m)
{
        if (m == NULL) {
                return;
        }

        // Free the memory used by each segment's words array
        for (uint32_t i = 0; i < m->colNum; ++i) {
                if (m->segments[i].words != NULL) {
                free(m->segments[i].words);
                }
        }

        // Free the memory used by the segments array
        if (m->segments != NULL) {
                free(m->segments);
        }

        // Free the memory used by the free array
        if (m->free != NULL) {
                Seq_free(&m->free);
        }

        // Free the memory used by the Memory_T struct itself
        free(m);
}

/* Name: memory_put
 * Input: A Memory_T struct, a segment number, an offset, and a value
 * Output: N/A
 * Does: Inserts value at the specificed segment and offset
 * Error: Asserts if struct is NULL
 *        Asserts if segment is not mapped
 *        Asserts if offset is not mapped
 */
void memory_put(Memory_T *m, uint32_t seg, uint32_t off, uint32_t val)
{
        /* Assert that the Memory_T struct is not NULL */
        assert(m != NULL);

        /* Assert that the segment is mapped */
        assert(seg < m->colNum);

        /* Assert that the offset is mapped */
        // fprintf(stderr, "seg: %d\n", seg);
        assert(off < m->segments[seg].numWords);

        /* Insert the value at the specified segment and offset */
        m->segments[seg].words[off] = val;
}

/* Name: memory_get
 * Input: A Memory_T struct, a segment number, and an offset
 * Output: A uint32_t which represents the value at that segment and offset
 * Does: Gets the value at the specified segment number and offset and returns
 * Error: Asserts if struct is NULL
 *        Asserts if segment is not mapped
 *        Asserts if offset is not mapped
 */
uint32_t memory_get(Memory_T *m, uint32_t seg, uint32_t off) {
    /* Assert that the Memory_T pointer is not NULL */
    assert(m != NULL);

    /* Assert that the segment is mapped */
    assert(seg < m->colNum && m->segments[seg].words != NULL);

    /* Assert that the offset is mapped */
    // fprintf(stderr, "off: %d numWords: %d\n", off, m->segments[seg].numWords);
    assert(off < m->segments[seg].numWords);

    /* Get the value at the specified segment and offset and return it */
    return m->segments[seg].words[off];
}



/* Name: memory_map
 * Input: A Memory_T struct, a segment number, and segment length
 * Output: the index of the mapped segment
 * Does: Creates a segment that is "length" long 
 *       with all of the segment's values being zero and 
 *       returns index of the mapped segment
 * Error: Asserts if struct is NULL
 *        Asserts if memory for segment is not allocated
 */
uint32_t memory_map(Memory_T *m, uint32_t length)
{
        uint32_t *newSeg = (uint32_t *)calloc(length, sizeof(uint32_t));
        uint32_t index;
        m->fullIndex++;

        /* If there are no unmapped segments */
        if (m->fullIndex == m->colNum) {
                expand_program(m);
        } 


        /* Get the first unmapped segment and map it */
        if ((uint32_t)(uintptr_t)Seq_length(m->free) != 0) {
                index = (uint32_t)(uintptr_t)Seq_remlo(m->free);
        } else {
                index = m->fullIndex;
        }
        // fprintf(stderr, "index: %d\n", index);
        // if (m->segments[index].numWords != 0) {
        //         memory_unmap(m, index);
        // }

        Row_T newRow;
        newRow.words = newSeg;
        newRow.numWords = length;
        m->segments[index] = newRow;
        return index;
}



void expand_program(Memory_T *m)
{
        // Create a new Memory_T structure with double the number of Row_T's
        uint32_t new_colNum = m->colNum * 2 + 1;

        m->segments = (Row_T *)realloc(m->segments, new_colNum * sizeof(Row_T));
        m->colNum = new_colNum;
}


/* Name: memory_unmap
 * Input: A Memory_T struct and a segment number
 * Output: N/A
 * Does: Unmaps a specified segment at the "seg_num" index and frees memory
 *       of the associated segment as well as adds that index back into the
 *       free segment sequence
 * Error: Asserts if struct is NULL
 *        Asserts if unmap segment 0
 *        Asserts if segment is NULL
 */
void memory_unmap(Memory_T *m, uint32_t seg_num)
{
        assert(m->segments != NULL);
        assert(seg_num != 0);

        Row_T *unmap = &m->segments[seg_num];
        assert(unmap != NULL);

        // Free the words array inside the Row_T struct
        free(unmap->words);

        // Set the numWords field to 0 to indicate that the segment is unmapped
        unmap->numWords = 0;
        m->fullIndex--;
        Seq_addhi(m->free, (void*)(uintptr_t)seg_num);
}
