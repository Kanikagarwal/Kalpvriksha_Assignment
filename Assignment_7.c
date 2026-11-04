#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_POOL_SIZE 1000000

// Structure with alias Block that is the main memory manager data type
typedef struct
{
    int start;
    int size;
    int allocationID;
    int isFree;
} Block;

// Global variables 
char *memoryPool = NULL; 
Block *blocks = NULL;

int poolSize = 0;
int blockCount = 0;
int capacity = 0; 
int nextAllocationID = 1;
int strategy = 1;

// The function for firstFit like the memory is allocated to first block which has size more than requested size
int firstFit(int reqSize)
{
    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isFree && blocks[i].size >= reqSize)
        {
            return i;
        }
    }
    return -1;
}
// The function for bestFit which finds the smallest block that has size > requested size 
int bestFit(int reqSize)
{
    int bestIndex = -1;

    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isFree && blocks[i].size >= reqSize)
        {

            if (bestIndex == -1 || blocks[i].size < blocks[bestIndex].size)
            {
                bestIndex = i;
            }
        }
    }
    return bestIndex;
}

// The function for worstFit which finds the largest block that has size > requested size 
int worstFit(int reqSize)
{
    int worstIndex = -1;

    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isFree &&
            blocks[i].size >= reqSize)
        {
            if (worstIndex == -1 || blocks[i].size > blocks[worstIndex].size)
            {
                worstIndex = i;
            }
        }
    }
    return worstIndex;
}

// Reading the input that takes up a prompt and the variable address in which value is needed to be stored
int readInput(char *prompt, int *value)
{
    char input[100];
    char *end;
    long num;

    while (1)
    {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL)
            return 0;
        num = strtol(input, &end, 10);
        if (num < INT_MIN || num > INT_MAX)
        {
            printf("Invalid input\n");
            continue;
        }
        *value = (int)num;
        return 1;
    }
}

// Initializing the pool with the asked size
int initializePool(int size)
{
    if (size <= 0 || size > MAX_POOL_SIZE)
    {
        printf("Invalid pool size\n");
        return 0;
    }

    memoryPool = malloc(size);
    if (memoryPool == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }
    capacity = 1;
    blocks = malloc(sizeof(Block) * capacity);

    if (blocks == NULL)
    {
        printf("Memory allocation failed\n");
        free(memoryPool);
        memoryPool = NULL;
        return 0;
    }

    // Initial values when entire memory is free
    poolSize = size;
    blockCount = 1;
    nextAllocationID = 1;
    blocks[0].start = 0;
    blocks[0].size = size;
    blocks[0].allocationID = -1;
    blocks[0].isFree = 1;

    printf("Memory pool initialized successfully.\n");
    return 1;
}

// Function for finding the block in which allocation can be done
int findSuitableBlock(int reqSize)
{
    switch (strategy)
    {

    case 1:
        return firstFit(reqSize);
    case 2:
        return bestFit(reqSize);
    case 3:
        return worstFit(reqSize);
    default:
        return -1;
    }
}

// A helper function that tells about the allocation metadata and updates it while resizing 
int growMetaData()
{
    int newCapacity = capacity * 2;
    Block *temp = realloc(blocks, sizeof(Block) * newCapacity);
    if (temp == NULL)
    {
        printf("Allocation failed");
        return 0;
    }
    blocks = temp;
    capacity = newCapacity;
    return 1;
}

// Function for detecting fragmentation(the memory is divided into fragments and are non-accessible contiguously)
void detectFragmentation(int reqSize)
{
    int totalFree = 0;
    int largestFree = 0;
    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isFree)
        {
            totalFree += blocks[i].size;
            if (blocks[i].size > largestFree)
            {
                largestFree = blocks[i].size;
            }
        }
    }
    if (totalFree >= reqSize && largestFree < reqSize)
    {
        printf("\nExternal fragmentation detected!\n");
        printf("Requested size: %d bytes\n", reqSize);
        printf("Total free memory: %d bytes\n", totalFree);
        printf("Largest free block: %d bytes\n", largestFree);
    }
    else if (totalFree < reqSize)
    {
        printf("Insufficient total free memory.\n");
    }
}

// Inserting the block at the prescribed index
void insertBlock(int index, Block newBlock)
{
    for (int i = blockCount; i > index; i--)
    {
        blocks[i] = blocks[i - 1];
    }

    blocks[index] = newBlock;
    blockCount++;
}

// Function for allocating memory
int allocateMemory(int reqSize)
{
    if (reqSize <= 0 || reqSize > poolSize)
    {
        printf("Invalid allocation size.\n");
        return 0;
    }
    int idx = findSuitableBlock(reqSize);
    if (idx == -1)
    {
        printf("Allocation failed: No suitable free block.\n");
        detectFragmentation(reqSize);
        return 0;
    }

    int oldSize = blocks[idx].size;
    int id = nextAllocationID++;

    // Marking blocks as allocated
    blocks[idx].size = reqSize;
    blocks[idx].allocationID = id;
    blocks[idx].isFree = 0;

    if (oldSize > reqSize)
    {
        if (blockCount == capacity)
        {
            if (!growMetaData())
            {
                blocks[idx].size = oldSize;
                blocks[idx].allocationID = -1;
                blocks[idx].isFree = 1;
                nextAllocationID--;
                printf("Metadata allocation failed.\n");
                return 0;
            }
        }
        Block newBlock;
        newBlock.start = blocks[idx].start + reqSize;
        newBlock.size = oldSize - reqSize;
        newBlock.allocationID = -1;
        newBlock.isFree = 1;

        insertBlock(idx + 1, newBlock);
    }
    printf("Allocation successful!\n");
    printf("Allocation ID: %d\n", id);
    printf("Start: %d\n", blocks[idx].start);
    printf("Size: %d\n", reqSize);
    return id;
}

// Removing the block from the memory manager blocks and reducing the blockCount 
void removeBlock(int idx)
{
    memmove(&blocks[idx], &blocks[idx + 1], (blockCount - idx - 1) * sizeof(Block));
    blockCount--;
}

// Merge the free blocks into a single block
void mergeFreeBlocks()
{
    int i = 0;
    while (i < blockCount - 1)
    {
        if (blocks[i].isFree && blocks[i + 1].isFree)
        {
            blocks[i].size += blocks[i + 1].size;
            removeBlock(i + 1);
        }
        else
        {
            i++;
        }
    }
}

// Releasing the allocated memory
void releaseMemory(int allocationID)
{
    int idx = -1;
    for (int i = 0; i < blockCount; i++)
    {
        if (!blocks[i].isFree && blocks[i].allocationID == allocationID)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1)
    {
        printf("Invalid allocation ID]\n");
        return;
    }

    blocks[idx].isFree = 1;
    blocks[idx].allocationID = -1;

    mergeFreeBlocks();
}

// Resizing the memory when required and merging the memory with the already existing
void resizeMemory(int allocationID, int newSize)
{
    int idx = -1;
    for (int i = 0; i < blockCount; i++)
    {
        if (!blocks[i].isFree && blocks[i].allocationID == allocationID)
        {
            idx = i;
            break;
        }
    }
    if (idx == -1 || newSize <= 0)
    {
        printf("Invalid allocation ID or size.\n");
        return;
    }
    int oldSize = blocks[idx].size;
    if (newSize == oldSize)
    {
        printf("Size is already the same.\n");
        return;
    }
    if (newSize < oldSize)
    {
        Block newBlock;
        newBlock.start = blocks[idx].start + newSize;
        newBlock.size = oldSize - newSize;
        newBlock.allocationID = -1;
        newBlock.isFree = 1;
        blocks[idx].size = newSize;
        if (blockCount == capacity)
        {
            if (!growMetaData())
            {
                printf("Unable to resize memory.\n");
                return;
            }
        }
        insertBlock(idx + 1, newBlock);
        mergeFreeBlocks();
        printf("Memory resized successfully\n");
        return;
    }
    int extraSize = newSize - oldSize;
    if (idx + 1 < blockCount && blocks[idx + 1].isFree && blocks[idx + 1].size >= extraSize)
    {
        blocks[idx].size = newSize;
        blocks[idx + 1].start += extraSize;
        blocks[idx + 1].size -= extraSize;
        if (blocks[idx + 1].size == 0)
        {
            removeBlock(idx + 1);
        }
        printf("Memory resized successfully\n");
        return;
    }
}

// Displaying the allocated memory layout
void displayMemoryMap(void)
{

    printf("\n========== MEMORY MAP ==========\n");
    printf("Memory Pool: %d bytes\n\n", poolSize);

    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isFree)
        {
            printf("[Free: %d bytes, Start: %d]\n", blocks[i].size, blocks[i].start);
        }
        else
        {
            printf("[Allocated #%d: %d bytes, Start: %d]\n", blocks[i].allocationID, blocks[i].size, blocks[i].start);
        }
    }
}

// Displaying the stats of the memory manager in required format
void displayStatistics()
{
    int allocatedMemory = 0;
    int freeMemory = 0;
    int allocatedBlocks = 0;
    int freeBlocks = 0;

    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].isFree)
        {
            freeMemory += blocks[i].size;
            freeBlocks++;
        }
        else
        {
            allocatedMemory += blocks[i].size;
            allocatedBlocks++;
        }
    }

    printf("\n========== MEMORY STATISTICS ==========\n");
    printf("Total Memory : %d bytes\n", poolSize);
    printf("Allocated Memory : %d bytes\n", allocatedMemory);
    printf("Free Memory : %d bytes\n", freeMemory);
    printf("Allocated Blocks : %d\n", allocatedBlocks);
    printf("Free Blocks : %d\n", freeBlocks);
}

// Function for changing the strategy -> bestFit, worstFit and firstFit
void changeStrategy(void)
{
    int choice;

    printf("\nSelect Allocation Strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    printf("Enter your choice: ");
    if (!readInput("Enter your choice: ", &choice))
        return;

    if (choice >= 1 && choice <= 3)
    {
        strategy = choice;
        printf("Allocation strategy changed successfully.\n");
    }
    else
    {
        printf("Invalid strategy choice.\n");
    }
}

// Function for cleaning up the space by calling free and deallocating the memory
void cleanup()
{
    free(memoryPool);
    free(blocks);

    memoryPool = NULL;
    blocks = NULL;

    poolSize = 0;
    blockCount = 0;
    capacity = 0;

    printf("Memory cleaned up successfully.\n");
}


int main()
{
    int choice;
    int size;
    int id;

    printf("====================================\n");
    printf("       SMART MEMORY MANAGER\n");
    printf("====================================\n");

    if (!readInput("Enter memory pool size: ", &size))
        return 0;

    if (!initializePool(size))
        return 1;

    printf("\nSelect allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    while (1)
    {
        if (!readInput("Enter strategy: ", &strategy))
        {
            return 0;
        }
        if (strategy >= 1 && strategy <= 3)
            break;
        printf("Invalid strategy, Choose 1,2 or 3\n");
    }
    printf("\nStrategy selected successfully.\n");

    // A menu-driven code
    while (1)
    {
        printf("\n========== MENU ==========\n");
        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        if (!readInput("Choice: ", &choice))
        {
            break;
        }

        switch (choice)
        {
        case 1:
            if(!readInput("Requested size: ", &size))
                return 1;
            allocateMemory(size);
            break;
        case 2:
            if (!readInput("Allocation ID: ", &id))
                return 1;
            releaseMemory(id);
            break;

        case 3:
            if (!readInput("Allocation ID: ", &id))
                return 1;
            if (!readInput("New size: ", &size))
                return 1;
            resizeMemory(id, size);
            break;

        case 4:
            displayMemoryMap();
            break;

        case 5:
            displayStatistics();
            break;

        case 6:
            changeStrategy();
            break;

        case 7:
            cleanup();
            printf("Exiting the program\n");
            return 0;

        default:
            printf("Invalid menu choice.\n");
        }
    }
    return 0;
}