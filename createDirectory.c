/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
* Project:: Basic File System
*
* File:: createDirectory.c
*
* Description:: basic directory creation method
*
**************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#include "fsLow.h"
#include "mfs.h"
#include "fs_control.h"
#include "freespace.h"
#include "fsUtils.h"

int createDirectory(int numberOfEntries, struct DE *parent)
{
	int bytes;		// The number of bytes needed for entries
	int blockCount;		// The total number of blocks needed
	int maxEntryCount;	// How many entries we can fit into our blocks
	int blocksRequested; 	// Location of first block for new directory

	/* From # of entries argument determine the maximum number of
	 * entries we can actually fit */
	bytes = sizeof(struct DE) * numberOfEntries;
	blockCount = NMOverM(bytes, MINBLOCKSIZE);
		// ((bytes + MINBLOCKSIZE - 1) / MINBLOCKSIZE);
	maxEntryCount = (blockCount * MINBLOCKSIZE) / sizeof(struct DE);

	// Allocate memory for directory
	struct DE *buffer = malloc(blockCount * MINBLOCKSIZE);
	memset(buffer, 0, blockCount *  MINBLOCKSIZE);
	if (buffer == NULL)
	{
		printf("Error: Could not allocate memory for new directory\n");
		exit(EXIT_FAILURE);
	}

	// Initialize each directory entry in new directory to an unused state
	for (int i = 0; i < maxEntryCount; i++)
		buffer[i].location = -2;

	// Request blocks from freespace system
	blocksRequested = getFreeBlocks(blockCount);
	printf("Creating Directory at %d, with size %ld bytes\n", blocksRequested, maxEntryCount * sizeof(struct DE));

	// Initialize dot and dot dot entries of the new directory
	buffer[0].location = blocksRequested;
	buffer[0].size = maxEntryCount * sizeof(struct DE);
	buffer[0].isDirectory = 1;
	strncpy(buffer[0].name, ".", DE_NAME_SIZE);

	// Set timestamp fields
	time_t tm = time(NULL);
	buffer[0].dateCreated = tm;
	buffer[0].dateModified = tm;
	buffer[0].dateLastAccessed = tm;

	// If no parent is passed, initialize root directory @UNSAFE
	// Cannot handle if root already exists
	if (parent == NULL)
	{
		printf("Initializing root directory\n");
		volumeControlBlock->rootSize = blockCount;
		volumeControlBlock->rootLocation = blocksRequested;
		buffer[1] = buffer[0];

		/* If a parent directory entry is provided, initialize new directory's
		 * parent and link the new directory entry back to the parent */
	}
	else
	{
		printf("Creating a new directory\n");
		buffer[1].location = parent -> location;
		buffer[1].size = parent -> size;
	}
	strncpy(buffer[1].name, "..", DE_NAME_SIZE);
	buffer[1].isDirectory = 1;

	// Write newly created directory to disk
	int blocksWritten = fileWrite(buffer, blockCount, blocksRequested);
    free(buffer);
	return blocksRequested;
}
