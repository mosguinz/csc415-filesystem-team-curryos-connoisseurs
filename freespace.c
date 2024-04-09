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
#include "fsUtils.h"
#define BYTESIZE 4
int initFreespace(uint64_t numberOfBlocks, uint64_t blockSize){
    // rounding for blocks: (n + m - 1 )/ m
    // the number of blocks the freespace map needs
    int blocksNeeded = NMOverM(sizeof(int)*numberOfBlocks, blockSize);
    int* fat = malloc( blocksNeeded * blockSize );
    for( int i = 1; i < numberOfBlocks; i++ ) {
        fat[i] = i+1;
    }
    printf("\nblocks needed: %i\n", blocksNeeded);

    // mark the VCB as used
    fat[0] = 0xFFFFFFFF;
    // mark the freespace map as used
    fat[blocksNeeded] = 0xFFFFFFFF;
    fat[numberOfBlocks] = 0xFFFFFFFF;

    int blocksWritten = LBAwrite(fat, blocksNeeded, 1);
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
    if( numberOfBlocks < 1 ) {
        return -1;
    }
    if( numberOfBlocks > volumeControlBlock->totalFreeSpace ) {
        return -1;
    }

    // first free block in the FAT table
    int head = volumeControlBlock->firstBlock;
    int currBlockLoc = volumeControlBlock->firstBlock;
    int nextBlockLoc = fat[currBlockLoc];
    volumeControlBlock->totalFreeSpace--;
    // jump through the blocks to find what the new first block will be
    for( int i = 1; i < numberOfBlocks; i ++ ) {
        currBlockLoc = nextBlockLoc;
        nextBlockLoc = fat[currBlockLoc];
        volumeControlBlock->totalFreeSpace--;
    }
    fat[currBlockLoc] = 0xFFFFFFFF;
    volumeControlBlock->firstBlock = nextBlockLoc;

    return head;
}

