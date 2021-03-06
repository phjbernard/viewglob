/* 
   common.h -- Process this file with configure to produce common.h
   Copyright (C) 2000 Gary V. Vaughan

   common.h -- Modified for viewglob's purposes.
   Copyright (C) 2004, 2005 Stephen Bach

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
   Much of the following was taken from the book GNU Auotconf, Automake,
   and Libtool by Gary V. Vaughan, Ben Elliston, Tom Tromey, and Ian
   Lance Taylor.  The website is here:
       http://sources.redhat.com/autobook/
*/

#ifndef COMMON_H
#define COMMON_H

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <glib.h>

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#ifndef HAVE_PTRDIFF_T
typedef ptrdiff_t int;
#endif

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS  0
#  define EXIT_FAILURE  1
#endif

#define STREQ(a,b)    (strcmp  ((a), (b)) == 0)
#define STRNEQ(a,b,n) (strncmp ((a), (b), (n)) == 0)

#define PERM_FILE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#endif /* !COMMON_H */

