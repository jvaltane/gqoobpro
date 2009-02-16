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

#include <usb.h>

#include "qoob-defaults.h"
#include "qoob-error.h"

#ifndef _QOOB_H_
#define _QOOB_H_

typedef struct QoobSlot qoob_slot_t;
struct QoobSlot {
  /* Name size is actually 0x99 bytes. Possible added 
     in some situation other text also.
   */
  char name[QOOB_PRO_MAX_BUFFER*5];
  unsigned short int slots_used;    /* How many slots application uses */
  unsigned short int first;         /* Tells is slot applications first slot */

  binary_type_t type;
};

enum {
  QOOB_DATA_READ = 0,
  QOOB_DATA_WRITE,
  QOOB_DATA_ERASE,
  QOOB_DATA_LIST,
  QOOB_DATA_LAST
};

typedef struct Qoob qoob_t;
struct Qoob
{
  struct usb_bus *busses;     /* USB busses */
  struct usb_device *dev;     /* USB device */
  usb_dev_handle *devh;       /* USB device handle */

  int verbose;                /* verbose level*/

  char *real_file;                 /* tmp file to read or write */

  binary_type_t binary_type;  /* binary type to write */

  qoob_slot_t slot[QOOB_PRO_SLOTS];

  /* Callbacks */
  void (*list) (int listed, int total, void *user_data);
  void (*read) (int read, int total, void *user_data);
  void (*write)(int wrote, int total, void *user_data);
  void (*erase)(int erased, int total, void *user_data);
  void *user_data[QOOB_DATA_LAST];
};

qoob_error_t qoob_init (qoob_t *qoob);
void qoob_deinit (qoob_t *qoob);

qoob_error_t qoob_file_format_set (qoob_t *qoob, binary_type_t type);
qoob_error_t qoob_file_format_get (qoob_t *qoob, binary_type_t *type);
qoob_error_t qoob_verbose_set (qoob_t *qoob, int v);

/* FIXME: add typedefs to callbacks */
/* callbacks */
qoob_error_t qoob_set_read_callback (qoob_t *qoob,
                                     void (*cb)(int r, int t, void *user_data),
                                     void *user_data);
qoob_error_t qoob_set_write_callback (qoob_t *qoob,
                                      void (*cb)(int w, int t, void *user_data),
                                      void *user_data);
qoob_error_t qoob_set_erase_callback (qoob_t *qoob,
                                      void (*cb)(int e, int t, void *user_data),
                                      void *user_data);
qoob_error_t qoob_set_write_callback (qoob_t *qoob,
                                      void (*cb)(int r, int t, void *user_data),
                                      void *user_data);

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
