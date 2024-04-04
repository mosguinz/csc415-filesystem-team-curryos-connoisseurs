/**************************************************************
* Class::  CSC-415-0# Spring 2024
* Name::
* Student IDs::
* GitHub-Name::
* Group-Name::
* Project:: Basic File System
*
* File:: freespace.c
*
* Description:: freespace functions
*
**************************************************************/
#include "freespace.h"
#define BYTESIZE 8
int initFreespace(uint64_t numberOfBlocks, uint64_t blockSize){
    // rounding for blocks: (n + m - 1 )/ m
    // the number of blocks the freespace map needs
    int bytesNeeded = ( numberOfBlocks + BYTESIZE - 1) / BYTESIZE;
    int blocksNeeded = ( bytesNeeded + blockSize - 1) / blockSize;
    int* freeSpaceList = malloc( blocksNeeded * blockSize );
    for( int i = 0; i < numberOfBlocks; i ++ ) {
        freeSpaceList[i] = i+1;
    }

    // mark the VCB as used
    freeSpaceList[0] = 0xFFFFFFFF;
    // mark the freespace map as used
    freeSpaceList[blocksNeeded] = 0xFFFFFFFF;
    // mark the end of the list
    freeSpaceList[numberOfBlocks - 1] = 0xFFFFFFFF;

    int blocksWritten = LBAwrite(freeSpaceList, blocksNeeded, 1);
    return blocksWritten == -1 ? -1: blocksNeeded + 1;
}

int getFreeBlocks(uint64_t numberOfBlocks) {
    int* freeBlocks;
	struct VCB * volumeControlBlock;
    if( LBAread(volumeControlBlock,1, 0 ) == -1) {
        return -1;
    }
}

