#ifndef _FSCTL
#define _FSCTL
#include <time.h>

#define DE_NAME_SIZE 28
#define DEFAULTDIRSIZE 56
#define DE_SIZE DEFAULTDIRSIZE * sizeof(struct DE)
#define DECOUNT ((DE_SIZE) / sizeof(struct DE))

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
	int location;
	int size;	// Size in bytes

	// Metadata
	time_t dateCreated;
	time_t dateModified;
	time_t dateLastAccessed;

	int isDirectory; // 1 for directories 0 for non-directories
	char name[DE_NAME_SIZE];
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
