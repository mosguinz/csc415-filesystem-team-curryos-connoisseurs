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
};
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "fsLow.h"
#include "mfs.h"
#include "fs_control.h"

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize){
	struct VCB * volumeControlBlock;
	long const VCBSIGNATURE = 8357492010847392157;
	struct VCB * buffer;

	printf ("Initializing File System with %ld blocks \
		with a block size of %ld\n", numberOfBlocks, blockSize);

	buffer = (struct VCB *) malloc(512);
	LBAread ( buffer, 1, 0);

	if ( buffer->signature == VCBSIGNATURE )
		printf("Disk already formatted\n");
	else{
		printf("Formatting disk\n");
		volumeControlBlock = (struct VCB *) malloc(MINBLOCKSIZE);
		memset(volumeControlBlock, 0, MINBLOCKSIZE);
		volumeControlBlock -> signature = VCBSIGNATURE;
		volumeControlBlock -> totalBlocks = numberOfBlocks;
		volumeControlBlock -> blockSize = blockSize;
		volumeControlBlock -> freeSpaceLocation = 3;
		volumeControlBlock -> rootLocation = createDirectory(50, NULL);
		volumeControlBlock -> firstBlock = 2;
		//volumeControlBlock -> totalFreeSpace = 4;

		LBAwrite ( volumeControlBlock, 1, 0 );
		free(volumeControlBlock);
	}

	free(buffer);

	return 0;
}
	
void exitFileSystem (){
	printf ("System exiting\n");
}

