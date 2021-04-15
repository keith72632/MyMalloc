#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

struct block {
    size_t size; //Carries size of block described by it
    int free; //Boolean for wether this block is occupied or not
    struct block *next; //Pointer to the next block
}; 

/*This will serve as a block of memory. 160kb of space*/
char memory[20000];

//Initialize a block with the starting address of memory
//Casted to type void so it can later be used for all types
struct block *freeList = (void*)memory;

//Prototypes
void init();
void split(struct block *fitting_slot, size_t size);
void *myMalloc(size_t numOfBytes);
void merge();
void myFree(void *ptr);

int main(int argc, char **argv)
{
    init();

    char *string = myMalloc(2000);
    string = "hello world\n";
    printf("%s\n", string);

    return 0;
}

void init()
{
    freeList->size = 20000-sizeof(struct block);  //20000 of certain data type(size_t). Char here
    freeList->free = 1;                           //1 == true 
    freeList->next = NULL;                        //First metablock has no next 
};

/*Making way for a new block allocation by splitting free block (First fit algorithm)*/
void split(struct block *fitting_slot, size_t size)
{
    //creates a new block with the appropriate size, assigns it, and marks it as occupied
    struct block *new = ((void*)fitting_slot+size+sizeof(struct block));
    new->size = (fitting_slot->size)-size-sizeof(struct block);
    new->free = 1;
    new->next = fitting_slot->next;
    fitting_slot->size = size;
    fitting_slot->free = 0;
    fitting_slot->next = new; 
}

void *myMalloc(size_t numOfBytes)
{
    //current block and previous block
    struct block *curr, *prev;
    //metadata pointer is to return the staring address of the allocated chunk of memory
    void *result;

    /*This condition will initialize the memory if it is not initialized in the beginning.
     * will be executed only if the size of the first metadata bolck is no tset*/
    if(!(freeList->size))
    {
        init();
        printf("Memory initialzied\n");
    }
    
    //temp pointer curr to point to the start of the metadata block list
    curr = freeList;

    while((((curr->size)<numOfBytes)||((curr->free)==0))&&(curr->next!=NULL))
    {
        prev=curr;
        curr=curr->next;
        printf("One block checked\n");
    }

    //If this condition is met, then return the starting addr of the block of alloced mem
    if((curr->size)==numOfBytes)
    {
        curr->free = 0;
        result = (void*)(++curr);
        printf("Exact fitting block allocated\n");
        return result;
    }

    //Block is greater than what is required. Returned starting addr
    else if((curr->size) > (numOfBytes + sizeof(struct block)))   
    {
        split(curr, numOfBytes);
        result = (void*)(++curr);
        printf("Fitting block split then allocated\n");
        return result;
    }

    else{
        result = NULL;
        printf("Sorry. No sufficient memory to allocate\n");
        return result;
    }
}

/*Situtations can arise when there is enough free space but it has previously been allocated. Metadata needs cleared*/
void merge()
{
    struct block *curr, *prev;

    curr = freeList;

    while((curr->next)!=NULL)
    {
        if((curr->free) && (curr->next->free))
        {
            curr->size+=(curr->next->size)+sizeof(struct block);
            curr->next = curr->next->next;
        }
        prev=curr;
        curr=curr->next;
    }
}

void myFree(void *ptr)
{
    if(((void*)memory<=ptr)&&(ptr<=(void*)(memory+20000)))
    {
        struct block *curr = ptr;
        --curr;
        curr->free = 1;
        merge();
    }
    else printf("Please provide a valid pointer allocated by myAlloc\n");
}










