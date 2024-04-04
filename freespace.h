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

int initFreespace(uint64_t numberOfBlocks, uint64_t blockSize);
int getFreeBlocks(uint64_t numberOfBlocks);

#endif

