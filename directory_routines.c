#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include "mfs.h"
#include "fs_control.h"
#include "freespace.h"
#include "fsUtils.h"

int fs_mkdir (const char *pathname, mode_t mode){
	struct PPRETDATA *parsepathinfo;

	parsepathinfo = malloc(sizeof(struct PPRETDATA));
	parsePath(pathname, parsepathinfo);
	return 0;
}
