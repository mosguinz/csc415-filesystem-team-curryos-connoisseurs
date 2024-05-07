/**************************************************************
* Class::  CSC-415-0# Spring 2024
* Name:: Arjun Gill, Mos Kullathon, Vignesh Guruswami, Sid Padmanabhuni
* Student IDs:: 922170168
* GitHub-Name:: ArjunS132
* Group-Name:: Curry OS Connoisseurs
* Project:: Basic File System
*
* File:: fsUtils.h
*
* Description:: header file for utility functions
*
**************************************************************/
#ifndef _FSUTILS
#define _FSUTILS
#include "fs_control.h"
#include "b_io.h"
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
int parsePath(const char* pathNmae, struct PPRETDATA *ppinfo);

/*
 * find the index of the child
 *
 * @param searchDirectory the DE that is being searched
 * @param the name of the DE that is being searched
 * @return the index of the DE or -1 if not found
 */
int findInDir(struct DE* searchDirectory, char* name);

/*
 * find the index of an empty DE
 *
 * @param directory the DE that is being searched
 * @return the index of the DE or -1 if not found
 */
int find_vacant_space ( struct DE * directory , char * fileName);

/*
 * method to help with debugging. prints VCB block
 */
void printVCB();
void printFCB(b_fcb fcb);
void printDE(struct DE* directory);
void printCurrDir();
struct DE* loadDir(struct DE* searchDirectory, int index);
#endif
