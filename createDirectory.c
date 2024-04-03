// TODO HEADER GUARD
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "fsLow.h"
#include "mfs.h"

// TODO ERR HANDLE
// TODO REQUESTED BLOCKS MUST REQUEST BLOCKS FROM FREE SPACE SYSTEM
// TODO CHANGE NAMES AND CLEAN UP
int createDirectory ( int numberOfEntries, struct DE * parent ){
	
	int requestedBlocks;

	// Determine how many DEs we can actually fit into requested blocks
	int space = sizeof(struct DE) * numberOfEntries;
	int blocks = ( space + MINBLOCKSIZE - 1 ) / MINBLOCKSIZE;
	int bytes = blocks * MINBLOCKSIZE;
	int actualNumberOfEntries = bytes/sizeof(struct DE);
	struct DE * buffer = malloc (bytes);

	// Initialize each directory entry to an unused state
	for ( int i = 0 ; i < actualNumberOfEntries ; i ++ ){
		buffer[i].location = -2;
	}
	
	// Request blocks from freespace system
	requestedBlocks = 10;

	strncpy(buffer[0].name, ".", 25);
	buffer[0].location = requestedBlocks;
	buffer[0].size = actualNumberOfEntries * sizeof(struct DE);
	
	// If no parent is passed, initialize root directory @UNSAFE
		// Cannot handle if root already exists
	if ( parent == NULL ){
		printf("Initializing root directory\n");
		buffer[1] = buffer[0];
		strncpy(buffer[1].name, "..", 25);

	}else{
		printf("Creating a new directory\n");
	}

	// Write newly created directory to disk
	LBAwrite(buffer, blocks, requestedBlocks);

	return requestedBlocks;
}
