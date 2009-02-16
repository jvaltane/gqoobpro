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

  qoob->read = NULL;
  qoob->write = NULL;
  qoob->erase = NULL;
  qoob->list = NULL;

  for (i=0;i<QOOB_DATA_LAST;i++) {
    qoob->user_data[i] = NULL;
  }

  return 0;
}

void 
qoob_deinit (qoob_t *qoob)
{
  int i;
  qoob_usb_clear (qoob);

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

  qoob->read = NULL;
  qoob->write = NULL;
  qoob->erase = NULL;
  qoob->list = NULL;

  for (i=0;i<QOOB_DATA_LAST;i++) {
    qoob->user_data[i] = NULL;
  }
}

qoob_error_t
qoob_file_format_set (qoob_t *qoob, binary_type_t type)
{
  if (qoob == NULL) {
    return 1;
  }
  qoob->binary_type = type;

  return 0;
}

qoob_error_t 
qoob_file_format_get (qoob_t *qoob, binary_type_t *type)
{
  if (qoob == NULL) {
    return 1;
  }
  *type = qoob->binary_type;

  return 0;
}


qoob_error_t 
qoob_verbose_set (qoob_t *qoob, int v)
{
  if (qoob == NULL) {
    return 1;
  }
  qoob->verbose = v;
  
  return 0;
}


/* callback code  */
#define QOOB_SET_CB(q,scb,cb,sdata,data)        \
       do {                                     \
         if ((q) == NULL) {                     \
           return QOOB_ERROR_INPUT_NOT_VALID;   \
         }                                      \
         if ((cb) == NULL) {                    \
           return QOOB_ERROR_INPUT_NOT_VALID;   \
         }                                      \
         (scb)=(cb);                            \
         (sdata)=(data);                        \
       } while (0)

qoob_error_t
qoob_set_read_callback (qoob_t *qoob,
                        void (*cb)(int r, int t, void *user_data),
                        void *user_data)
{
  QOOB_SET_CB(qoob, 
              qoob->read, cb, 
              qoob->user_data[QOOB_DATA_READ], user_data);

  return QOOB_ERROR_OK;
}

qoob_error_t
qoob_set_write_callback (qoob_t *qoob,
                         void (*cb)(int w, int t, void *user_data),
                         void *user_data)
{

  QOOB_SET_CB(qoob, 
              qoob->write, cb, 
              qoob->user_data[QOOB_DATA_WRITE], user_data);


  return QOOB_ERROR_OK;
}

qoob_error_t
qoob_set_erase_callback (qoob_t *qoob,
                         void (*cb)(int e, int t, void *user_data),
                         void *user_data)
{
  QOOB_SET_CB(qoob, 
              qoob->erase, cb, 
              qoob->user_data[QOOB_DATA_ERASE], user_data);

  return QOOB_ERROR_OK;
}

qoob_error_t
qoob_set_list_callback (qoob_t *qoob,
                        void (*cb)(int r, int t, void *user_data),
                        void *user_data)
{
  QOOB_SET_CB(qoob, 
              qoob->list, cb, 
              qoob->user_data[QOOB_DATA_LIST], user_data);
  
  return QOOB_ERROR_OK;
}


#if 0
  void (*list)(int listed, int total, void *user_data);
  void (*read)(int read, int total, void *user_data);
  void (*write)(int wrote, int total, void *user_data);
  void (*erase)(int erased, int total, void *user_data);
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
