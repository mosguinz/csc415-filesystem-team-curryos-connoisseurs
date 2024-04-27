#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include "mfs.h"
#include "fs_control.h"
#include "freespace.h"
#include "fsUtils.h"

#define DEFAULTDIRSIZE 50

int fs_mkdir (const char *pathname, mode_t mode){
	int ret;				// Used for error handling
	int emptyIndex;				// Track first unused entry in parent
	char * directoryName;			// Name of new directory
	struct PPRETDATA *parsepathinfo;	// Structure returned from parse path
	struct DE * parent;			// Track parent directory entry
	struct DE * newDirectory;		// Used for new directory entry

	parsepathinfo = malloc(sizeof(struct PPRETDATA));
	parsepathinfo->parent = malloc(7*MINBLOCKSIZE);
	newDirectory = malloc(MINBLOCKSIZE);
	if ( !parsepathinfo || !newDirectory ){
		perror("malloc");
		return -1;
	}

	parsePath(pathname, parsepathinfo);
	// Read all relevant data from parsepath needed for directory creation
	parent = parsepathinfo -> parent;
	directoryName = parsepathinfo->lastElementName;

	// Create and link new directory to an empty position in parent array
	ret = createDirectory(DEFAULTDIRSIZE, parent);
	if ( ret == -1 ){
		perror("Create Directory");
		return -1;
	}
	memset(newDirectory, 0, MINBLOCKSIZE);
	ret = fileRead(newDirectory, 1, ret);
	if ( ret == -1){
		perror("File Read");
		return -1;
	}
	emptyIndex = find_vacant_space ( parent );
	parent[emptyIndex] = newDirectory[0];
	strncpy(parent[emptyIndex].name, directoryName, 36);

	// Write changes back to parent directory to complete linking
	fileWrite(parent, (parent->size/MINBLOCKSIZE), parent->location);

	return 0;
}

int find_vacant_space ( struct DE * directory ){
	for ( int i = 0 ; i < (directory->size)/sizeof(struct DE) ; i++ )
		if ( (directory + i)->location == -2 )
			return i;
	perror("Directory is full");
	return -1;
}
