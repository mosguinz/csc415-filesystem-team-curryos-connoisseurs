/**************************************************************
* Class::  CSC-415-0# Spring 2024
* Name::
* Student IDs::
* GitHub-Name::
* Group-Name::
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

#endif

