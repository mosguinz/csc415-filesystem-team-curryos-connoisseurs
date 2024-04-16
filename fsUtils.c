#include "fsUtils.h"
#include "freespace.h"
#include "fs_control.h"
#include <string.h>
int NMOverM(int n, int m){
    return (n+m-1)/m;
}

/*
 * find the index of the parent
 */
int findInDir(struct DE* searchDirectory, char* name){
    int size = searchDirectory->size;
    int loc = searchDirectory->location;
    struct DE** directories = (struct DE**)malloc( size * volumeControlBlock->blockSize);
    int res = fileRead(directories, size, loc);
    int i = 0;
    int returnVal = -1;
    while( directories[i]->location != -1 ) {
        if( directories[i]->name == name ) {
            returnVal = i;
        }
    }
    return returnVal;
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
    char* token = strtok_r(pathName, "/", &savePtr);
    if( token == NULL ) {
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
    int index = findInDir(searchDirectory, token);
    return 0;
}
