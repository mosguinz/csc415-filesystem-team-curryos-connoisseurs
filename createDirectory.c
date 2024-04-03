#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "fsLow.h"
#include "mfs.h"

#ifndef CREATE_DIRECTORY
#define CREATE_DIRECTORY

// TODO: REQUESTED BLOCKS MUST REQUEST BLOCKS FROM FREE SPACE SYSTEM
int createDirectory ( int numberOfEntries, struct DE * parent ){
	// Determine how many DEs we can actually fit into requested blocks
	int bytes = sizeof(struct DE) * numberOfEntries;
	int numberOfBlocks = (bytes + MINBLOCKSIZE - 1) / MINBLOCKSIZE;
	int blocksInBytes = numberOfBlocks * MINBLOCKSIZE;
	int actualNumberOfEntries = blocksInBytes/sizeof(struct DE);
	
	// Allocate memory for directory
	struct DE * buffer = malloc (blocksInBytes);
	if( buffer == NULL ){
		printf("Error: Could not allocate memory for directory\n");
		return -1;
	}

	// Initialize each directory entry to an unused state
	for ( int i = 0 ; i < actualNumberOfEntries ; i++ ){
		buffer[i].location = -2;
	}
	
	// Request blocks from freespace system
	int blocksRequested = 10;

	strncpy(buffer[0].name, ".", 25);
	buffer[0].location = blocksRequested;
	buffer[0].size = actualNumberOfEntries * sizeof(struct DE);
	
	// If no parent is passed, initialize root directory @UNSAFE
		// Cannot handle if root already exists
	if ( parent == NULL ){
		printf("Initializing root directory\n");
		buffer[1] = buffer[0];
		strncpy(buffer[1].name, "..", 25);

	} else {
		printf("Creating a new directory\n");
	}

	// Write newly created directory to disk
	int blocksWritten = LBAwrite(buffer, numberOfBlocks, blocksRequested);
	if( blocksWritten = -1 ){
		printf("Error: Could not write directory to disk\n");
		return -1;
	}

	return blocksRequested;
}

#endif