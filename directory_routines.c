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
	int ret;
	int readret;
	int emptyIndex;
	char * directoryName;
	struct PPRETDATA *parsepathinfo;
	struct DE * parent;
	struct DE * newDirectory;

	parsepathinfo = malloc(sizeof(struct PPRETDATA));
	newDirectory = malloc(MINBLOCKSIZE);
	if ( !parsepathinfo || !newDirectory ){
		perror("malloc");
		return -1;
	}

	//parsePath(pathname, parsepathinfo);
	parsepathinfo->parent = root;
	parsepathinfo->lastElementName = "Love Letter to Arjun";

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
	readret = fileRead(newDirectory, 1, ret);
	if ( readret == -1){
		perror("File Read");
		return -1;
	}
	emptyIndex = find_vacant_space ( parent );
	parent[emptyIndex] = newDirectory[0];
	strncpy(parent[emptyIndex].name, directoryName, 36);

	printf("New Directory Name:\t%s\n", parent[emptyIndex].name);
	printf("New Directory Size:\t%d\n", parent[emptyIndex].size);
	printf("New Directory Created At:\t%ld\n", parent[emptyIndex].location);
	
	// Write changes back to parent directory to complete linking
	fileWrite(parent, (parent->size/MINBLOCKSIZE), parent->location);

	return 0;
}

// Helper function for finding an empty space in a directory
int find_vacant_space ( struct DE * directory ){
	for ( int i = 0 ; i < (root->size)/sizeof(struct DE) ; i++ )
		if ( (root + i)->location == -2 )
			return i;
	perror("Directory is full");
	return -1;
}
