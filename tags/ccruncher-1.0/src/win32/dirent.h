/*
    Declaration of POSIX directory browsing functions and types for Win32.

    Kevlin Henney (mailto:kevlin@acm.org), March 1997.

    Copyright Kevlin Henney, 1997. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives, and that no charge may be made for the software and its
    documentation except to cover cost of distribution.
*/

#ifndef DIRENT_H
#define DIRENT_H

#if defined(_MSC_VER) && !defined(BUILD_DIRENT)
#  ifdef _DEBUG
#    pragma comment(lib,"dirent_debug.lib") 
#  else
#    pragma comment(lib,"dirent.lib") 
#  endif
#endif

typedef struct DIR DIR;

struct dirent
{
    char * d_name;
};

#ifdef __cplusplus
extern "C" {
#endif

DIR           * opendir(const char *);
int             closedir(DIR *);
struct dirent * readdir(DIR *);
void            rewinddir(DIR *);

#ifdef __cplusplus
};
#endif

#endif /* DIRENT_H */
