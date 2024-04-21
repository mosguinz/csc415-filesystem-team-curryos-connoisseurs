#ifndef _FSCTL
#define _FSCTL
// Specifications for volume control block
struct VCB
{
	long signature;		   // VCB identifier
	int totalBlocks;	   // blocks in volume
	int blockSize;		   // size of blocks
	int freeSpaceLocation; // location of the FAT table
	int freeSpaceSize;     // size of the FAT table
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

	int isDirectory; // 1 for directories 0 for non-directories
	char name[36];
};

// Specifications for parsepath return data
struct PPRETDATA{
    struct DE* parent;
    int lastElementIndex;
    char* lastElementName;
};
/*
 * @brief
 * @param
 * @param
 * @return
 */
extern struct VCB *volumeControlBlock;
extern struct DE *root;
extern struct DE *cwd;
extern char * cwdPathName;
extern int *fat;
int createDirectory(int numberOfEntries, struct DE *parent);
#endif
