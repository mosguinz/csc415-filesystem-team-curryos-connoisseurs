/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
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
    volumeControlBlock->freeSpaceSize = blocksNeeded;
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
    printf("current first block in getfreeblocks: %i\n", head);
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
 * return free blocks
 *
 * @param location the location of the block for the blocks being returned
 * @return the number of blocks that were returned. -1 on error
 */
int returnFreeBlocks(int location){
    printf("reached the return free blocks method\n");
    printf("the location: %i\n", location);
    // TODO: Check if location
    if( location < 1 || location > volumeControlBlock->totalBlocks ) {
        printf("hit the early return\n");
        return -1;
    }
    int currBlockLoc = location;
    printf("currBlockLoc: %i\n", currBlockLoc);
    int i = 0;
    while( fat[currBlockLoc] != 0xFFFFFFFF ) {
        currBlockLoc = fat[currBlockLoc];
        // printf("currBlockLoc: %i\n", currBlockLoc);
        i++;
    }
    fat[currBlockLoc] = volumeControlBlock->firstBlock;
    volumeControlBlock->firstBlock = location;
    printf("vcb first block: %i\n", volumeControlBlock->firstBlock);
    return i;
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
    for( int i = 0; location != -1l && i < numberOfBlocks; i++ ) {
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
    for( int i = 0; location != -1l && i < numberOfBlocks; i++ ) {
        if( LBAread(buff + blockSize*i, 1, location) == -1) {
            return -1;
        }
        location = fat[location];
        blocksRead++;
    }
    return blocksRead;
}

/*
 * get the index n blocks over
 *
 * @param location the location of the block where the search is starting at
 * @param numberOfBlocks the number of blocks to move over
 * @return the index of the block n blocks over. -1 on error
 */
int fileSeek(int location, int numberOfBlocks){
    for( int i = 0; location != -1l && i < numberOfBlocks; i ++ ) {
        location = fat[location];
    }
    return location;
}
