#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define MAX_INPUT 8192
#define MAX_PATH_DEPTH 256

// struct for freeBlock
typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

// struct for fileNode
typedef struct FileNode
{
    char name[MAX_NAME + 1];
    int isDirectory;

    struct FileNode *prev;
    struct FileNode *next;
    struct FileNode *parent;
    struct FileNode *children;

    int fileSize;
    int blockCount;
    int *blockPointers;
} FileNode;

// Global variables
FreeBlock *freeHead = NULL;    // Head of freeBlock
FreeBlock *freeTail = NULL;    // Tail of freeBlock
FreeBlock **blockNodes = NULL; // Array of pointers

FileNode *root = NULL; // which is the root directory
FileNode *cwd = NULL;  // which is the current working directory

int totalBlocks = NUM_BLOCKS; // total number of blocks(from macros)
int usedBlocks = 0;           // total used blocks

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

// Function to append a free block at the end of linked list (similar to add node at the end of doubly linked list)
void appendFreeBlock(int index)
{
    FreeBlock *node = blockNodes[index];
    node->index = index;
    node->next = NULL;
    node->prev = freeTail;

    if (freeTail == NULL)
    {
        freeHead = node;
    }
    else
    {
        freeTail->next = node;
    }
    freeTail = node;
}

// Function to find if a particular fileNode is present or not
FileNode *findChild(FileNode *parent, const char *name)
{
    FileNode *current = parent->children;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

// Function to free the block that are allocated
void freeBlock(int index)
{
    appendFreeBlock(index);
    usedBlocks--;
}

// A simple cleanup function to deallocate the memory
void cleanUP(FileNode *directory)
{
    if (directory == NULL)
        return;

    FileNode *current = directory->children;

    while (current != NULL)
    {
        FileNode *next = current->next;
        if (current->isDirectory)
        {
            cleanUP(current);
        }
        free(current->blockPointers);
        free(current);

        current = next;
    }
    if (directory == root)
    {
        free(directory);
        root = NULL;
        cwd = NULL;
        for (int i = 0; i < NUM_BLOCKS; i++)
        {
            free(blockNodes[i]);
        }

        free(blockNodes);
        blockNodes = NULL;

        freeHead = NULL;
        freeTail = NULL;
        usedBlocks = 0;
    }
}

// Function to create a node either it is directory or file
FileNode *createNode(char *name, int isDirectory, FileNode *parent)
{
    FileNode *node = (FileNode *)malloc(sizeof(FileNode));
    if (node == NULL)
        return NULL;
    strncpy(node->name, name, MAX_NAME);
    node->name[MAX_NAME] = '\0';

    node->isDirectory = isDirectory;

    node->parent = parent;
    node->children = NULL;
    node->next = NULL;
    node->prev = NULL;

    node->fileSize = 0;
    node->blockCount = 0;
    node->blockPointers = NULL;

    return node;
}

// Initializing the disk memory
int intializeDisk()
{
    freeHead = freeTail = NULL;
    usedBlocks = 0;
    blockNodes = calloc(NUM_BLOCKS, sizeof(FreeBlock *));

    if (blockNodes == NULL)
        return 0;

    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        blockNodes[i] = malloc(sizeof(FreeBlock));
        if (blockNodes[i] == NULL)
            return 0;
        appendFreeBlock(i);
    }
    return 1;
}

// Initializing the VFS
int initializeVFS()
{
    if (!intializeDisk())
        return 0;
    root = createNode("/", 1, NULL);
    if (root == NULL)
        return 0;
    cwd = root;
    printf("VFS is ready\n");
    printf("Type exit to quit\n");
    return 1;
}

// Printing current working directory
void printWorkingDirectory(FileNode *node)
{
    if (node == NULL)
    {
        return;
    }
    printWorkingDirectory(node->parent);
    if (node->parent != NULL)
    {
        printf("/");
    }
    printf("%s", node->name);
}

void makeDirectory(char *argument)
{
    if (cwd == NULL)
    {
        printf("No current working directory.\n");
        return;
    }
    if (argument == NULL || argument[0] == '\0')
    {
        printf("Invalid directory name.\n");
        return;
    }

    FileNode *existing = findChild(cwd, argument);
    if (existing != NULL)
    {
        printf("A file or directory with this name already exists.\n");
        return;
    }

    FileNode *newDir = createNode(argument, 1, cwd);
    if (newDir == NULL)
    {
        printf("Directory creation failed.\n");
        return;
    }
    newDir->next = cwd->children;
    newDir->prev = NULL;
    if (cwd->children != NULL)
    {
        cwd->children->prev = newDir;
    }
    cwd->children = newDir;
    printf("Directory created successfully.\n");
}

void createFile(char *argument)
{
    if (cwd == NULL)
    {
        printf("No current working directory.\n");
        return;
    }

    if (argument == NULL || argument[0] == '\0')
    {
        printf("Invalid file name.\n");
        return;
    }

    if (strlen(argument) > MAX_NAME)
    {
        printf("Filename is too long.\n");
        return;
    }

    if (findChild(cwd, argument) != NULL)
    {
        printf("A file or directory with this name already exists.\n");
        return;
    }

    FileNode *newFile = createNode(argument, 0, cwd);

    if (newFile == NULL)
    {
        printf("File creation failed.\n");
        return;
    }

    newFile->next = cwd->children;
    newFile->prev = NULL;

    if (cwd->children != NULL)
    {
        cwd->children->prev = newFile;
    }

    cwd->children = newFile;
    printf("File '%s' created successfully.\n", argument);
}

void changeDirectory(char *argument)
{
    if (cwd == NULL)
    {
        printf("No current working directory.\n");
        return;
    }

    if (argument == NULL || argument[0] == '\0')
    {
        printf("Invalid file name.\n");
        return;
    }
    if (strcmp(argument, "/") == 0)
    {
        cwd = root;
        return;
    }
    if (strcmp(argument, "..") == 0)
    {

        if (cwd->parent != NULL)
        {
            cwd = cwd->parent;
        }

        return;
    }
    FileNode *target = findChild(cwd, argument);

    if (target == NULL)
    {
        printf("Directory not found.\n");
        return;
    }
    if (target->isDirectory == 0)
    {
        printf("'%s' is a file, not a directory.\n", argument);
        return;
    }

    cwd = target;

    printf("Changed directory to '%s'.\n", cwd->name);
}

// Function to list the directories and files created by user
void listDirectory()
{
    if (cwd == NULL)
    {
        printf("No current working directory.\n");
        return;
    }
    if (!cwd->isDirectory)
    {
        printf("Current location is not a directory.\n");
        return;
    }

    FileNode *current = cwd->children;
    if (current == NULL)
    {
        printf("Directory is empty.\n");
        return;
    }
    while (current != NULL)
    {

        if (current->isDirectory)
        {
            printf("%s/\n", current->name);
        }
        else
        {
            printf("%s\n", current->name);
        }
        current = current->next;
    }
}

// Removing a directory if it is empty
void removeDirectory(char *name)
{
    if (cwd == NULL || !cwd->isDirectory)
    {
        printf("Invalid current directory.\n");
        return;
    }

    if (name == NULL || name[0] == '\0')
    {
        printf("Directory name is required.\n");
        return;
    }
    FileNode *target = findChild(cwd, name);

    if (target == NULL)
    {
        printf("Directory not found.\n");
        return;
    }
    if (!target->isDirectory)
    {
        printf("'%s' is a file, not a directory.\n", name);
        return;
    }
    if (target->children != NULL)
    {
        printf("Directory is not empty.\n");
        return;
    }
    if (target->prev != NULL)
    {
        target->prev->next = target->next;
    }
    else
    {
        cwd->children = target->next;
    }
    if (target->next != NULL)
    {
        target->next->prev = target->prev;
    }
    free(target);
    printf("Directory '%s' removed successfully.\n", name);
}

void deleteFile(char *name)
{

    if (cwd == NULL || !cwd->isDirectory)
    {
        printf("Invalid current directory.\n");
        return;
    }
    if (name == NULL || name[0] == '\0')
    {
        printf("Filename is required.\n");
        return;
    }
    FileNode *target = findChild(cwd, name);

    if (target == NULL)
    {
        printf("File not found.\n");
        return;
    }
    if (target->isDirectory)
    {
        printf("'%s' is a directory, not a file.\n", name);
        return;
    }
    for (int i = 0; i < target->blockCount; i++)
    {
        freeBlock(target->blockPointers[i]);
    }
    free(target->blockPointers);
    target->blockPointers = NULL;
    if (target->prev != NULL)
    {
        target->prev->next = target->next;
    }
    else
    {
        cwd->children = target->next;
    }

    // Disconnect from next node
    if (target->next != NULL)
    {
        target->next->prev = target->prev;
    }
    free(target);

    printf("File '%s' deleted successfully.\n", name);
}

// A function that helps to read the content of file
void readFile(char *name)
{
    if (cwd == NULL)
    {
        printf("No current directory.\n");
        return;
    }

    if (name == NULL || name[0] == '\0')
    {
        printf("Please provide a filename.\n");
        return;
    }

    // Find the file in the current directory
    FileNode *file = findChild(cwd, name);

    if (file == NULL)
    {
        printf("File not found.\n");
        return;
    }

    // Directories cannot be read as files
    if (file->isDirectory)
    {
        printf("%s is a directory, not a file.\n", name);
        return;
    }

    printf("Content of %s:\n", name);

    // Handle an empty file
    if (file->fileSize == 0)
    {
        printf("File is empty.\n");
        return;
    }

    // Read and print each byte from the virtual disk
    for (int i = 0; i < file->fileSize; i++)
    {

        int blockPosition = i / BLOCK_SIZE;
        int offset = i % BLOCK_SIZE;

        int diskBlock = file->blockPointers[blockPosition];

        putchar(virtualDisk[diskBlock][offset]);
    }

    printf("\n");
}

// Function for write on a file
void writeFile(const char *name, const char *content)
{

    //Find the file
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    //Calculate required blocks
    int length = strlen(content);

    int blocksNeeded = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;

    //Check available space
    if (blocksNeeded > totalBlocks - usedBlocks)
    {
        printf("Not enough disk space.\n");
        return;
    }

    //Allocate memory for block pointers
    file->blockPointers = malloc(blocksNeeded * sizeof(int));

    if (blocksNeeded > 0 && file->blockPointers == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    //Allocate blocks and copy content
    int copied = 0;

    for (int i = 0; i < blocksNeeded; i++)
    {

        FreeBlock *block = freeHead;

        // Remove block from free list
        freeHead = freeHead->next;

        if (freeHead != NULL)
            freeHead->prev = NULL;
        else
            freeTail = NULL;

        int index = block->index;

        // Save block index in file
        file->blockPointers[i] = index;

        // Calculate bytes to copy
        int bytes = length - copied;

        if (bytes > BLOCK_SIZE)
            bytes = BLOCK_SIZE;

        // Copy content into virtual disk
        memcpy(virtualDisk[index], content + copied, bytes);

        copied += bytes;

        usedBlocks++;
    }

    file->fileSize = length;
    file->blockCount = blocksNeeded;

    printf("File written successfully.\n");
}

// Function that shows the analytics of overall virtual disk
void diskUsage()
{
    int freeBlocks = totalBlocks - usedBlocks;

    int totalSpace = totalBlocks * BLOCK_SIZE;
    int usedSpace = usedBlocks * BLOCK_SIZE;
    int freeSpace = freeBlocks * BLOCK_SIZE;

    printf("\n----- Disk Usage -----\n");

    printf("Total Blocks : %d\n", totalBlocks);
    printf("Used Blocks  : %d\n", usedBlocks);
    printf("Free Blocks  : %d\n", freeBlocks);

    printf("\nTotal Space  : %d bytes\n", totalSpace);
    printf("Used Space   : %d bytes\n", usedSpace);
    printf("Free Space   : %d bytes\n", freeSpace);

    printf("----------------------\n");
}
// Takes the input extract the command and argument and execute a command accordingly
void executeCommand(char *input)
{
    char command[32] = "";
    char argument[MAX_INPUT] = "";

    int pos = 0;
    while (input[pos] && !isspace((unsigned char)input[pos]) && pos < 31)
    {
        command[pos] = input[pos];
        pos++;
    }
    command[pos] = '\0';
    char *rest = input + pos; // rest is a substring of input that starts from index pos
    while (isspace((unsigned char)*rest))
        rest++;

    strcpy(argument, rest);

    // If-else are matching the command with a predefined value and executing the corresponding functions
    if (strcmp(command, "mkdir") == 0)
    {
        if (*argument == '\0')
        {
            printf("Usage: mkdir directory_name\n");
        }
        else
        {
            makeDirectory(argument);
        }
    }
    else if (strcmp(command, "create") == 0)
    {
        if (*argument == '\0')
            printf("Usage: create filename\n");
        else
            createFile(argument);
    }

    else if (strcmp(command, "cd") == 0)
    {
        if (*argument == '\0')
            printf("Usage: cd directory_name\n");
        else
            changeDirectory(argument);
    }

    else if (strcmp(command, "ls") == 0)
    {
        if (*argument != '\0')
            printf("This implementation supports: ls\n");
        else
            listDirectory();
    }

    else if (strcmp(command, "rmdir") == 0)
    {
        if (*argument == '\0')
            printf("Usage: rmdir directory_name\n");
        else
            removeDirectory(argument);
    }

    else if (strcmp(command, "delete") == 0)
    {
        if (*argument == '\0')
            printf("Usage: delete filename\n");
        else
            deleteFile(argument);
    }

    else if (strcmp(command, "read") == 0)
    {
        if (*argument == '\0')
            printf("Usage: read filename\n");
        else
            readFile(argument);
    }

    else if (strcmp(command, "pwd") == 0)
    {
        printWorkingDirectory(cwd);
    }

    else if (strcmp(command, "df") == 0)
    {
        diskUsage();
    }

    else if (strcmp(command, "write") == 0)
    {
        char filename[MAX_NAME + 1] = "";
        int i = 0;

        while (argument[i] && !isspace((unsigned char)argument[i]))
        {
            if (i >= MAX_NAME)
            {
                printf("Filename is too long.\n");
                return;
            }

            filename[i] = argument[i];
            i++;
        }

        filename[i] = '\0';

        // Skip spaces after filename
        while (isspace((unsigned char)argument[i]))
        {
            i++;
        }

        char *content = argument + i;

        // Remove opening quote
        if (*content == '"')
            content++;

        // Remove closing quote
        size_t len = strlen(content);

        if (len > 0 && content[len - 1] == '"')
            content[len - 1] = '\0';

        if (filename[0] == '\0')
        {
            printf("Invalid filename.\n");
            return;
        }

        writeFile(filename, content);
    }

    else if (command[0] != '\0')
    {
        printf("Unknown command: %s\n", command);
    }
}

int main()
{
    if (!initializeVFS())
    {
        printf("VFS initialization failed!!\n");
        cleanUP(root);
        return 1;
    }

    char input[MAX_INPUT];

    while (1)
    {
        printWorkingDirectory(cwd);
        printf(" > ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        int len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
        }

        char command[32] = "";
        sscanf(input, "%31s", command);
        if (strcmp(command, "exit") == 0)
            break;
        executeCommand(input);
    }

    cleanUP(root);
    return 0;
}