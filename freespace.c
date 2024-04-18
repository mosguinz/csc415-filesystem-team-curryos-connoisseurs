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

/*
 * initilize freespace table
 *
 * @param numberOfBlocks the number of blocks the file system has
 * @param blockSize the size of a block
 * @return the index of the first usable block
 */
int initFreespace(uint64_t numberOfBlocks, uint64_t blockSize){
    // rounding for blocks: (n + m - 1 )/ m
    // the number of blocks the freespace map needs
    int blocksNeeded = NMOverM(sizeof(int)*numberOfBlocks, blockSize);
    // int* fat = malloc( blocksNeeded * blockSize );
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
    volumeControlBlock->totalFreeSpace = numberOfBlocks - blocksNeeded;
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

/*
 * write blocks to disk
 *
 * @param buff the buffer that is being written
 * @param numberOfBlocks the number of blocks being written
 * @param location the location where the blocks are written
 * @return the number of blocks written
 */
int fileWrite(void* buff, int numberOfBlocks, int location){
    int blockSize = volumeControlBlock->blockSize;
    int blocksWritten = 0;
    for( int i = 0; location != 0xFFFFFFFF && i < numberOfBlocks; i++ ) {
        if( LBAwrite(buff + blockSize * i, 1, location) == -1 ) {
            return -1;
        }
        location = fat[location];
        blocksWritten++;
    }
    return blocksWritten;
}

/*
 * read blocks from the disk
 *
 * @param buff the buffer that is being filled
 * @param numberOfBlocks the number of blocks being read
 * @param location the location where the blocks are read from
 * @return the number of blocks read
 */
int fileRead(void* buff, int numberOfBlocks, int location){
    int blockSize = volumeControlBlock->blockSize;
    int blocksRead = 0;
    for( int i = 0; location != 0xFFFFFFFF && i < numberOfBlocks; i++ ) {
        if( LBAread(buff + blockSize*i, 1, location) == -1) {
            return -1;
        }
        location = fat[location];
        blocksRead++;
    }
    return blocksRead;
}
