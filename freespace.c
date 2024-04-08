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
#include "fs_control.h"
#define BYTESIZE 8
int initFreespace(uint64_t numberOfBlocks, uint64_t blockSize){
    // rounding for blocks: (n + m - 1 )/ m
    // the number of blocks the freespace map needs
    int blocksNeeded = ( numberOfBlocks + blockSize - 1) / blockSize;
    int* freeSpaceList = malloc( blocksNeeded * blockSize );
    for( int i = 1; i < blocksNeeded; i++ ) {
        freeSpaceList[i] = i+1;
    }

    // mark the VCB as used
    freeSpaceList[0] = 0xFFFFFFFF;
    // mark the freespace map as used
    freeSpaceList[blocksNeeded] = 0xFFFFFFFF;
    // mark the end of the list
    freeSpaceList[numberOfBlocks - 1] = 0xFFFFFFFF;

    int blocksWritten = LBAwrite(freeSpaceList, blocksNeeded, 1);
    volumeControlBlock->totalFreeSpace = blocksWritten;
    volumeControlBlock->freeSpaceLocation = 1;
    return blocksWritten == -1 ? -1: blocksNeeded + 1;
}

/*
 * get free blocks
 *
 * @param numberOfBlocks the number of blocks that are needed
 * @return the location of the first block that can be used. -1 on error
 */
int getFreeBlocks(uint64_t numberOfBlocks) {
    int* freeBlocks;
	struct VCB * volumeControlBlock;
    if( LBAread(volumeControlBlock,1, 0 ) == -1) {
        return -1;
    }
    if( numberOfBlocks < 1 ) {
        return -1;
    }
    if( numberOfBlocks > volumeControlBlock->totalFreeSpace ) {
        return -1;
    }

    // first free block in the FAT table
    // laod in the freespace map
    int* table;
    int blocksRead = (volumeControlBlock->totalFreeSpace + volumeControlBlock->blockSize - 1) / volumeControlBlock->blockSize;
    LBAread(table, blocksRead, volumeControlBlock->freeSpaceLocation);
    int head = volumeControlBlock->firstBlock;
    int currBlockLoc = volumeControlBlock->firstBlock;
    int nextBlockLoc = table[currBlockLoc];
    volumeControlBlock->totalFreeSpace--;
    // jump through the blocks to find what the new first block will be
    for( int i = 1; i < numberOfBlocks; i ++ ) {
        currBlockLoc = nextBlockLoc;
        nextBlockLoc = table[currBlockLoc];
        volumeControlBlock->totalFreeSpace--;
    }
    table[currBlockLoc] = 0xFFFFFFFF;
    volumeControlBlock->firstBlock = nextBlockLoc;

    return head;
}

