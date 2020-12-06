#ifndef EVER_KEEPER_H
#define EVER_KEEPER_H

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#define PATH_MAX_HOPE 1024

#ifdef PATH_MAX
long pathmax = PATH_MAX;
#else
long pathmax = 0;
#endif

char* palloc(size_t*);

int isdir(char*);

#endif
