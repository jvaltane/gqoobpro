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
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <qoob.h>
#include <qoob-usb.h>
#include <qoob-error.h>
#include <qoob-file.h>
#include <qoob-defaults.h>

#include "qoob-flasher-util.h"

static void print_slots (qoob_t *qoob);

int
main (int argc, char **argv)
{
  qoob_t qoob;
  qoob_error_t ret;

  /* Initialize struct */
  ret = qoob_init (&qoob);
  if (ret != QOOB_ERROR_OK)
    return 1;

  /* Parse options */
  qoob_flasher_util_parse_options (&qoob, &argc, &argv);
  if (qoob_flasher_util_test_options (&qoob) != 0) {
    qoob_usb_clear (&qoob);
    qoop_flasher_util_print_help_and_exit (1);
  }

  /* Check is device connected to USB */
  ret = qoob_usb_find_qoob (&qoob);
  if (ret != QOOB_ERROR_OK) {
    goto error;
  }

  /* Every command needs to list of slots */
  ret = qoob_usb_get_list (&qoob);
  if (ret != QOOB_ERROR_OK) {
    goto error;
  }


  /* Actual command executing */
  switch (qoob.command) {

  /* List all slots */
  case QOOB_COMMAND_LIST: {
    print_slots (&qoob);
  }
    break;

  /* Reading flashed bios or application to file */
  case QOOB_COMMAND_READ: {
    ret = qoob_usb_read_app_to_file (&qoob);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
  }
    break;

  /* Writing file to flash */
  case QOOB_COMMAND_WRITE: {

    /* Try to get file format type automaticly */
    if (qoob.binary_type == QOOB_BINARY_TYPE_VOID) {
      ret = qoob_file_format_parse (&qoob);
      if (ret != QOOB_ERROR_OK) {
        goto error;
      }
    }

    if (qoob.binary_type == QOOB_BINARY_TYPE_VOID) {
      goto error;
    }

    ret = qoob_usb_write_file_to_flash (&qoob);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    ret = qoob_usb_get_list (&qoob);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    print_slots (&qoob);
  }
    break;

  /* Erasing slots from flash (safe)*/
  case QOOB_COMMAND_ERASE: {

    ret = qoob_usb_erase_app_from_slot (&qoob);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
    
    ret = qoob_usb_get_list (&qoob);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
    print_slots (&qoob);
  }
    break;

  /* Erasing slots from flash (not safe)*/
  case QOOB_COMMAND_FORCE_ERASE: {
    ret = qoob_usb_erase_forced (&qoob, 
                                  qoob.erase_from, 
                                  qoob.erase_to);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    ret = qoob_usb_get_list (&qoob);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
    print_slots (&qoob);
  }
    break;

  default:
    qoob_usb_clear (&qoob);
    qoop_flasher_util_print_help_and_exit (1);
    break;
  }

  qoob_deinit (&qoob);

  return 0;

 error:
  printf ("Error: %s\n", qoob_error_to_string (ret));
  qoob_usb_clear (&qoob);
  return 1;
}

static void
print_slots (qoob_t *qoob)
{
    int i;

    printf ("\n\n");
    printf ("------------------\n");
    printf (" Qoob Flasher %s\n", VERSION);
    printf ("------------------\n\n");

    printf ("----------------------------------------------\n");
    printf (" Slot\tType\tApplication\n");
    printf ("----------------------------------------------\n");
      
    for (i=0; i<QOOB_PRO_SLOTS; i++) {
      printf (" [%02d]\t",i);

      if (qoob->slot[i].type == QOOB_BINARY_TYPE_GCB) {
        printf ("[GCB]\t");
      } else if (qoob->slot[i].type == QOOB_BINARY_TYPE_ELF) {
        printf ("[ELF]\t");
      } else if (qoob->slot[i].type == QOOB_BINARY_TYPE_DOL) {
        printf ("[DOL]\t");
      } else {
        printf ("\t"); 
      }
      printf ("%s\n",qoob->slot[i].name);
      if ( ((i+1)%8) == 0)
        printf ("\n");
    }
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
