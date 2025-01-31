/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
* Project:: Basic File System
*
* File:: freespace.h
*
* Description:: header file for free space functions
*
**************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "fsLow.h"

#ifndef _FREESPACE_H
#define _FREESPACE_H
#include <fcntl.h>

typedef int b_io_fd;

/*
 * initilize freespace table
 *
 * @param numberOfBlocks the number of blocks the file system has
 * @param blockSize the size of a block
 * @return the index of the first usable block
 */
int initFreespace(uint64_t numberOfBlocks, uint64_t blockSize);

/*
 * get free blocks
 *
 * @param numberOfBlocks the number of blocks that are needed
 * @return the location of the first block that can be used. -1 on error
 */
int getFreeBlocks(uint64_t numberOfBlocks);

/*
 * write blocks to disk
 *
 * @param buff the buffer that is being written
 * @param numberOfBlocks the number of blocks being written
 * @param location the location where the blocks are written
 * @return the number of blocks written
 */
int fileWrite(void* buff, int numberOfBlocks, int location);

/*
 * read blocks from the disk
 *
 * @param buff the buffer that is being filled
 * @param numberOfBlocks the number of blocks being read
 * @param location the location where the blocks are read from
 * @return the number of blocks read
 */
int fileRead(void* buff, int numberOfBlocks, int location);

/*
 * return free blocks
 *
 * @param location the location of the block for the blocks being returned
 * @return the number of blocks that were returned. -1 on error
 */
int returnFreeBlocks(int location);

/*
 * get the index n blocks over
 *
 * @param location the location of the block where the search is starting at
 * @param numberOfBlocks the number of blocks to move over
 * @return the index of the block n blocks over. -1 on error
 */
int fileSeek(int location, int numberOfBlocks);
#endif

