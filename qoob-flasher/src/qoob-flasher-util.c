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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <qoob.h>
#include <qoob-usb.h>

#include "qoob-flasher-util.h"

void 
qoob_flasher_util_parse_options (qoob_t *qoob, int *argc, char ***argv)
{
  while (1) {
    static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"verbose",no_argument, 0, 'v'},
      {"elf", no_argument, 0, 'l'},
      {"dol", no_argument, 0, 'd'},
      {"gcb", no_argument, 0, 'g'},
      {"write",required_argument, 0, 'w'},
      {"read", required_argument, 0, 'r'},
      {"force-erase", required_argument, 0, 'f'},
      {"erase", required_argument, 0, 'e'},
      {0, 0, 0, 0}
    };

    int index = 0;
     
    char c = getopt_long (*argc, *argv, "hvldgw:r:f:e:", long_options, &index);
     
    if (c == -1)
      break;

    switch (c) {
    case 0:
      break;
    case 'h':
      qoob->help = 1;
      break;
    case 'v':
      qoob->verbose = 1;
      break;
    case 'l':
      qoob->binary_type = QOOB_BINARY_TYPE_ELF;
      break;
    case 'd':
      qoob->binary_type = QOOB_BINARY_TYPE_DOL;
      break;
    case 'g':
      qoob->binary_type = QOOB_BINARY_TYPE_GCB;
      break;
    case 'w':
      qoob->command = QOOB_COMMAND_WRITE;
      qoob->slotnum = (int)strtol (optarg, NULL, 10);
      break;
    case 'r':
      qoob->command = QOOB_COMMAND_READ;
      qoob->slotnum = (int)strtol (optarg, NULL, 10);
      break;
    case 'e':
      qoob->command = QOOB_COMMAND_ERASE;
      qoob->slotnum = (int)strtol (optarg, NULL, 10);
      break;
    case 'f':
      qoob->command = QOOB_COMMAND_FORCE_ERASE;
      qoob->slotnum = (int)strtol (optarg, NULL, 10);
      break;
    case '?':
      break;
    default:
      return;
      break;
    }
  }

  if (optind < (*argc)) {
    qoob->file =  strdup ((char *)(*argv)[optind]);
  }
}

int 
qoob_flasher_util_test_options (qoob_t *qoob)
{
  if (qoob->help == 1) {
    qoob_usb_clear (qoob);
    qoop_flasher_util_print_help_and_exit (0);
  }

  if (qoob->command == QOOB_COMMAND_READ ||
      qoob->command == QOOB_COMMAND_WRITE) {
    if (qoob->slotnum >= QOOB_PRO_SLOTS || 
	qoob->slotnum < 0 || 
	qoob->file == NULL) {
      return 1;
    }
  }

  if (qoob->command == QOOB_COMMAND_ERASE ||
      qoob->command == QOOB_COMMAND_FORCE_ERASE) {
    if (qoob->slotnum >= QOOB_PRO_SLOTS || 
	qoob->slotnum < 0) {
      return 1;
    }
  }
 
  return 0;
}

void
qoop_flasher_util_print_help (void) 
{
  printf ("Usage: qoob-flasher [OPTION]... [FILE]\n");
  printf ("Util to work with Qoop Pro Gamecube mod-chip. Only GCB is supported fileformat to flash.\n\n");

  printf ("  -h, --help               display this help and exits\n");
  printf ("  -v, --verbose            gives more information what happens\n");
  printf ("  -l, --list               lists slots in flash\n");
  printf ("  -w, --write=SLOT         writes given file to flash\n");
  printf ("  -r, --read=SLOT          reads gcb file from flash to given file\n");
  printf ("  -e, --erase=SLOT         erase appilacion in flash\n");
  printf ("  -f, --force-erase=SLOT   erase one slot\n");
  printf ("\n");


  printf ("Examples:\n\n");

  printf (" Show what is flashed.\n");
  printf ("  qoob-flasher\n\n");

  printf (" Read qoob-bios from flash\n");
  printf ("  qoob-flasher -r0 /tmp/qoob-bios.gcb\n\n");

  printf (" Erase qoob-bios from flash\n");
  printf ("  qoob-flasher -e0\n\n");

  printf (" Write qoob-bios to flash\n");
  printf ("  qoob-flasher -w0 /tmp/qoob-bios.gcb\n\n");

  printf (" See man page for more detailed descriptions\n\n");  
}

void
qoop_flasher_util_print_help_and_exit (int e)
{
  qoop_flasher_util_print_help ();
  exit (e);
}

/* Emacs indentatation information
   Local Variables:
   indent-tabs-mode:nil
   tab-width:2
   c-set-offset:2
   c-basic-offset:2
   End:
*/
// vim: filetype=c:expandtab:shiftwidth=2:tabstop=2:softtabstop=2
