#include "fsUtils.h"
#include "freespace.h"
#include "fs_control.h"
#include "mfs.h"
#include <string.h>

#define DECOUNT (7 * 512 ) / sizeof(struct DE)

int NMOverM(int n, int m){
    return (n+m-1)/m;
}

/** Get the file name from a path i.e., last token after slash. */
char* extractFileName(const char* path) {
    char* lastSlash = strrchr(path, '/');
    if (strlen(path) == 1 && path[0] == '/') return ".";
    if (lastSlash) return lastSlash+1;
    return path;
}

/*
 * find the index of the child
 *
 * @param searchDirectory the DE that is being searched
 * @param the name of the DE that is being searched
 * @return the index of the DE or -1 if not found
 */
int findInDir(struct DE* searchDirectory, char* name){
    int res = -1;
    for( int i = 0; i < DECOUNT; i++) {
        if( searchDirectory[i].location != -2l && strcmp(searchDirectory[i].name, name ) == 0) {
            res = i;
        }
    }
    return res;
}

/*
 * find the index of an empty DE
 *
 * @param directory the DE that is being searched
 * @return the index of the DE or -1 if not found
 */
int find_vacant_space ( struct DE * directory ){
	for ( int i = 0 ; i < (directory->size)/sizeof(struct DE) ; i++ )
		if ( (directory + i)->location == -2 )
			return i;
	perror("Directory is full");
	return -1;
}

/*
 * load a directory
 *
 * @param searchDirectory the parentDirectory
 * @param index the index of the directory that is being loaded
 * @return the loaded directory (needs to be freed)
 */
struct DE* loadDir(struct DE* searchDirectory, int index) {
    int size = NMOverM(searchDirectory[index].size, volumeControlBlock->blockSize);
    int loc = searchDirectory[index].location;
    struct DE* directories = (struct DE*)malloc(7 * 512);
    int res = fileRead(directories, size, loc);
    if( res == -1 ) {
        free(directories);
        return NULL;
    }
    return directories;
}

//return 1 if directory, 0 otherwise
int fs_isDir(char * pathname){
    struct PPRETDATA *ppinfo = malloc(sizeof(struct PPRETDATA));
    ppinfo->parent = malloc(7 * 512);
    int res = parsePath(pathname, ppinfo);

    if (res == -1 || ppinfo->lastElementIndex < 0) {
        return 0;
    }

    struct DE* dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    int returnStatement = dir->isDirectory;
    free(ppinfo->parent);
    free(ppinfo);
    free(dir);
    return returnStatement;
}

//return 1 if file, 0 otherwise
int fs_isFile(char * filename){
    int index = findInDir(cwd, filename);
    struct DE* dir = loadDir(cwd, index);
    int returnStatement = dir->isDirectory;
    free(dir);
    return returnStatement;
    // TODO: Remove this once DE.isDirectory is flipped.
    // if( returnStatement == 1 ) {
    //     return 0;
    // } else if( returnStatement == 0 ){
    //     return 1;
    // }
    // return -1;
}

void printCurrDir() {
    struct DE* searchDirectory = loadDir(cwd, 0);
    for( int i = 0; i < DECOUNT; i++) {
        if(searchDirectory[i].location != -2l ) {
            printf("name of directory: %s\n", searchDirectory[i].name);
        }
    }
    free(searchDirectory);
}

/*
 * move a file or directory from 1 location to another
 *
 * @param startpathname the path to the file that is being moved
 * @param endpathname the directory to move the file into
 * @return 0 on sucess else -1
 */
int fs_mv(const char* startpathname, const char* endpathname) {
    struct PPRETDATA *startppinfo = malloc( sizeof(struct PPRETDATA));
    startppinfo->parent = malloc( 7 * 512 );
    int startRes = parsePath(startpathname, startppinfo);
    int startIndex = startppinfo->lastElementIndex;
    if( startRes == -1 || startIndex == -1 ) {
        free(startppinfo->parent);
        free(startppinfo);
        return -1;
    }

    struct PPRETDATA *endppinfo = malloc( sizeof(struct PPRETDATA));
    endppinfo->parent = malloc( 7 * 512 );
    int endRes = parsePath(endpathname, endppinfo);
    int endIndex = endppinfo->lastElementIndex;
    if( endRes == -1 || endIndex == -1 || endppinfo->parent[endIndex].isDirectory == 0) {
        free(startppinfo->parent);
        free(startppinfo);
        free(endppinfo->parent);
        free(endppinfo);
        return -1;
    }

    struct DE* endDir = loadDir(endppinfo->parent, endIndex);

    int emptyIndex = find_vacant_space(endDir);
    if(emptyIndex == -1) {
        free(startppinfo->parent);
        free(startppinfo);
        free(endppinfo->parent);
        free(endppinfo);
        free(endDir);
        return -1;
    }

    struct DE* parentDir = startppinfo->parent;
    struct DE* sourceDir = loadDir(startppinfo->parent, startIndex);
    endDir[emptyIndex] = parentDir[startIndex];
    parentDir[startIndex].location = -2l;
    sourceDir[1] = endDir[0];
    strncpy(sourceDir[1].name, "..", 28);

    fileWrite(endDir, NMOverM(endDir->size, MINBLOCKSIZE), endDir->location);
    fileWrite(parentDir, NMOverM(parentDir->size, MINBLOCKSIZE), parentDir->location);
    fileWrite(sourceDir, NMOverM(sourceDir->size, MINBLOCKSIZE), sourceDir->location);

    free(startppinfo->parent);
    free(startppinfo);
    free(endppinfo->parent);
    free(endppinfo);
    free(endDir);
    free(sourceDir);
    return 0;
}

/*
 * gets the path of the current working directory
 *
 * @param pathname the buffer to fill the directory with
 * @param size the size of the buffer
 * @return the path of the current working directory
 */
char * fs_getcwd(char *pathname, size_t size){
    strncpy(pathname, cwdPathName, size);
    return cwdPathName;
}

/*
 * appends paths and cleans them
 *
 */
char* cleanPath(char* pathname) {
    char** pathTable = malloc(sizeof(char*)*strlen(pathname)/2);
    char* savePtr = NULL;
    char* token = strtok_r(pathname, "/", &savePtr);
    int size = 0;
    while( token != NULL ) {
        pathTable[size] = strdup(token);
        token = strtok_r(NULL, "/", &savePtr);
        size++;
    }
    int* indices = malloc(sizeof(int) * size);
    int index = 0;
    int i = 0;
    while( i < size ) {
        char* token = pathTable[i];
        if( strcmp(token, ".") == 0 );
        else if( strcmp(token, "..") == 0 && index > 0) {
            index--;
        }
        else if( strcmp(token, "..") == 0 && index == 0  );
        else {
            indices[index] = i;
            index++;
        }
        i++;
    }
    char* res = malloc( strlen(pathname));
    strcpy(res, "/");
    for( int i = 0; i < index; i++ ) {
        strcat(res, pathTable[indices[i]]);
        strcat(res, "/");
    }
    return res;
}

/*
 * set the current working directory to something else
 *
 * @param pathname the path to the new current working directory
 * @return int
 */
int fs_setcwd(char *pathname){
    struct PPRETDATA *ppinfo = malloc( sizeof(struct PPRETDATA));
    ppinfo->parent = malloc( 7 * 512 );
    int res = parsePath(pathname, ppinfo);
    if( ppinfo->lastElementIndex == -2 ) {
        cwd = loadDir(root, 0);
        strcpy(cwdPathName, "/");
        free(ppinfo->parent);
        free(ppinfo);
        return 0;
    }
    if( res == -1 || ppinfo->lastElementIndex == -1){
        printf("fail 1\n");
        free(ppinfo->parent);
        free(ppinfo);
        return -1;
    }
    struct DE* dir = malloc(512 * 7 );
    dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    if( dir->isDirectory != 1 ) {
        free(dir);
        printf("fail 2\n");
        return -1;
    }
    memcpy(cwd, dir, 7 * 512);
    free(dir);
    if( pathname[0] == '/' ) {
        cwdPathName = strdup(pathname);
    }
    else {
        strcat(cwdPathName, pathname);
    }
    cwdPathName = cleanPath(cwdPathName);
    int size = NMOverM(cwd->size, MINBLOCKSIZE);
    fileWrite(cwd, size, cwd->location);
    printCurrDir();
    return 0;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp){
  int num_blocks = NMOverM(sizeof(struct DE) * DECOUNT, volumeControlBlock->blockSize);
  struct DE* entries = malloc(num_blocks * volumeControlBlock->blockSize);
  int res = fileRead(entries, num_blocks, dirp->dirEntryLocation);
  struct DE entry = entries[dirp->index];

  if (res == -1) {
    perror("Could not load entry\n");
    free(entries);
    return NULL;
  }
  if (dirp->index == DECOUNT-1 || entry.location < 0) {
    free(entries);
    return NULL;
  }

//   printDE(entry);
  dirp->di->d_reclen = dirp->d_reclen;
  dirp->di->fileType = entry.isDirectory;
  strcpy(dirp->di->d_name, entry.name);
  dirp->index++;

  free(entries);
  return dirp->di;
}

int fs_stat(const char *pathname, struct fs_stat *buf) {
    struct PPRETDATA *ppinfo = malloc(sizeof(struct PPRETDATA));
    ppinfo->parent = malloc(7 * volumeControlBlock->blockSize); // TODO: why not malloc in pp?
    int res = parsePath(pathname, ppinfo);
    printf("in fs_stat\n");

    if (res == -1) {
        fprintf(stderr, "no such file or directory: %s\n", pathname);
        return -1;
    }

    char* filename = extractFileName(pathname);
    int index = findInDir(ppinfo->parent, filename);
    if (index == -1) {
        fprintf(stderr, "%s not found\n", filename);
        return -1;
    }

    struct DE entry = ppinfo->parent[index];
    buf->st_size = entry.size;
    buf->st_blksize = volumeControlBlock->blockSize;
    buf->st_blocks = NMOverM(entry.size, volumeControlBlock->blockSize);
    buf->st_accesstime = entry.dateLastAccessed;
    buf->st_modtime = entry.dataModified;
    buf->st_createtime = entry.dateLastAccessed;

    return index;
}

int fs_closedir(fdDir *dirp) {

    if (dirp == NULL) {
        perror("Cannot close directory, is null\n");
        return 0;
    }

    printf("Closing %s\n", dirp->di->d_name);
    free(dirp);
    return 1;
}

fdDir * fs_opendir(const char *pathname) {
    struct PPRETDATA *ppinfo = malloc(sizeof(struct PPRETDATA));
    ppinfo->parent = malloc(7 * volumeControlBlock->blockSize); // TODO: why not malloc in pp?
    int res = parsePath(pathname, ppinfo);

    printf("in opendir\n");

    if (res == -1) {
        fprintf(stderr, "no such file or directory: %s\n", pathname);
        return NULL;
    }

    char* filename = extractFileName(pathname);
    int index = findInDir(ppinfo->parent, filename);

    if (index == -1) {
        fprintf(stderr, "%s not found\n", filename);
        return NULL;
    }

    printf("The file name is %s at index %i\n", filename, index);

    struct DE entry = ppinfo->parent[index];
    // printDE(ppinfo->parent[2]);
    if (!entry.isDirectory) {
        fprintf(stderr, "%s is not a directory\n", pathname);
        return NULL;
    }
    
    fdDir *fd = malloc(sizeof(fdDir));    

    fd->d_reclen = NMOverM(entry.size, volumeControlBlock->blockSize);
    fd->dirEntryPosition = index;
    fd->dirEntryLocation = entry.location;
    fd->index = 0;

    // TODO: why tf was this not typedef?? pick one!!11!
    fd->di = malloc(sizeof(struct fs_diriteminfo));
    fd->di->d_reclen = NMOverM(entry.size, volumeControlBlock->blockSize);
    fd->di->fileType = entry.isDirectory;
    strcpy(fd->di->d_name, filename);
    
    free(ppinfo->parent);
    free(ppinfo);
    return fd;
}



/*
 * method to help with debugging. prints VCB block
 */
void printVCB() {
    printf ("|------------ Volume Control Block -------------|\n");
    printf ("|------- Variable ------|-------- Value --------|\n");
    printf ("| signature             | %-22li|\n", volumeControlBlock->signature);
    printf ("| totalBlocks           | %-22i|\n", volumeControlBlock->totalBlocks);
    printf ("| blockSize             | %-22i|\n", volumeControlBlock->blockSize);
    printf ("| freeSpaceLocation     | %-22i|\n", volumeControlBlock->freeSpaceLocation);
    printf ("| rootLocation          | %-22i|\n", volumeControlBlock->rootLocation);
    printf ("| rootSize              | %-22i|\n", volumeControlBlock->rootSize);
    printf ("| firstFreeBlock        | %-22i|\n", volumeControlBlock->firstBlock);
    printf ("| totalFreeSpace        | %-22i|\n", volumeControlBlock->totalFreeSpace);
    printf ("|-----------------------------------------------|\n");
}

void printDE(struct DE* directory) {
    printf ("|--------------- Directory Entry ---------------|\n");
    printf ("|------- Variable ------|-------- Value --------|\n");
    printf ("| name             | %-26s|\n", directory->name);
    printf ("| size             | %-26i|\n", directory->size);
    printf ("| location         | %-26li|\n", directory->location);
    printf ("| is directory     | %-26i|\n", directory->isDirectory);
    printf ("| date created     | %-26i|\n", directory->dateCreated);
    printf ("| date modified    | %-26i|\n", directory->dataModified);
    printf ("|-----------------------------------------------|\n");
}

void printPPInfo(struct PPRETDATA * res) {
    printf ("|--------------- PPRETDATA Entry ---------------|\n");
    printf ("|------- Variable ------|-------- Value --------|\n");
    printf ("| last ele name         | %-27s|\n", res->lastElementName);
    printf ("| last ele Index        | %-27i|\n", res->lastElementIndex);
    printf ("|-----------------------------------------------|\n");
    printDE(res->parent);
}

void printFCB(b_fcb fcb){
    printf ("|--------------------- FCB ---------------------|\n");
    printf ("|------- Variable ------|-------- Value --------|\n");
    printf ("| index                 | %-22i|\n", fcb.index);
    printf ("| buflen                | %-22i|\n", fcb.buflen);
    printf ("| numBlocks             | %-22i|\n", fcb.numBlocks);
    printf ("| currentBlock          | %-22i|\n", fcb.currentBlock);
    printf ("| remainingBytes        | %-22i|\n", fcb.remainingBytes);
    printf ("|-----------------------------------------------|\n");
    // printf("the buffer:\n%s\n", fcb.buf);
    // printDE(fcb.fileInfo);
}

//removes a file
int fs_delete(char* filename){
    struct PPRETDATA *ppinfo = malloc( sizeof(struct PPRETDATA));
    ppinfo->parent = malloc( 7 * 512 );
    int res = parsePath(filename, ppinfo);
    int index = ppinfo->lastElementIndex;
    if( res == -1 || index == -1 ) {
        free(ppinfo->parent);
        free(ppinfo);
        return -1;
    }
    if( cwd[index].size > 0 ) {
        if( returnFreeBlocks(cwd[index].location) == -1) {
            free(ppinfo->parent);
            free(ppinfo);
            return -1;
        }
    }
    cwd[index].location = -2l;
    int dirSize = NMOverM(cwd[0].size, MINBLOCKSIZE);
    fileWrite(cwd, dirSize, cwd[0].location);
    free(ppinfo->parent);
    free(ppinfo);
    return 0;
}

void clearDir(struct DE* dir) {
    int location = dir->location;
    for(int i=2; i < DECOUNT; i++) {
        if(dir[i].location > 0 && dir[i].isDirectory == 1) {
            struct DE* currDir = loadDir(dir, i);
            clearDir(currDir);
            free(currDir);
        }
        if( dir[i].location > 0 && dir[i].isDirectory == 0) {
            returnFreeBlocks(dir[i].location);
            dir[i].location = (long)-2;
        }
    }
    returnFreeBlocks(location);
    int size = NMOverM(dir->size, MINBLOCKSIZE);
    fileWrite(dir, size, location);
}

int isEmpty(struct DE* dir) {
    for( int i = 2; i < DECOUNT; i++ ) {
        if(dir[i].location > 0) {
            return 0;
        }
    }
    return 1;
}

int fs_rmdir(const char *pathname) {
    struct PPRETDATA *ppinfo = malloc( sizeof(struct PPRETDATA));
    ppinfo->parent = malloc( 7 * 512 );
    int res = parsePath(pathname, ppinfo);
    int index = ppinfo->lastElementIndex;
    if( res == -1 || index <= -1 ) {
        return -1;
    }
    struct DE* currDir = loadDir(ppinfo->parent, index);;
    // clearDir(currDir);
    if(isEmpty(currDir) == 0) {
        free(currDir);
        free(ppinfo->parent);
        free(ppinfo);
        return -1;
    }
    ppinfo->parent[index].location = (long)-2;
    int size = NMOverM(ppinfo->parent->size, volumeControlBlock->blockSize);
    int location = ppinfo->parent->location;
    fileWrite(ppinfo->parent, size, location);
    free(currDir);
    free(ppinfo->parent);
    free(ppinfo);
    return 0;
}

/*
 * parse the given path
 *
 * @param pathName the path that is being parsed
 * @param ppinfo the struct that will be populated
 * @return 0 on success -1 on failure
 */
int parsePath(const char* pathName, struct PPRETDATA *ppinfo){
    printf("path: %s\n", pathName);
    if(pathName == NULL || ppinfo == NULL) {
        return -1;
    }
    struct DE* currDirectory;
    if(pathName[0] == '/'){
        currDirectory = loadDir(root, 0);
    }
    else {
        currDirectory = loadDir(cwd, 0);
    }
    char* savePtr = NULL;
    char* path = strdup(pathName);
    char* currToken = strtok_r(path, "/", &savePtr);
    printf("curr token: %s\n", currToken);
    if( currToken == NULL ) {
        if(pathName[0] == '/') {
            memcpy(ppinfo->parent, currDirectory, 7*512);
            ppinfo->lastElementIndex = -2;
            ppinfo->lastElementName = NULL;
            free(path);
            free(currDirectory);
            return 0;
        }
        else {
            free(path);
            free(currDirectory);
            return -1;
        }
    }
    struct DE* prevDirectory = malloc(7 * 512);
    memcpy(prevDirectory, currDirectory, 7 * 512);
    int index = findInDir(prevDirectory, currToken);
    if(index != -1) {
        currDirectory = loadDir(prevDirectory, index);
    }
    char* prevToken = currToken;
    while( (currToken = strtok_r(NULL, "/", &savePtr)) != NULL ) {
        printf("curr token: %s\n", currToken);
        memcpy(prevDirectory, currDirectory, 7 * 512);
        index = findInDir(prevDirectory, currToken);
        if( index == -1 ) {
            prevToken = currToken;
            currToken = strtok_r(NULL, "/", &savePtr);
            if( currToken == NULL ) {
                memcpy(ppinfo->parent, prevDirectory, 7*512);
                ppinfo->lastElementIndex = -1;
                ppinfo->lastElementName = prevToken;
                return 0;
            }
            else {
                return -1;
            }
        }
        else {
            currDirectory = loadDir(prevDirectory, index);
        }
    }
    memcpy(ppinfo->parent, prevDirectory, 7*512);
    ppinfo->lastElementName = prevToken;
    ppinfo->lastElementIndex = index;
    if( currDirectory != cwd && currDirectory != root ) {
        free(currDirectory);
    }
    return 0;
}
