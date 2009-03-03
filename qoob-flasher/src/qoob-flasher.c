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

#include "qoob-flasher-util.h"

static int flasher_init (qoob_flasher_t *flasher);
static void flasher_deinit (qoob_flasher_t *flasher);

static void print_slots (qoob_slot_t *slots);
static void qoob_callback (qoob_sync_callback_t type,
                           int progress,
                           int total,
                           void *user_data);

int
main (int argc, char **argv)
{
  qoob_flasher_t flasher;
  qoob_error_t ret;

  /* Initialize struct */
  if (flasher_init (&flasher)) {
    fprintf (stderr, "Error: init flasher\n");
    return 1;
  }

  /* Initialize qoob */
  ret = qoob_sync_init (&flasher.qoob);
  if (ret != QOOB_ERROR_OK) {
    fprintf (stderr, "Error: init qoob library\n");
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
  ret = qoob_sync_usb_find (&flasher.qoob);
  if (ret != QOOB_ERROR_OK) {
    goto error;
  }

  qoob_sync_set_callback (&(flasher.qoob), qoob_callback, &flasher);

  /* Every command needs to list of slots */
  ret = qoob_sync_usb_list (&flasher.qoob, &flasher.slots);
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

    if (flasher.verbose > 0) {
      printf ("\nSaving from flash to file %s. Starting at slot [%02d].\n", 
              flasher.file, 
              flasher.slot_num);
    }

    ret = qoob_sync_usb_read (&flasher.qoob, flasher.file, flasher.slot_num);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    if (flasher.verbose > 0) {
      printf ("\nGCB file saved succesfully.\n");
    }
  }
    break;

  /* Writing file to flash */
  case FLASHER_COMMAND_WRITE: {

    if (flasher.verbose > 0) {
      printf ("\nWriting file %s to flash. Starting at slot [%02d].\n", 
              flasher.file,
              flasher.slot_num);
    }

    ret = qoob_sync_usb_write (&flasher.qoob, flasher.file, flasher.slot_num);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    if (flasher.list == QOOB_TRUE) {

      /* modified -> list */
      ret = qoob_sync_usb_list (&flasher.qoob, &flasher.slots);
      if (ret != QOOB_ERROR_OK) {
        goto error;
      }

      /* modified -> print list */
      print_slots (flasher.slots);
    }

    if (flasher.verbose > 0) {
      printf ("\nFile %s flashed succesfully.\n", flasher.file);
    }
  }
    break;

  /* Erasing slots from flash (safe)*/
  case FLASHER_COMMAND_ERASE: {
    if (flasher.verbose > 0) {
      printf ("Erasing flash starting at slot [%02d].\n", flasher.slot_num);
    }

    ret = qoob_sync_usb_erase (&flasher.qoob, flasher.slot_num);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }
    
    if (flasher.list == QOOB_TRUE) {
      ret = qoob_sync_usb_list (&flasher.qoob, &flasher.slots);
      if (ret != QOOB_ERROR_OK) {
        goto error;
      }

      /* modified -> print list */
      print_slots (flasher.slots);
    }

    if (flasher.verbose > 0) {
      printf ("\nErasing finished succesfully.\n");
    }
  }
    break;

  /* Erasing slots from flash (not safe)*/
  case FLASHER_COMMAND_FORCE_ERASE: {
    ret = qoob_sync_usb_erase_forced (&flasher.qoob, 
                                 flasher.erase_from, 
                                 flasher.erase_to);
    if (ret != QOOB_ERROR_OK) {
      goto error;
    }

    /* Update slots */
    ret = qoob_sync_usb_list (&flasher.qoob, &flasher.slots);
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
    printf ("-------------------\n");
    printf (" Qoob Flasher %s\n", VERSION);
    printf ("-------------------\n\n");

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

static int total_slots = -1;
static int slot_count = -1;

static void
qoob_callback (qoob_sync_callback_t type,
               int progress,
               int total,
               void *user_data)
{
  qoob_flasher_t *flasher = (qoob_flasher_t *)user_data;

  if (flasher->verbose < 2) {
    return;
  }

  switch (type) {
  case QOOB_SYNC_CALLBACK_READ_SLOT:
    if (total_slots < 0) {
      total_slots = total-progress+1;
    }
    ++slot_count;
    break;
  case QOOB_SYNC_CALLBACK_READ_CONTENT:
    printf ("\rReading content from slot(s) %02dkb/%02dkb", 
            (progress/1024+1)+(slot_count*64), 
            (total/1024+1)*total_slots);

    if (slot_count == total_slots) {
      printf ("\n");
      total_slots = -1;
    }
    break;
  case QOOB_SYNC_CALLBACK_WRITE_SLOT:
    if (total_slots < 0) {
      total_slots = total-progress+1;
    }
    ++slot_count;
    break;
  case QOOB_SYNC_CALLBACK_WRITE_CONTENT: {
    int real_progress = (progress+1)+(slot_count*64*1024);
    int real_total = (total+1)*total_slots;

    printf ("\rWriting content to slot(s) %02dkb/%02dkb", 
            real_progress/1024,
            real_total/1024);

    if (real_progress == real_total) {
      printf ("\n");
      total_slots = -1;
    }
    break;
  }
  case QOOB_SYNC_CALLBACK_ERASE:
    if (flasher->command != FLASHER_COMMAND_ERASE) {
      break;
    }

    if (total_slots < 0) {
      printf ("\rErasing slots from [%02d] to [%02d]\n", progress, total);
      total_slots = 1;
    }

    if (total_slots > 0) {
      printf ("\rErasing slot [%02d]", progress);
    }

    if (total == progress) {
      printf ("\n");
      total_slots = -1;
    }
    break;
  case QOOB_SYNC_CALLBACK_LIST:
    if (flasher->command != FLASHER_COMMAND_LIST) {
      break;
    }

    printf ("\rCollecting slot information [%02d]/[%02d]", 
            progress, 
            total);

    if (total == progress) {
      printf ("\n");
    }

    break;
  default:
    fprintf (stderr, 
             "ERROR: Unhandled callback.\n"
             "This is bug in qoob-flasher or libqoob\n");
    flasher_deinit ((qoob_flasher_t *)user_data);
    exit (113);
    break;
  }
  fflush (stdout);
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

  flasher->help = QOOB_FALSE;
  flasher->list = QOOB_FALSE;
  flasher->verbose = QOOB_FALSE;
  
  return 0;
}

static void
flasher_deinit (qoob_flasher_t *flasher)
{
  qoob_sync_deinit (&flasher->qoob);

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
