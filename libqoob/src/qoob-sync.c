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

#include "qoob-sync.h"
#include "qoob-sync-usb.h"

qoob_error_t
qoob_sync_init (qoob_t *qoob)
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

  qoob->real_file = NULL;

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

  qoob->sync_cb = NULL;

  qoob->user_data = NULL;

  qoob->async = FALSE;

  return 0;
}

void 
qoob_sync_deinit (qoob_t *qoob)
{
  int i;
  qoob_sync_usb_clear (qoob);

  if (qoob == NULL)
    return;

  qoob->busses = NULL;
  qoob->dev = NULL;
  qoob->devh = NULL;
  
  for (i=0; i<QOOB_PRO_SLOTS; i++) { 
    qoob->slot[i].first = TRUE;
    qoob->slot[i].slots_used = 0;
    qoob->slot[i].type = QOOB_BINARY_TYPE_VOID;
  }

  qoob->sync_cb = NULL;

  qoob->user_data = NULL;
}

qoob_error_t
qoob_sync_file_format_set (qoob_t *qoob, binary_type_t type)
{
  if (qoob == NULL) {
    return 1;
  }
  qoob->binary_type = type;

  return 0;
}

qoob_error_t 
qoob_sync_file_format_get (qoob_t *qoob, binary_type_t *type)
{
  if (qoob == NULL) {
    return 1;
  }
  *type = qoob->binary_type;

  return 0;
}


/* callback */
qoob_error_t
qoob_sync_set_callback (qoob_t *qoob,
                        void (*cb)(qoob_sync_callback_t type, 
                                   int r, 
                                   int t, 
                                   void *user_data),
                        void *user_data)
{
  if (qoob == NULL) {
    return QOOB_ERROR_INPUT_NOT_VALID;
  }

  if ((cb) == NULL) {
    return QOOB_ERROR_INPUT_NOT_VALID;
  }

  qoob->sync_cb = cb;
  qoob->user_data = user_data;

  return QOOB_ERROR_OK;
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
