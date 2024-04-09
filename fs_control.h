#ifndef _FSCTL
#define _FSCTL
// Specifications for volume control block
struct VCB
{
	long signature;		   // VCB identifier
	int totalBlocks;	   // blocks in volume
	int blockSize;		   // size of blocks
	int freeSpaceLocation; // location of the FAT table
	int rootLocation;	   // location of root directory
	int rootSize;		// Size of root directory in blocks
	int firstBlock;		   // location of the first usable block
	int totalFreeSpace;	   // number of free blocks
};

// Specifications for directory entry
struct DE
{
	/* location set to -2 for unused directory entry,
	 * location set to positive non-zero value if in use */
	long location;
	int size;

	// Metadata
	int dateCreated;
	int dataModified;
	int dateLastAccessed;

	int isDirectory; // '0' for directories '1' for non-directories
	char name[20];
};
/*
 * @brief
 * @param
 * @param
 * @return
 */
extern struct VCB *volumeControlBlock;
extern struct DE *root;
extern int *fat;
int createDirectory(int numberOfEntries, struct DE *parent, char *name);
#endif
