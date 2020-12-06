#ifndef EVER_KEEPER_H
#define EVER_KEEPER_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <err.h>

#ifdef PATH_MAX
long pathmax = PATH_MAX;
#else
long pathmax = 0;
#endif



#define MAX_PATH_HOPE 1024

char* palloc(size_t*);

int isdir(char*);

#endif
