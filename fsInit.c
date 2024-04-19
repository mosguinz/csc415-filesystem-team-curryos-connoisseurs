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
int * fat;
struct DE * root;
struct DE * cwd;
char * cwdPathName;

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize){
	long const 		VCBSIGNATURE = 8357492010847392157;
	struct VCB * 		buffer;
	int 			freeSpaceBlocks;

	freeSpaceBlocks =
		((numberOfBlocks + MINBLOCKSIZE - 1) / MINBLOCKSIZE );
	fat = (int * ) malloc(sizeof(int) * numberOfBlocks * MINBLOCKSIZE);
	volumeControlBlock = (struct VCB *) malloc(MINBLOCKSIZE);
	root = (struct DE *) malloc(7 * MINBLOCKSIZE);
	cwd = (struct DE *) malloc(7 * MINBLOCKSIZE);
	cwdPathName = (char *) malloc(36);

	printf ("Initializing File System with %ld blocks \
		with a block size of %ld\n", numberOfBlocks, blockSize);

	buffer = (struct VCB *) malloc(MINBLOCKSIZE);
	LBAread ( buffer, 1, 0);

	if ( buffer->signature == VCBSIGNATURE ){
        	LBAread(volumeControlBlock, 1, 0);
		printf("Disk already formatted\n");
		LBAread ( root, 1, volumeControlBlock->rootLocation );
		LBAread ( fat, volumeControlBlock->totalFreeSpace,
			volumeControlBlock->freeSpaceLocation);
		//fs_setcwd("/");
	}else{
		printf("Formatting disk\n");
		memset(volumeControlBlock, 0, MINBLOCKSIZE);
		volumeControlBlock -> signature = VCBSIGNATURE;
		volumeControlBlock -> totalBlocks = numberOfBlocks;
		volumeControlBlock -> blockSize = blockSize;
		volumeControlBlock -> firstBlock =
			initFreespace(numberOfBlocks, MINBLOCKSIZE);
		printf("Free space initialized\n");
		volumeControlBlock -> freeSpaceLocation = 1;
		volumeControlBlock -> rootLocation = 
			createDirectory(50, NULL);
		LBAwrite(volumeControlBlock, 1, 0);
	}
	strncpy(cwdPathName, "/", 36);
	printf("Setting CWD to %s\n", cwdPathName);

	createDirectory(200, root);

	free(buffer);

	return 0;
}

void exitFileSystem (){
	fileWrite(volumeControlBlock, 1, 0);
	fileWrite(fat, 
		sizeof(int) * volumeControlBlock -> totalFreeSpace,
		volumeControlBlock -> freeSpaceLocation);
	printf ("System exiting\n");
}

