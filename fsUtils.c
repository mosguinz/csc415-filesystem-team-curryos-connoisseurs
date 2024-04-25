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
    int size = NMOverM(searchDirectory[index].size, volumeControlBlock->blockSize);
    int loc = searchDirectory[index].location;
    struct DE* directories = (struct DE*)malloc(7 * 512);
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

void printCurrDir() {
    struct DE* searchDirectory = loadDir(cwd, 0);
    for( int i = 0; i < DECOUNT; i++) {
        if(searchDirectory[i].location >= 0 ) {
            printf("name of directory: %s\n", searchDirectory[i].name);
        }
    }
    free(searchDirectory);
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
        cwd = root;
        strcpy(cwdPathName, "/");
        return 0;
    }
    if( res == -1 || ppinfo->lastElementIndex == -1){
        printf("fail 1\n");
        return -1;
    }
    struct DE* dir = malloc(512 * 7 );
    printPPInfo(ppinfo);
    dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    printDE(dir);
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
/*
 * parse the given path
 *
 * @param pathName the path that is being parsed
 * @param ppinfo the struct that will be populated
 * @return 0 on success -1 on failure
 */
int parsePath(char* pathName, struct PPRETDATA *ppinfo){
    printf("path: %s\n", pathName);
    if(pathName == NULL || ppinfo == NULL) {
        return -1;
    }
    struct DE* currDirectory = malloc(7*512);
    if(pathName[0] == '/'){
        currDirectory = loadDir(root, 0);
    }
    else {
        currDirectory = loadDir(cwd, 0);
    }
    char* savePtr = NULL;
    char* currToken = strtok_r(pathName, "/", &savePtr);
    printf("curr token: %s\n", currToken);
    if( currToken == NULL ) {
        if(pathName[0] == '/') {
            memcpy(ppinfo->parent, currDirectory, 7*512);
            ppinfo->lastElementIndex = -2;
            ppinfo->lastElementName = NULL;
            return 0;
        }
        else {
            return -1;
        }
    }
    struct DE* prevDirectory = malloc(7 * 512);
    memcpy(prevDirectory, currDirectory, 7 * 512);
    int index = findInDir(prevDirectory, currToken);
    if(index != -1) {
        printf("made the correct curr dir\n");
        currDirectory = loadDir(prevDirectory, index);
    }
    char* prevToken = currToken;
    while( (currToken = strtok_r(NULL, "/", &savePtr)) != NULL ) {
        printf("curr token: %s\n", currToken);
        memcpy(prevDirectory, currDirectory, 7 * 512);
        index = findInDir(prevDirectory, currToken);
        if( index == -1 ) {
            printf("did not find the token\n");
            prevToken = currToken;
            currToken = strtok_r(NULL, "/", &savePtr);
            if( currToken == NULL ) {
                printf("hit the correct if statement");
                memcpy(ppinfo->parent, prevDirectory, 7*512);
                ppinfo->lastElementIndex = -1;
                ppinfo->lastElementName = NULL;
                printPPInfo(ppinfo);
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
