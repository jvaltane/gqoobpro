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

static int flasher_init (qoob_flasher_t *flasher);
static void flasher_deinit (qoob_flasher_t *flasher);

static void print_slots (qoob_slot_t *slots);

int
main (int argc, char **argv)
{
  qoob_flasher_t flasher;
  qoob_error_t ret;

  /* Initialize struct */
  if (flasher_init (&flasher)) {
    return 1;
  }

  /* Initialize qoob */
  ret = qoob_init (&flasher.qoob);
  if (ret != QOOB_ERROR_OK) {
    return 1;
  }

  /* Parse options */
  qoob_flasher_util_parse_options (&flasher, &argc, &argv);

  /* Test is user input valid */
  if (qoob_flasher_util_test_options (&flasher) != 0) {
    flasher_deinit (&flasher);
    qoop_flasher_util_print_help_and_exit (1);
  }

  /* Check is device connected to USB */
  ret = qoob_usb_find (&flasher.qoob);
  if (ret != QOOB_ERROR_OK) {
    goto error;
  }

  /* Every command needs to list of slots */
  ret = qoob_usb_list (&flasher.qoob, &flasher.slots);
  if (ret != QOOB_ERROR_OK) {
    goto error;
  }

  /* Actual command executing */
  switch (flasher.command) {

  /* List all slots */
  case FLASHER_COMMAND_LIST: {
    print_slots (flasher.slots);
  }
    break;

  /* Reading flashed bios or application to file */
  case FLASHER_COMMAND_READ: {
    ret = qoob_usb_read (&flasher.qoob, flasher.file, flasher.slot_num);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
  }
    break;

  /* Writing file to flash */
  case FLASHER_COMMAND_WRITE: {

    ret = qoob_usb_write (&flasher.qoob, flasher.file, flasher.slot_num);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    /* modified -> list */
    ret = qoob_usb_list (&flasher.qoob, &flasher.slots);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    /* modified -> print list */
    print_slots (flasher.slots);
  }
    break;

  /* Erasing slots from flash (safe)*/
  case FLASHER_COMMAND_ERASE: {

    ret = qoob_usb_erase (&flasher.qoob, flasher.slot_num);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
    
    ret = qoob_usb_list (&flasher.qoob, &flasher.slots);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    /* modified -> print list */
    print_slots (flasher.slots);
  }
    break;

  /* Erasing slots from flash (not safe)*/
  case FLASHER_COMMAND_FORCE_ERASE: {
    ret = qoob_usb_erase_forced (&flasher.qoob, 
                                 flasher.erase_from, 
                                 flasher.erase_to);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    /* Update slots */
    ret = qoob_usb_list (&flasher.qoob, &flasher.slots);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    /* modified -> print list */
    print_slots (flasher.slots);
  }
    break;

  default:
    flasher_deinit (&flasher);
    qoop_flasher_util_print_help_and_exit (1);
    break;
  }

  flasher_deinit (&flasher);

  return 0;

 error:
  printf ("Error: %s\n", qoob_error_to_string (ret));
  flasher_deinit (&flasher);
  return 1;
}

static void
print_slots (qoob_slot_t *slots)
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

      if (slots[i].type == QOOB_BINARY_TYPE_GCB) {
        printf ("[GCB]\t");
      } else if (slots[i].type == QOOB_BINARY_TYPE_ELF) {
        printf ("[ELF]\t");
      } else if (slots[i].type == QOOB_BINARY_TYPE_DOL) {
        printf ("[DOL]\t");
      } else if (slots[i].type == QOOB_BINARY_TYPE_CONFIG) {
        printf ("[CFG]\t");
      } else {
        printf ("\t");
      }
      printf ("%s\n",slots[i].name);

      if ( ((i+1)%8) == 0)
        printf ("\n");
    }
}

static int
flasher_init (qoob_flasher_t *flasher)
{
  if (flasher == NULL) {
    return 1;
  }

  flasher->command = FLASHER_COMMAND_LIST;
  flasher->slot_num = -1;
  flasher->file = NULL;
  flasher->slots = NULL;

  /* Impossible situatioons */
  flasher->erase_from = 32;
  flasher->erase_to = 32;

  flasher->help = 0;
}

static void
flasher_deinit (qoob_flasher_t *flasher)
{
  qoob_deinit (&flasher->qoob);

  if (flasher->file != NULL) {
    free (flasher->file);
  }
  flasher->file = NULL;

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
