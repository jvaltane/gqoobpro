/* 
 * Copyright (C) 2009 Joni Valtanen <jvaltane@kapsi.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>

#include <usb.h>

#include "qoob.h"
#include "qoob-usb.h"

qoob_error_t
qoob_init (qoob_t *qoob)
{
  int i;

  if (qoob == NULL)
    return 1;

  /* initialize libusb */
  usb_init();
#ifdef DEBUG
  usb_set_debug (255);
#endif
  usb_find_busses();
  usb_find_devices();

  qoob->busses = NULL;
  qoob->dev = NULL;
  qoob->devh = NULL;

#ifdef DEBUG
  qoob->verbose = 1;
#else
  qoob->verbose = 0;
#endif

  qoob->command = QOOB_COMMAND_LIST;
  qoob->slotnum = -1;
  qoob->file = NULL;
  qoob->real_file = NULL;

  /* Impossible situatioons */
  qoob->erase_from = 32;
  qoob->erase_to = 32;

  qoob->help=0;

  for (i=0; i<QOOB_PRO_SLOTS; i++) { 
    qoob->slot[i].first = TRUE;
    qoob->slot[i].slots_used = 0;
    qoob->slot[i].type = QOOB_BINARY_TYPE_VOID;
  }

  /* get busses */
  qoob->busses = usb_get_busses();
  if (qoob->busses == NULL) {
    fprintf (stderr, "Could not find any USB busses!!!\n");
    return 2;
  }

  return 0;
}

void 
qoob_deinit (qoob_t *qoob)
{
  qoob_usb_clear (qoob);
  int i;

  if (qoob == NULL)
    return;

  qoob->busses = NULL;
  qoob->dev = NULL;
  qoob->devh = NULL;
  
  if (qoob->file != NULL)
    free (qoob->file);
  qoob->file = NULL;

  for (i=0; i<QOOB_PRO_SLOTS; i++) { 
    qoob->slot[i].first = TRUE;
    qoob->slot[i].slots_used = 0;
    qoob->slot[i].type = QOOB_BINARY_TYPE_VOID;
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
