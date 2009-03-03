/*
 * Copyright (C) 2009 Joni Valtanen <jvaltane@kapsi.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#include "qoob.h"

#ifndef _QOOB_FLASHER_UTIL_H_
#define _QOOB_FLASHER_UTIL_H_

typedef struct QoobFlasher qoob_flasher_t;
typedef enum {
  FLASHER_COMMAND_LIST,
  FLASHER_COMMAND_READ,
  FLASHER_COMMAND_WRITE,
  FLASHER_COMMAND_ERASE,
  FLASHER_COMMAND_FORCE_ERASE
} flasher_command_t;

struct QoobFlasher
{
  qoob_t qoob;
  qoob_slot_t *slots; //[QOOB_PRO_SLOTS];

  char *file;
  short int slot_num;
  short int erase_from;
  short int erase_to;

  flasher_command_t command;

  qoob_boolean_t help;
  qoob_boolean_t list;

  unsigned int verbose;
};

void qoob_flasher_util_parse_options (qoob_flasher_t *flasher, 
                                      int *argc, 
                                      char ***argv);
int qoob_flasher_util_test_options (qoob_flasher_t *flasher);
void qoop_flasher_util_print_help_and_exit (int e);

#endif

/* Emacs indentatation information
   Local Variables:
   indent-tabs-mode:nil
   tab-width:2
   c-set-offset:2
   c-basic-offset:2
   End:
*/
// vim: filetype=c:expandtab:shiftwidth=2:tabstop=2:softtabstop=2
