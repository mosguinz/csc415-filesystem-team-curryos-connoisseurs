#include "fsUtils.h"
#include "freespace.h"
#include "fs_control.h"
#include "mfs.h"
#include <string.h>

#define DECOUNT (7 * 512 ) / sizeof(struct DE)

int NMOverM(int n, int m){
    return (n+m-1)/m;
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
        if( strcmp(searchDirectory[i].name, name ) == 0) {
            res = i;
        }
    }
    return res;
}

/*
 * load a directory
 *
 * @param searchDirectory the parentDirectory
 * @param index the index of the directory that is being loaded
 * @return the loaded directory (needs to be freed)
 */
struct DE* loadDir(struct DE* searchDirectory, int index) {
    int size = searchDirectory[0].size;
    int loc = searchDirectory[0].location;
    struct DE* directories = (struct DE*)malloc(size);
    int res = fileRead(directories, size, loc);
    if( res == -1 ) {
        return NULL;
    }
    return directories;
}

//return 1 if directory, 0 otherwise
int fs_isDir(char * pathname){
    struct PPRETDATA *ppinfo;
    int res = parsePath(pathname, ppinfo);
    struct DE* dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    int returnStatement = dir->isDirectory;
    free(dir);
    return returnStatement;
}

//return 1 if file, 0 otherwise
int fs_isFile(char * filename){
    int index = findInDir(cwd, filename);
    struct DE* dir = loadDir(cwd, index);
    int returnStatement = dir->isDirectory;
    free(dir);
    if( returnStatement == 1 ) {
        return 0;
    } else if( returnStatement == 0 ){
        return 1;
    }
    return -1;
}

/*
 * gets the path of the current working directory
 *
 * @param pathname the buffer to fill the directory with
 * @param size the size of the buffer
 * @return the path of the current working directory
 */
char * fs_getcwd(char *pathname, size_t size){
    return cwd->name;
}

/*
 * set the current working directory to something else
 *
 * @param pathname the path to the new current working directory
 * @return int
 */
int fs_setcwd(char *pathname){
    struct PPRETDATA *ppinfo = malloc( sizeof(struct PPRETDATA));
    int res = parsePath(pathname, ppinfo);
    if( res == -1 || ppinfo->lastElementIndex == -1){
            return -1;
    }
    struct DE* dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    free(cwd);
    cwd = dir;
    return 0;
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
    printf ("| name             | %-22s|\n", directory->name);
    printf ("| size             | %-22i|\n", directory->size);
    printf ("| location         | %-22li|\n", directory->location);
    printf ("| is directory     | %-22i|\n", directory->isDirectory);
    printf ("| date created     | %-22i|\n", directory->dateCreated);
    printf ("| date modified    | %-22i|\n", directory->dataModified);
    printf ("|-----------------------------------------------|\n");
}
/*
 * parse the given path
 *
 * @param pathName the path that is being parsed
 * @param ppinfo the struct that will be populated
 * @return 0 on success -1 on failure
 */
int parsePath(char* pathName, struct PPRETDATA *ppinfo){
    struct DE* searchDirectory = malloc(7*512);
    if(pathName == NULL || ppinfo == NULL) {
        return -1;
    }
    if(pathName[0] == '/'){
        searchDirectory = root;
    }
    else {
        searchDirectory = cwd;
    }
    char* savePtr = NULL;
    char* nextToken = strtok_r(pathName, "/", &savePtr);
    if( nextToken == NULL ) {
        if(pathName[0] == '/') {
            ppinfo->parent = searchDirectory;
            ppinfo->lastElementIndex = -2;
            ppinfo->lastElementName = NULL;
            return 0;
        }
        else {
            return -1;
        }
    }
    char* currToken;
    int index;
    do {
        currToken = nextToken;
        index = findInDir(searchDirectory, nextToken);
        if( index == -1 ) {
            if( searchDirectory != cwd && searchDirectory != root ) {
                free(searchDirectory);
            }
            return -1;
        }
        struct DE* tempDir = searchDirectory;
        searchDirectory = loadDir(tempDir, index);
        if( tempDir != cwd && tempDir != root ) {
            free(tempDir);
        }
        char* nextToken = strtok_r(NULL, "/", &savePtr);
    } while (nextToken != NULL);
    ppinfo->lastElementName = currToken;
    ppinfo->lastElementIndex = index;
    ppinfo->parent = searchDirectory;
    if( searchDirectory != cwd && searchDirectory != root ) {
        free(searchDirectory);
    }
    return 0;
}
