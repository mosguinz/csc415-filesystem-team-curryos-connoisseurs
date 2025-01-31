/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
* Project:: Basic File System
*
* File:: b_io.c
*
* Description:: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>         // for malloc
#include <string.h>         // for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "b_io.h"
#include "fs_control.h"
#include "fsUtils.h"
#include "freespace.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

b_fcb fcbArray[MAXFCBS];

int startup = 0;    //Indicates that this has not been initialized

//Method to initialize our file system
void b_init ()
    {
    //init fcbArray to all free
    for (int i = 0; i < MAXFCBS; i++)
        {
        fcbArray[i].buf = NULL; //indicates a free fcbArray
        }

    startup = 1;
    }

//Method to get a free FCB element
b_io_fd b_getFCB ()
    {
    for (int i = 0; i < MAXFCBS; i++)
        {
        if (fcbArray[i].buf == NULL)
            {
            return i;       //Not thread safe (But do not worry about it for this assignment)
            }
        }
    return (-1);  //all in use
    }

// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags){
    int ret;                // Used for error handling
    int fileLocation;           // Used for storing physical location of file
    int emptyIndex;             // Track first unused entry in parent
    int parentSize;             // Compute size of parent directory for linking
    b_io_fd returnFd;           // File Descriptor
    struct PPRETDATA * ppinfo;   // Structure returned from parse path
    struct DE * parent;         // Track parent directory entry to write back to
    struct DE * fileInfo;           // Used in case we create a new file
    struct DE file;

    if (startup == 0) b_init();         //Initialize our system
    returnFd = b_getFCB();          // get our own file descriptor
    if ( returnFd == -2 ){
        fprintf(stderr, "error in getFCB\n");
        return returnFd;
    }

    // Prepare structures for parse path
    ppinfo = malloc(sizeof(struct PPRETDATA));
    ppinfo->parent = malloc(DE_SIZE);
    fileInfo = malloc(MINBLOCKSIZE);
    memset(fileInfo, 0, MINBLOCKSIZE);
    if ( !ppinfo || !fileInfo ) { // TODO separate malloc checks and free if needed
        perror("malloc");
        return -1;
    }

    // Parse through input string and retrieve parent structure and file name
    if ( parsePath(filename, ppinfo) == -1 ) {
        fprintf(stderr, "error in Parse Path\n");
        return -1;
    }
    parent = ppinfo->parent;

    b_fcb fcb = fcbArray[returnFd];
    fcb.fileInfo = malloc(sizeof(struct DE));
    fcb.buf = malloc(B_CHUNK_SIZE);

    // read an existing file
    if( flags & O_RDONLY && ppinfo->lastElementIndex > 0 ) {
        file = ppinfo->parent[ppinfo->lastElementIndex];
        strncpy(fcb.fileInfo->name, file.name, DE_NAME_SIZE);
        fcb.fileInfo->size = file.size;
        fcb.fileInfo->location = file.location;
        fcb.fileInfo->dateCreated = file.dateCreated;
        fcb.fileInfo->dateModified = file.dateModified;
        fcb.fileInfo->dateLastAccessed = file.dateLastAccessed;
        fcb.fileInfo->isDirectory = file.isDirectory;
        fcb.remainingBytes = file.size;
        fcb.index = 0;
        fcb.fileIndex = ppinfo->lastElementIndex;
        fcb.numBlocks = 0;
        fcb.fileIndex = ppinfo->lastElementIndex;
    }
    // write an already created file
    else if( ppinfo->lastElementIndex > 0) {
        file = ppinfo->parent[ppinfo->lastElementIndex];
        strncpy(fcb.fileInfo->name, file.name, DE_NAME_SIZE);
        fcb.fileInfo->size = file.size;
        fcb.fileInfo->location = file.location;
        fcb.fileInfo->dateCreated = file.dateCreated;
        fcb.fileInfo->dateModified = file.dateModified;
        fcb.fileInfo->dateLastAccessed = file.dateLastAccessed;
        fcb.fileInfo->isDirectory = file.isDirectory;
        fcb.remainingBytes = file.size;
        fcb.index = 0;
        fcb.currentBlock = file.location;
        fcb.fileIndex = ppinfo->lastElementIndex;
        fcb.numBlocks = NMOverM(file.size, MINBLOCKSIZE);
        fcb.fileIndex = ppinfo->lastElementIndex;
        // offset the file if it needs to be truncated
        if(flags & O_TRUNC) {
            fcb.currentBlock = fileSeek(fcb.currentBlock, fcb.numBlocks);
            fcb.index = file.size % B_CHUNK_SIZE;
            fileRead(fcb.buf, 1, fcb.currentBlock);
        }
    }
    // write a new file
    else if(flags & O_CREAT) {
        strncpy(file.name, ppinfo->lastElementName, DE_NAME_SIZE);
        file.size = 0;
        file.location = -1;
        time_t currTime = time(NULL);
        file.dateCreated = currTime;
        file.dateModified = currTime;
        file.dateLastAccessed = currTime;
        file.isDirectory = 0;
        fcb.remainingBytes = file.size;
        fcb.index = 0;
        fcb.numBlocks = NMOverM(file.size, MINBLOCKSIZE);
        fcb.fileIndex = find_vacant_space(parent, file.name);
    }
    // failed
    else {
        free(fcb.buf);
        return -1;
    }
    // consistent stuff among read and write
    fcb.parent = parent;
    fcb.buflen = B_CHUNK_SIZE;
    fcb.activeFlags = flags;
    strncpy(fcb.fileInfo->name, file.name, DE_NAME_SIZE);
    fcb.fileInfo->size = file.size;
    fcb.fileInfo->location = file.location;
    fcb.fileInfo->dateCreated = file.dateCreated;
    fcb.fileInfo->dateModified = file.dateModified;
    fcb.fileInfo->dateLastAccessed = file.dateLastAccessed;
    fcb.fileInfo->isDirectory = file.isDirectory;
    int parentInBlocks = NMOverM(DE_SIZE, MINBLOCKSIZE);
    parent[fcb.fileIndex] = *fcb.fileInfo;
    fileWrite(parent, parentInBlocks, parent[0].location);
    fcbArray[returnFd] = fcb;
    return returnFd;
}


// Interface to seek function
int b_seek (b_io_fd fd, off_t offset, int whence)
    {
    if (startup == 0) b_init();  //Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS))
        {
        return (-1);                    //invalid file descriptor
        }


    return (0); //Change this
    }



// Interface to write function
int b_write (b_io_fd fd, char * buffer, int count) {
    if (startup == 0) b_init();  //Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS)) {
        printf("fail 1\n");
        return (-1);                    //invalid file descriptor
    }

    // check if there is an active fd
    if( fcbArray[fd].fileInfo == NULL ) {
        printf("fail 2\n");
        return -1;
    }

    // check if there is a valid length
    if( count < 0 ) {
        printf("fail 3\n");
        return -1;
    }

    // check the flag
    if( fcbArray[fd].activeFlags & O_RDONLY) {
        printf("b_write: file is read only\n");
        return -1;
    }

    b_fcb fcb = fcbArray[fd];

    int bytesInBuff;
    int part1, part2, part3;
    int numBlocks;
    if( fcb.fileInfo->size < 1 ) {
        fcb.fileInfo->size = 0;
        bytesInBuff = B_CHUNK_SIZE;
        fcb.buflen = B_CHUNK_SIZE;
    }
    else {
        bytesInBuff = fcb.buflen - fcb.index;
    }

    int remainingSpace = (fcb.fileInfo->size + count) - (fcb.numBlocks * MINBLOCKSIZE);
    int additionalBlocks = NMOverM(remainingSpace, MINBLOCKSIZE);

    if( additionalBlocks > 0 ) {
        additionalBlocks = additionalBlocks > fcb.numBlocks ? additionalBlocks : fcb.numBlocks;
        int newBlocks = getFreeBlocks(additionalBlocks);
        int lastBlock = fileSeek(fcb.fileInfo->location, fcb.numBlocks - 1);
        if(fcb.fileInfo->location == -1) {
            fcb.currentBlock = newBlocks;
            fcb.fileInfo->location = newBlocks;
        }
        else {
            fat[lastBlock] = newBlocks;
        }
        fcb.numBlocks += additionalBlocks;
    }

    if( bytesInBuff >= count ) {
        part1 = count;
        part2 = 0;
        part3 = 0;
    }
    else {
        part1 = bytesInBuff;
        part3 = count - bytesInBuff;
        numBlocks = part3 / B_CHUNK_SIZE;
        part2 = numBlocks * B_CHUNK_SIZE;
        part3 = part3 - part2;
    }
    if( part1 > 0 ) {
        memcpy(fcb.buf + fcb.index, buffer, part1);
        fileWrite(fcb.buf, 1, fcb.currentBlock);
        fcb.index += part1;
    }
    if( part2 > 0 ) {
        int blocksWritten = fileWrite(buffer + part1, numBlocks, fcb.currentBlock);
        fcb.currentBlock = fileSeek(fcb.currentBlock, numBlocks);
        part2 = blocksWritten * B_CHUNK_SIZE;
    }
    if( part3 > 0 ) {
        fcb.index = 0;
        memcpy(fcb.buf, buffer + part1 + part2, part3);
        fcb.currentBlock = fileSeek(fcb.currentBlock, 1);
        fileWrite(fcb.buf, 1, fcb.currentBlock);
        fcb.index += part3;
    }
    fcb.fileInfo->size += part1 + part2 + part3;
    fcbArray[fd] = fcb;
    struct DE* parent = fcbArray[fd].parent;
    parent[fcb.fileIndex] = *fcb.fileInfo;
    int parentSizeInBlocks = NMOverM(parent->size, MINBLOCKSIZE);
    fileWrite(parent, parentSizeInBlocks, parent->location);
    return part1 + part2 + part3;
}



// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read (b_io_fd fd, char * buffer, int count) {
    if (startup == 0) b_init();  //Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS)) {
        printf("fail 1\n");
        return (-1);                    //invalid file descriptor
    }
    // unused file descriptor
    if( fcbArray[fd].fileInfo == NULL ) {
        printf("fail 1\n");
        return -1;
    }

    if( count < 0 ) {
        printf("fail 1\n");
        return -1;
    }

    if( fcbArray[fd].activeFlags & O_WRONLY) {
        printf("b_read: file is write only\n");
        return -1;
    }

    int part1, part2, part3;
    int numBlocks;
    b_fcb fcb = fcbArray[fd];
    int bytesInBuff;
    if(fcb.remainingBytes == fcb.fileInfo->size) {
        bytesInBuff = B_CHUNK_SIZE;
        fcb.buflen = B_CHUNK_SIZE;
        fcb.index = 0;
        fileRead(fcb.buf, 1, fcb.currentBlock);
    }
    else {
        bytesInBuff = fcb.buflen - fcb.index;
    }

    if( count > fcb.remainingBytes ) {
        count = fcb.remainingBytes;
    }
    if( bytesInBuff > count ) {
        part1 = count;
        part2 = 0;
        part3 = 0;
    }
    else {
        part1 = bytesInBuff;
        part3 = count - bytesInBuff;
        numBlocks = part3/B_CHUNK_SIZE;
        part2 = numBlocks * B_CHUNK_SIZE;
        part3 -= part2;
    }
    if( part1 > 0 ) {
        memcpy(buffer, fcb.buf + fcb.index, part1);
        fcb.index += part1;
    }
    if( part2 > 0 ) {
        int blocksRead = fileRead(buffer + part1, numBlocks, fcb.currentBlock);
        fcb.currentBlock = fileSeek(fcb.currentBlock, numBlocks);
        part2 = blocksRead * B_CHUNK_SIZE;
    }
    if( part3 > 0 ) {
        fcb.currentBlock = fileSeek(fcb.currentBlock, 1);
        int blocksRead = fileRead(fcb.buf, 1, fcb.currentBlock);
        fcb.index = 0;
        fcb.buflen = blocksRead * B_CHUNK_SIZE;
        if( fcb.buflen < part3 ) {
            part3 = fcb.buflen;
        }
        if( part3 > 0 ) {
            memcpy(buffer + part1 + part2, fcb.buf + fcb.index, part3);
            fcb.index += part3;
        }
    }
    fcb.remainingBytes = fcb.remainingBytes - part1 - part2 - part3;
    fcbArray[fd] = fcb;
    return part1 + part2 + part3;
}

// Interface to Close the file
int b_close (b_io_fd fd)
{
    free(fcbArray[fd].buf);
    fcbArray[fd].fileInfo = NULL;
    fcbArray[fd].buf = NULL;
    return 0;
}
