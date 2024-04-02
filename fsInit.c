/**************************************************************
* Class::  CSC-415-0# Spring 2024
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

int compare_hex_string (long val, char * );

int initFileSystem (uint64_t numberOfBlocks, uint64_t blockSize){
	struct VCB * volumeControlBlock;
	long const VCBSIGNATURE = 8357492010847392157;
	char * buffer;

	printf ("Initializing File System with %ld blocks \
		with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */

	buffer = (char *) malloc(512);
	LBAread ( buffer, 1, 0);

	volumeControlBlock = (struct VCB *) malloc(sizeof(struct VCB));

	compare_hex_string (VCBSIGNATURE, buffer);
/*
	volumeControlBlock -> signature = VCBSIGNATURE;
	volumeControlBlock -> totalBlocks = numberOfBlocks;
	volumeControlBlock -> rootLocation = 1;
	volumeControlBlock -> firstBlock = 2;
	volumeControlBlock -> freeSpaceLocation = 3;
	volumeControlBlock -> totalFreeSpace = 4;
	
	LBAwrite ( volumeControlBlock, 1, 0 );
*/
	free(volumeControlBlock);
	free(buffer);
	return 0;
}
	
	
void exitFileSystem ()
	{
	printf ("System exiting\n");
	}

// Compare a 64 bit hex value given as a string to a decimal value
int compare_hex_string (long val, char * buffer) {
	int result;
	int power;

	//printf("%s\t%li\n", buffer, val);
	result = 0;
	power = 1;
	char digit;
	for ( int i = 0 ; i < 8 ; i++ ){
		printf("%d\n", *(buffer + i) );
		power *= 16;
		result += power;
	}

}
