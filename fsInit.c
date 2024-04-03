/**************************************************************
* Class::  CSC-415-0# Spring 024
* Name::
* Student IDs::
* GitHub-Name::
* Group-Name::
* Project:: Basic File System
*
* File:: fsInit.c
*
* Description:: Main driver for file system assignment.
*
* This file is where you will start and initialize your system
*
**************************************************************/
/*
struct VCB
{
	long signature; 	// VCB identifier
	int totalBlocks; 	// blocks in volume
	int blockSize; 		// size of blocks
	int rootLocation; 	// location of root directory
	int firstBlock; 	// location of the first usable block
	int freeSpaceLocation; 	// location of the free space block
	int totalFreeSpace; 	// number of free block
};
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize){
	struct VCB * volumeControlBlock;
	long const VCBSIGNATURE = 8357492010847392157;
	struct VCB * buffer;

	printf ("Initializing File System with %ld blocks \
		with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	buffer = (struct VCB *) malloc(512);
	LBAread ( buffer, 1, 0);

	if ( buffer->signature == VCBSIGNATURE )
		printf("Disk already formatted\n");
	else{
		printf("Formatting disk\n");
		volumeControlBlock = (struct VCB *) malloc(sizeof(struct VCB));
		volumeControlBlock -> signature = VCBSIGNATURE;
		volumeControlBlock -> totalBlocks = numberOfBlocks;
		volumeControlBlock -> blockSize = blockSize;
		volumeControlBlock -> rootLocation = createDirectory(50, NULL);
		volumeControlBlock -> firstBlock = 2;
		volumeControlBlock -> freeSpaceLocation = 3;
		volumeControlBlock -> totalFreeSpace = 4;

		LBAwrite ( volumeControlBlock, 1, 0 );
		free(volumeControlBlock);
	}

	struct DE * newBuffer = malloc(512);
	LBAread ( newBuffer, 1, 10);
	printf("%s\n", newBuffer[1].name);

	free(buffer);
	return 0;
}
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}


