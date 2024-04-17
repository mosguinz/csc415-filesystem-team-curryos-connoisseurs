#ifndef _FSUTILS
#define _FSUTILS
#include "fs_control.h"
#include <stdlib.h>
/*
 * just for quick artithmetic to not have human error
 */
int NMOverM(int n, int m);

/*
 * parse the given path
 *
 * @param pathName the path that is being parsed
 * @param ppinfo the struct that will be populated
 * @return 0 on success -1 on failure
 */
int parsePath(char* pathNmae, struct PPRETDATA *ppinfo);

/*
 * find the index of the child
 *
 * @param searchDirectory the DE that is being searched
 * @param the name of the DE that is being searched
 * @return the index of the DE or -1 if not found
 */
int findInDir(struct DE* searchDirectory, char* name);

#endif
