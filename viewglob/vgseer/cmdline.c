/*
	Copyright (C) 2004, 2005 Stephen Bach
	This file is part of the viewglob package.

	viewglob is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	viewglob is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with viewglob; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "config.h"

#include "common.h"
#include "viewglob-error.h"
#include "seer.h"
#include "sequences.h"
#include "actions.h"
#include "cmdline.h"

#include <string.h>

struct overwrite_queue {
	char c;
	bool preserve_cret;
	struct overwrite_queue* next;
}* oq = NULL;

#if DEBUG_ON
extern FILE *df;
#endif

extern struct user_shell u;

/* Initialize working command line and sequence buffer. */
bool cmd_init(void) {

	/* Initialize u.cmd */
	u.cmd.length = 0;
	u.cmd.pos = 0;
	return cmd_alloc();
}

/* Allocate or reallocate space for u.cmd.command. */
bool cmd_alloc(void) {
	static int step = 0;
	step++;

	/* Allocate */
	if (step == 1)
		u.cmd.command = XMALLOC(char, CMD_STEP_SIZE);
	else
		u.cmd.command = XREALLOC(char, u.cmd.command, step * CMD_STEP_SIZE);

	/* Set new memory to \0 */
	memset(u.cmd.command + (step - 1) * CMD_STEP_SIZE, '\0', CMD_STEP_SIZE * sizeof(char));
	return true;
}


/* Start over from scratch (usually after a command has been executed). */
bool cmd_clear(void) {
	memset(u.cmd.command, '\0', u.cmd.length);
	u.cmd.pos = 0;
	u.cmd.length = 0;
	return true;
}


void cmd_enqueue_overwrite(char c, bool preserve_cret) {
	struct overwrite_queue* new_oq;

	DEBUG((df, "enqueuing \'%c\'\n", c));

	new_oq = XMALLOC(struct overwrite_queue, 1);
	new_oq->c = c;
	new_oq->preserve_cret = preserve_cret;
	new_oq->next = oq;
	oq = new_oq;
}


void cmd_dequeue_overwrite(void) {
	if (oq) {
		struct overwrite_queue* tmp = oq->next;
		XFREE(oq);
		oq = tmp;
	}
}


bool cmd_has_queue(void) {
	return oq != NULL;
}


bool cmd_write_queue(void) {
	struct overwrite_queue* tmp;
	bool ok = true;

	while (oq) {
		DEBUG((df, "(queue) "));
		if ( (!ok) || (!cmd_overwrite_char(oq->c, oq->preserve_cret)) )
			ok = false;

		tmp = oq;
		oq = oq->next;
		XFREE(tmp);
	}

	return ok;
}


void cmd_clear_queue(void) {
	struct overwrite_queue* tmp;
	while (oq) {
		tmp = oq;
		oq = oq->next;
		XFREE(tmp);
	}
}

/* Determine whether there is whitespace to the left of the cursor. */
bool cmd_whitespace_to_left(char* holdover) {
	bool result;

	if ( (u.cmd.pos == 0) ||
	     /* Kludge: if the shell buffer has a holdover, which consists
			only of a space, it's reasonable to assume it won't
			complete a sequence and will end up being added to the
			command line. */
	     (holdover && strlen(holdover) == 1 && *(holdover) == ' ') )
		result = true;
	else {
		switch ( *(u.cmd.command + u.cmd.pos - 1) ) {
			case ' ':
			case '\n':
			case '\t':
				result = true;
				break;
			default:
				result = false;
				break;
		}
	}

	return result;
}


/* Determine whether there is whitespace to the right of the cursor
   (i.e. underneath the cursor). */
bool cmd_whitespace_to_right(void) {
	bool result;

	switch ( *(u.cmd.command + u.cmd.pos) ) {
		case ' ':
		case '\n':
		case '\t':
			result = true;
			break;
		default:
			result = false;
			break;
	}

	return result;
}


/* Overwrite the char in the working command line at pos in command;
   realloc if necessary. */
bool cmd_overwrite_char(char c, bool preserve_cret) {

	while ( preserve_cret && *(u.cmd.command + u.cmd.pos) == '\015' ) {
		/* Preserve ^Ms. */
		u.cmd.pos++;
	}

	DEBUG((df, "overwriting \'%c\'\n", c));

	*(u.cmd.command + u.cmd.pos) = c;
	if (u.cmd.pos == u.cmd.length)
		u.cmd.length++;
	u.cmd.pos++;

	if (u.cmd.length % CMD_STEP_SIZE == 0) {
		if (!cmd_alloc()) {
			viewglob_error("Could not allocate space for cmd");
			return false;
		}
	}

	action_queue(A_SEND_CMD);
	return true;
}


/* Remove n chars from the working command line at u.cmd.pos. */
bool cmd_del_chars(int n) {

	if (u.cmd.pos + n > u.cmd.length) {
		/* We've failed to keep up. */
		action_queue(A_SEND_LOST);
		return false;
	}

	memmove(u.cmd.command + u.cmd.pos, u.cmd.command + u.cmd.pos + n, u.cmd.length - (u.cmd.pos + n));
	memset(u.cmd.command + u.cmd.length - n, '\0', n);
	u.cmd.length -= n;

	action_queue(A_SEND_CMD);
	return true;
}


/* Trash everything. */
bool cmd_wipe_in_line(enum direction dir) {
	int cret_pos_l, cret_pos_r;

	switch (dir) {
		case D_RIGHT:	/* Clear to right (in this line) */
			DEBUG((df, "(right)\n"));
			cret_pos_r = find_next_cret(u.cmd.pos);
			if (cret_pos_r == -1) {
				/* Erase everything to the right -- no ^Ms to take into account. */
				memset(u.cmd.command + u.cmd.pos, '\0', u.cmd.length - u.cmd.pos);
				u.cmd.length = u.cmd.pos;
			}
			else {
				/* Erase to the right up to the first ^M. */
				cmd_del_chars(cret_pos_r - u.cmd.pos);

				/* If we were at pos 0, this is the new pos 0; delete the cret. */
				if (!u.cmd.pos)
					cmd_del_chars(1);
			}
			break;
		case D_LEFT:	/* Clear to left -- I've never seen this happen. */
			DEBUG((df, "D_LEFT seen in cmd_wipe_in_line\n"));
			break;
		case D_ALL:	/* Clear all (in this line). */
			DEBUG((df, "(all)\n"));

			/* Find the ^M to the right. */
			cret_pos_r = find_next_cret(u.cmd.pos);
			if (cret_pos_r == -1)
				cret_pos_r = u.cmd.length;
			
			/* Find the ^M to the left. */
			cret_pos_l = find_prev_cret(u.cmd.pos);
			if (cret_pos_l == -1)
				cret_pos_l = 0;

			/* Delete everything in-between. */
			u.cmd.pos = cret_pos_l;
			cmd_del_chars(cret_pos_r - cret_pos_l);

			break;
		default:
			viewglob_warning("Unexpected direction in cmd_wipe_in_line");
			break;
	}
	action_queue(A_SEND_CMD);
	return true;
}


bool cmd_backspace(int n) {
	int i;
	for (i = 0; i < n; i++) {
		if (u.cmd.pos > 0)
			u.cmd.pos--;
		else {
			viewglob_error("Backspaced out of command line");
			action_queue(A_SEND_LOST);
			return false;
		}
	}
	return true;
}


bool cmd_insert_chars(char c, int n) {
	if (n < 0) {
		viewglob_error("<0 in cmd_insert_chars");
		action_queue(A_SEND_LOST);
		return false;
	}

	memmove(u.cmd.command + u.cmd.pos + n, u.cmd.command + u.cmd.pos, u.cmd.length - u.cmd.pos);
	memset(u.cmd.command + u.cmd.pos, c, n);
	u.cmd.length += n;

	action_queue(A_SEND_CMD);
	return true;
}


/* Remove trailing ^Ms.
   These have a tendency to collect after a lot of modifications in a command line.
   They're mostly harmless, and this is treating the symptom rather than the sickness,
   but it seems to work all right. */
void cmd_del_trailing_crets(void) {
	int temp;
	while (u.cmd.command[u.cmd.length - 1] == '\015' && u.cmd.pos < u.cmd.length - 1) {
		temp = u.cmd.pos;
		u.cmd.pos = u.cmd.length - 1;
		cmd_del_chars(1);
		u.cmd.pos = temp;
	}
}
