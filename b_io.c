/**************************************************************
* Class::  CSC-415-0# Spring 2024
* Name::
* Student IDs::
* GitHub-Name::
* Group-Name::
* Project:: Basic File System
*
* File:: b_io.c
*
* Description:: Basic File System - Key File I/O Operations
*
**************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>			// for malloc
#include <string.h>			// for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "b_io.h"
#include "fs_control.h"
#include "fsUtils.h"
#include "freespace.h"

#define MAXFCBS 20
#define B_CHUNK_SIZE 512

typedef struct b_fcb
	{
	struct DE * fileInfo;	//holfd information relevant to file operations

	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
    int blocksRead; //the number of blocks that have been read so far
    int remainingBytes; // the number of bytes that are left in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	int currentBlock;	//holds position within file in blocks
	int numBlocks;		//holds the total number of blocks in file

	int activeFlags;	//holds the flags for the opened file
	} b_fcb;

b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

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
			return i;		//Not thread safe (But do not worry about it for this assignment)
			}
		}
	return (-1);  //all in use
	}

// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open (char * filename, int flags){
	int ret;
	b_io_fd returnFd;


	if (startup == 0) b_init();  //Initialize our system

	returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's

	return (returnFd);						// all set
}


// Interface to seek function
int b_seek (b_io_fd fd, off_t offset, int whence)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}


	return (0); //Change this
	}



// Interface to write function
int b_write (b_io_fd fd, char * buffer, int count)
	{
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}


	return (0); //Change this
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
		return (-1); 					//invalid file descriptor
	}
    // unused file descriptor
    if( fcbArray[fd].fileInfo == NULL ) {
        return -1;
    }

    if( count < 0 ) {
        return -1;
    }

    // int userPosition = 0;
    // int bytesRead = 0;
    // b_fcb fcb = fcbArray[fd];

    // // ensure that only as many as there are available are returned to the user
    // if( count > fcb.remainingBytes ) {
    //     count = fcb.remainingBytes;
    // }
    // // iterate, so that as per specification the buffer can be copied over in chunks
    // while( count > B_CHUNK_SIZE - fcb.index ) {
    //     int size = B_CHUNK_SIZE - fcb.index;
    //     memcpy(buffer + userPosition, fcb.buf + fcb.index, size);
    //     userPosition += size;
    //     fcb.index = 0;
    //     int numBlocks = fileRead(fcb.buf, 1, fcb.fileInfo->location + fcb.blocksRead);
    //     if( numBlocks != 1 ) {
    //         return -1;
    //     }
    //     fcb.blocksRead += numBlocks;
    //     count -= size;
    //     bytesRead += size;
    //     fcb.remainingBytes -= size;
    // }
    // memcpy(buffer + userPosition, fcb.buf + fcb.index, count);
    // fcb.index += count;
    // fcb.remainingBytes -= count;
    // fcbArray[fd] = fcb;
    // return bytesRead + count;

    int part1, part2, part3;
    int numBlocks;
    b_fcb fcb = fcbArray[fd];
    int bytesInBuff = fcb.buflen - fcb.index;

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
        memcpy(buffer, fcb.buf, part1);
    }
    if( part2 > 0 ) {
        int blocksRead = fileRead(buffer + part1, numBlocks, fcb.currentBlock);
        fcb.currentBlock = fileSeek(fcb.currentBlock, numBlocks);
        part2 = blocksRead * B_CHUNK_SIZE;
    }
    if( part3 > 0 ) {
        int blocksRead = fileRead(fcb.buf, 1, fcb.currentBlock);
        fcb.currentBlock = fileSeek(fcb.currentBlock, 1);
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
}
