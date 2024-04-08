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
#include "freespace.h"

struct VCB * volumeControlBlock;
struct DE * root;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize){
	long const VCBSIGNATURE = 8357492010847392157;
	struct VCB * buffer;
	volumeControlBlock = (struct VCB *) malloc(MINBLOCKSIZE);
	root = (struct DE *) malloc(MINBLOCKSIZE);

	printf ("Initializing File System with %ld blocks \
		with a block size of %ld\n", numberOfBlocks, blockSize);

	buffer = (struct VCB *) malloc(512);
	LBAread ( buffer, 1, 0);

	if ( buffer->signature == VCBSIGNATURE ){
        	LBAread(volumeControlBlock, 1, 0);
		printf("Disk already formatted\n");
		LBAread ( root, 1, volumeControlBlock->rootLocation );
	}else{
		printf("Formatting disk\n");
		memset(volumeControlBlock, 0, MINBLOCKSIZE);
		volumeControlBlock -> signature = VCBSIGNATURE;
		volumeControlBlock -> totalBlocks = numberOfBlocks;
		volumeControlBlock -> blockSize = blockSize;
		volumeControlBlock -> firstBlock = 
			initFreespace(numberOfBlocks, blockSize);
		volumeControlBlock -> rootLocation = createDirectory(50, NULL);
	}

	free(buffer);
	createDirectory(50, root);

	return 0;
}

void exitFileSystem (){
	printf ("System exiting\n");
}

