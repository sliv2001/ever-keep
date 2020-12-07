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

/** Allocate mem to path */
char* palloc(size_t*);

/** Pretty understandable */
int isdir(char*);

/** Makes dirs of any depth */
int makedir(char*);

/** Makes copy of file source, where initlength
tells which part of path is not copied */
int backup_file(char*, size_t);

/** Makes copy of dir
(dont use this -- use backup_init() */
int backup(char*, size_t, size_t);

/** Makes copy of s to t, where initlength tells
which part of s wont be copied */
int backup_init(char*, char*, size_t);

#endif
