#ifndef _FSCTL
#define _FSCTL
// Specifications for volume control block
struct VCB
{
	long signature; 	// VCB identifier
	int totalBlocks; 	// blocks in volume
	int blockSize; 		// size of blocks
	int freeSpaceLocation; 	// location of the FAT table
	int rootLocation; 	// location of root directory
	int firstBlock; 	// location of the first usable block
	int totalFreeSpace; // number of free blocks
};

// Specifications for directory entry
struct DE {
	/* location set to -2 for unused directory entry,
	 * location set to positive non-zero value if in use */
	long location;
	char name[25];
	int size;

	// Metadata
	int dateCreated;
	int dataModified;
	int dateLastAccessed;

	char isDirectory; // '0' for directories '1' for non-directories
};
/*
 * @brief
 * @param
 * @param
 * @return
 */
extern struct VCB * volumeControlBlock;
extern struct DE * root;
extern int * FAT;
int createDirectory ( int numberOfEntries, struct DE * parent, char * name );
#endif
