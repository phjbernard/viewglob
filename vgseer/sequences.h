/*
	Copyright (C) 2004, 2005 Stephen Bach
	This file is part of the Viewglob package.

	Viewglob is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Viewglob is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Viewglob; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SEQUENCES_H
#define SEQUENCES_H

#include "common.h"
#include "shell.h"
#include "cmdline.h"
#include "connection.h"

G_BEGIN_DECLS

#define IN_PROGRESS(x) ( (!(x & MS_MATCH)) && (x & MS_IN_PROGRESS))

/* Return values of match-success functions. */
typedef enum _MatchEffect MatchEffect;
enum _MatchEffect {
	ME_ERROR,
	ME_NO_EFFECT,
	ME_CMD_EXECUTED,
	ME_CMD_STARTED,
	ME_CMD_REBUILD,
	ME_PWD_CHANGED,
	ME_RPROMPT_STARTED,
};

/* Sequence functions. */
void  init_seqs(enum shell_type shell);
void  check_seqs(Connection* b, struct cmdline* cmd);
void  enable_all_seqs(enum process_level pl);
void  disable_all_seqs(enum process_level pl);
void  clear_seqs(enum process_level pl);

G_END_DECLS

#endif	/* !SEQUENCES_H */

