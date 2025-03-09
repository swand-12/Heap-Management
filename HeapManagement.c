#include <stdio.h>
#include <stdlib.h>

typedef struct metadata
{
    struct metadata *next;
    int size;
} metadata;

char memory[10000]; // taking a memory of 10000 bytes from the stack
metadata *freeList = (metadata *)memory;
// memory points to the 1st location of the array
// free list also points to the 1st location of the array considering it as a metadata

void merge(metadata *prev)
{
    if (prev != NULL && prev->next != NULL)
    {
        metadata *ptr = prev->next;
        // merging with the next node
        if (ptr->next != NULL && (void *)ptr + ptr->size + sizeof(metadata) == ptr->next)
        {
            ptr->size = ptr->size + (ptr->next)->size + sizeof(metadata);
            ptr->next = ptr->next->next;
            ptr->next = NULL;
        }
        // merging with the previous node
        if ((void *)prev + prev->size + sizeof(metadata) == ptr)
        {
            prev->next = ptr->next;
            prev->size = prev->size + ptr->size + sizeof(metadata);
            ptr = NULL;
        }
    }
}

void deallocate(void *ptr)
{
    if (ptr == NULL)
    {
        printf("\nInvalid block");
    }
    else
    {
        metadata *temp = freeList, *prev;
        // find where the free block should be inserted
        while (ptr > (void *)temp)
        {
            prev = temp;
            temp = temp->next;
        }
        if (freeList == temp) // insertion in freeList before first node
        {
            ((metadata *)ptr)->next = freeList;
            freeList = (metadata *)ptr;
        }
        else
        {
            ((metadata *)ptr)->next = temp;
            prev->next = (metadata *)ptr;
        }
        temp = freeList;
        printf("\n\nBefore merging: ");
        while (temp != NULL)
        {
            printf("\naddress: %p, size: %d", temp, temp->size);
            temp = temp->next;
        }
        merge(prev);
        printf("\n\nAfter Merging: ");
        temp = freeList;
        while (temp != NULL)
        {
            printf("\naddress: %p, size: %d", temp, temp->size);
            temp = temp->next;
        }
        printf ("\nDeallocated");
    }
}

void *allocate(int blockSize)
{
    void *res;
    metadata *ptr;
    if (freeList->next == NULL && (int)(blockSize + sizeof(metadata)) < freeList->size) // single free block
    {
        ptr = freeList;
        //moving by some bytes
        freeList = (metadata *)((void *)freeList + blockSize + sizeof(metadata));
        // restoring values of freeList
        freeList->size = ptr->size - sizeof(metadata) - blockSize;
        freeList->next = NULL;
        // while comparing, compare blockSize + sizeof(metadata), but while storing in the meta data, ignore size of metadata for better user readability
        ptr->size = blockSize;
        ptr->next = NULL;
        printf("\n\nAllocated! ");
    }
    else
    {
        // allocating in freed chunks, or last remaining large chunk
        metadata *temp = freeList, *prev = freeList;
        // find an appropriate sized chunk
        while (temp != NULL && temp->size < (int)(blockSize + sizeof(metadata)))
        {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL) // array is finished, cause can be external fragmentation
        {
            printf("No memory");
            res = NULL;
        }
        else // a chunk is found(can be first or last or in between)
        {
            ptr = (void *)temp; // points to the starting address of thet chunk
            if (prev == temp)   // changing free pointer if memory is allocated at the start
            {
                freeList = (metadata *)((void *)temp + blockSize + sizeof(metadata));
            }
            temp = (metadata *)((void *)temp + blockSize + sizeof(metadata)); // shifting the metabadata of the free chunk, making space for the memory to be allocated
            // restoring size and next pointer of temp(the free chunk which was moved)
            temp->size = ((metadata *)ptr)->size - blockSize - sizeof(metadata); // absolute size of the meomory chunk, metadata included
            temp->next = ((metadata *)ptr)->next;
            // setting metadata for the newly allocated memry from the chunk
            ((metadata *)ptr)->size = blockSize;
            ((metadata *)ptr)->next = NULL;
            if (prev != temp) // if its not the starting part
            {
                prev->next = temp;
            }
            printf("\n\nAllocated");
        }
    }
    res = (void *)ptr;
    return res;
}

int main()
{
    freeList->next = NULL;  // Set next to NULL
    freeList->size = 10000; // Set size to the total size of the memory array

    printf("\nInitial freeList values: start=%p, size=%d", (void *)freeList, freeList->size);

    void *res = allocate(2);
    printf("\nsize: %d, next: %p", ((metadata *)res)->size, ((metadata *)res)->next);
    printf("\nptr: %p, freeList: %p", res, freeList);

    void *res1 = allocate(15);
    printf("\nsize: %d, next: %p", ((metadata *)res1)->size, ((metadata *)res1)->next);
    printf("\nptr: %p, freeList: %p", res1, freeList);

    void *res3 = allocate(30);
    printf("\nsize: %d, next: %p", ((metadata *)res3)->size, ((metadata *)res3)->next);
    printf("\nptr: %p, freeList: %p", res3, freeList);

    void *res5 = allocate(20);
    printf("\nsize: %d, next: %p", ((metadata *)res5)->size, ((metadata *)res5)->next);
    printf("\nptr: %p, freeList: %p", res5, freeList);

    void *res6 = allocate(54);
    printf("\nsize: %d, next: %p", ((metadata *)res6)->size, ((metadata *)res6)->next);
    printf("\nptr: %p, freeList: %p", res6, freeList);

    printf("\n\nSize of freeList: %d", freeList->size);

    printf("\n\nDeallocating");

    deallocate(res);
    deallocate(res1);
    deallocate(res5);

    printf("\n\nAllocating in free chunks");

    void *res2 = allocate(20);
    printf("\nsize: %d, next: %p", ((metadata *)res2)->size, ((metadata *)res2)->next);
    printf("\nptr: %p, freeList: %p\n", res2, freeList);

    metadata *temp = freeList;
    while (temp != NULL)
    {
        printf("\naddress: %p, size: %d", temp, temp->size);
        temp = temp->next;
    }
    printf("\n\n");

    return 0;
}
