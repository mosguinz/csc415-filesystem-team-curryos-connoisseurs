/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
* Project:: Basic File System
*
* File:: b_io.h
*
* Description:: Interface of basic I/O Operations
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

typedef int b_io_fd;

b_io_fd b_open (char * filename, int flags);
int b_read (b_io_fd fd, char * buffer, int count);
int b_write (b_io_fd fd, char * buffer, int count);
int b_seek (b_io_fd fd, off_t offset, int whence);
int b_close (b_io_fd fd);

typedef struct b_fcb
	{
	struct DE * fileInfo;	//holfd information relevant to file operations
	struct DE * parent;	//holfd information relevant to file operations

	char * buf;		//holds the open file buffer
	int index;		//holds the current position in the buffer
    int remainingBytes; // the number of bytes that are left in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	int currentBlock;	//holds position within file in blocks
	int numBlocks;		//holds the total number of blocks in file
    int fileIndex;      //holds the index in the parent of the file

	int activeFlags;	//holds the flags for the opened file
	} b_fcb;

#endif

