#include "mymalloc.h"

// TODO:
// get rid of static initalizer flag | DONE
// merge continuous free'd blocks | DONE
// nullptr return if not allocated | DONE
// allocating to free'd blocks | DONE
// fix freeing unallocated blocks issue | DONE


// global memory array to simulate heap
char MyMemory[MEMORY_SIZE];


void* MyMalloc(size_t size) {
    // static char initalized = 'F';
    // if (initalized == 'F') {
    //     initalized = 'T';
    //     for (int i = 0; i < MEMORY_SIZE; i++)
    //         MyMemory[i] = 0; // set values to zero
    // }

    if (MyMemory[MEMORY_SIZE - 1] != 'x') {
        for (int i = 0; i < MEMORY_SIZE; i++)
            MyMemory[i] = 0; // set values to zero
        MyMemory[MEMORY_SIZE - 1] = 'x';
    }

    if (size <= 0) return NULL;

    short *currBlock = (short *) MyMemory;

    while (*currBlock != 0) {
        // traverse MyMemory using metadata to jump mimicing a linked list
        // traverse until an appropriate space is found and loop breaks out
        //    or until end of pseudo linked list is found
        
        if ((char *) currBlock == &MyMemory[MEMORY_SIZE - 1] || (char *) currBlock == &MyMemory[MEMORY_SIZE - 2]) {
            // ensure final index's initialization flag isnt treated as metadata
            // check index before as well since short deference of that index will take in flag as well
            return NULL;
        }

        if (*currBlock < 0) {
            // free'd block; negative size

            if (-*currBlock == size) {
                // same size space
                // overwrite free'd block
                *currBlock = 0;
                break;
            }
            else if (-*currBlock > size) {
                // bigger space
                // deal with excess space and break out of loop

                short excess = (-*currBlock) - size;
                if (excess > 2) {
                    // create seperate "free'd" block for remaining space 
                    short *excessBlock = (short *) ((char *) currBlock + size + 2);
                    *excessBlock = -(excess - 2);
                }
                else {
                    // cannot fit another block, extend size of allocated block
                    // since size zero allocated block is useless
                    size += 2;
                }

                *currBlock = 0;
                break;
            }

            // not enough space
            currBlock = (short *) ((char *) currBlock + (-*currBlock) + 2);
            continue;
        }

        // allocated block
        currBlock = (short *) ((char *) currBlock + *currBlock + 2);
        // logic explaination:
        //      currBlock is interpreted as a short (size of immediate block) and is added to
        //      currBlock pointer type casted to a char pointer, thereby jumping by the size of the immediate block
        //      2 is added to account for the metadata segment
        //      new pointer is type casted to a short pointer and reassigned
        //      essentially functioning as a pseudo linked list
    }

    if (*currBlock == 0) {
        if ((((char *) currBlock - MyMemory) / sizeof(char)) + size + 2 > MEMORY_SIZE - 1) {
            // above expression: finding index, adding size, check for overflow

            // not enough space in MyMemory
            return NULL;
        }

        *currBlock = (short) size;
        return currBlock + 1; // return allocated address w/o metadata
    }
    else return NULL; // unable to allocate
}


void MyFree(void* ptr) {
    if (ptr == NULL) return;

    short *currBlock = (short *) ptr;
    currBlock -= 1; // access metadata of block

    if (*currBlock > MEMORY_SIZE - 1 || *currBlock < 0) {
        printf("\e[0;31mERROR: Invalid Pointer passed into MyFree\e[0m\n\a");
        return; // segmentation fault catching
    }

    // check if next block is free
    short *nextBlock = (short *) ((char *) currBlock + *currBlock + 2);
    if (*nextBlock < 0)
        *currBlock += (-*nextBlock) + 2;

    // check if prev block is free
    short *prevBlock = (short *) MyMemory;
    while (*prevBlock != 0) {
        if ((char *) currBlock == &MyMemory[MEMORY_SIZE - 1] || (char *) currBlock == &MyMemory[MEMORY_SIZE - 2])
            break;

        if (*prevBlock < 0) {
            // free'd block

            nextBlock = (short *) ((char *) prevBlock + (-*prevBlock) + 2);
            if (nextBlock == currBlock) { // block before current
                *prevBlock = (-*prevBlock) + *currBlock + 2;
                currBlock = prevBlock;
                break;
            }

            prevBlock = nextBlock;
        }
        else {
            // alloc'd block
            prevBlock = (short *) ((char *) prevBlock + *prevBlock + 2);
        }
    }

    if (*currBlock > 0) // negate size metadata
        *currBlock *= -1;
}


// ===================================================================================


// logic summary
// intepret MyMemory as a singly-linked-list
// only metadata stored is the "size" attribute of the immediate block
// when traversing, jump forward by the size of the block and account for metadata
// free'd blocks are indicated by a negative size attribute,
//      and calculations are adjusted where necessary
// since original size is necessary to traverse linked list,
//      free'd blocks' size attribute must be retained
// final index of MyMemory stores a single char value to indicate if MyMemory has been initialized,
//      as any garbage values encountered when traversing linked list,
//      can mess up implementation of MyMalloc and MyFree


// ===================================================================================


void displayMemory() { // debugging purposes
    printf("\n========================================================================================\n");
    printf("\e[0;36mDisplay MyMemory at %p\e[0m", MyMemory);

    short *currBlock = (short *) MyMemory;

    while (*currBlock != 0) {
        if ((char *) currBlock == &MyMemory[MEMORY_SIZE - 1] || (char *) currBlock == &MyMemory[MEMORY_SIZE - 2])
            break;

        if (*currBlock < 0) {
            printf("\n\n\t\e[1;32m FREE'D BLOCK\e[0m of\e[0;35m size %d\e[0m at \e[0;34m0x%p\e[0m;", -*currBlock, currBlock);
            currBlock = (short *) ((char *) currBlock + (-*currBlock) + 2);
            continue;
        }

        printf("\n\n\t\e[1;31m ALLOC'D BLOCK\e[0m of\e[0;35m size %d\e[0m at \e[0;34m0x%p\e[0m;", *currBlock, currBlock);

        printf("\n\t\t\e[0;33mchar Rep;\e[0m \t");
        for (int i = 0; i < *currBlock; i++)
            printf("%c ", *((char *) currBlock + 2 + i));
        printf("\n\t\t\e[0;33mint Rep;\e[0m \t");
        for (int i = 0; i < *currBlock; i++)
            printf("%d ", *(int *) ((char *) currBlock + 2 + i));
        printf("\n\t\t\e[0;33mhex Rep;\e[0m \t");
        for (int i = 0; i < *currBlock; i++)
            printf("%x ", *((char *) currBlock + 2 + i));

        currBlock = (short *) ((char *) currBlock + *currBlock + 2);
    }
    printf("\n========================================================================================\n\n");
}