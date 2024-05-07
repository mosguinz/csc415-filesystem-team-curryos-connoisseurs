/**************************************************************
* Class::  CSC-415-03 Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
* Project:: Basic File System
*
* File:: fs_control.h
*
* Description:: header file for basic file system controls
*
**************************************************************/
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
	int size;	// Size in bytes

	// Metadata
	time_t dateCreated;
	time_t dateModified;
	time_t dateLastAccessed;

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
