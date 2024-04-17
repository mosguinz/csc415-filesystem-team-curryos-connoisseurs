#include "fsUtils.h"
#include "freespace.h"
#include "fs_control.h"
#include "mfs.h"
#include <string.h>
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
    int size = searchDirectory->size;
    int loc = searchDirectory->location;
    struct DE** directories = (struct DE**)malloc(size);
    int res = fileRead(directories, size, loc);
    int i = 0;
    int returnVal = -1;
    while( directories[i]->location != -1 ) {
        if( directories[i]->name == name ) {
            returnVal = i;
        }
    }
    free(directories);
    return returnVal;
}

/*
 * load a directory
 *
 * @param searchDirectory the parentDirectory
 * @param index the index of the directory that is being loaded
 * @return the loaded directory (needs to be freed)
 */
struct DE* loadDir(struct DE* searchDirectory, int index) {
    int size = searchDirectory->size;
    int loc = searchDirectory->location;
    struct DE** directories = (struct DE**)malloc(size);
    int res = fileRead(directories, size, loc);
    if( res == -1 ) {
        return NULL;
    }
    struct DE* directory = (struct DE*)malloc( sizeof(struct DE));
    directory = directories[index];
    free(directories);
    return directory;
}

//return 1 if directory, 0 otherwise
int fs_isDir(char * pathname){
    struct PPRETDATA *ppinfo;
    int res = parsePath(pathname, ppinfo);
    struct DE* dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    int returnStatement = dir->isDirectory;
    free(dir);
    if( returnStatement == 1 ) {
        return 0;
    } else if( returnStatement == 0 ){
        return 1;
    }
    return -1;
}

//return 1 if file, 0 otherwise
int fs_isFile(char * filename){
    int index = findInDir(cwd, filename);
    struct DE* dir = loadDir(cwd, index);
    int returnStatement = dir->isDirectory;
    free(dir);
    return returnStatement;
}

char * fs_getcwd(char *pathname, size_t size){
    return cwd->name;
}

int fs_setcwd(char *pathname){
    struct PPRETDATA *ppinfo;
    int res = parsePath(pathname, ppinfo);
    if( res == -1 ){
            return -1;
    }
    struct DE* dir = loadDir(ppinfo->parent, ppinfo->lastElementIndex);
    free(cwd);
    cwd = dir;
    return 0;
}

/*
 * parse the given path
 *
 * @param pathName the path that is being parsed
 * @param ppinfo the struct that will be populated
 * @return 0 on success -1 on failure
 */
int parsePath(char* pathName, struct PPRETDATA *ppinfo){
    struct DE* searchDirectory;
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
