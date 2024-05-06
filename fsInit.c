/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
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
#include "fsUtils.h"
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
    // this variable isn't used. delete the lines if they are extra. LOVE ARJUN
	// int 			freeSpaceBlocks;

	// freeSpaceBlocks =
	// 	((numberOfBlocks + MINBLOCKSIZE - 1) / MINBLOCKSIZE );
    int blocksNeeded = NMOverM(sizeof(int)*numberOfBlocks, blockSize);
    fat = (int *) malloc(blocksNeeded * blockSize );
	volumeControlBlock = (struct VCB *) malloc(MINBLOCKSIZE);
	root = (struct DE *) malloc(DE_SIZE);
	cwdPathName = (char *) malloc(36);

	printf ("Initializing File System with %ld blocks \
		with a block size of %ld\n", numberOfBlocks, blockSize);

	buffer = (struct VCB *) malloc(MINBLOCKSIZE);
	LBAread ( buffer, 1, 0);

	if ( buffer->signature == VCBSIGNATURE ){
        	LBAread(volumeControlBlock, 1, 0);
		printf("Disk already formatted\n");
		LBAread ( root, volumeControlBlock->rootSize, volumeControlBlock->rootLocation );
		LBAread ( fat, volumeControlBlock->freeSpaceSize,
			volumeControlBlock->freeSpaceLocation);
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

		LBAread ( root, volumeControlBlock->rootSize, volumeControlBlock->rootLocation );
	}
	fs_setcwd("/");
	strncpy(cwdPathName, "/", 36);
	printf("Setting CWD to %s\n", cwdPathName);

	free(buffer);

	return 0;
}

void exitFileSystem (){
	fileWrite(volumeControlBlock, 1, 0);
	fileWrite(fat,
		MINBLOCKSIZE * volumeControlBlock -> freeSpaceSize,
		volumeControlBlock -> freeSpaceLocation);
    free(fat);
    free(volumeControlBlock);
    free(root);
    free(cwdPathName);
    free(cwd);
	printf ("System exiting\n");
}

