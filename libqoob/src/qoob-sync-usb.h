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

#include "qoob-struct.h"
#include "qoob-error.h"

#ifndef _QOOB_SYNC_USB_H_
#define _QOOB_SYNC_USB_H_

#define QOOB_PRO_VENDOR 0x03eb
#define QOOB_PRO_PRODUCT 0x0001


#define QOOB_USB_CMD_ZERO "\x00"

/* Commands to talk with device */
/* No need for receive answer after these */
#define QOOB_USB_CMD_CONTROL "\x08"
#define QOOB_USB_CMD_CONTROL_START "\x01"
#define QOOB_USB_CMD_CONTROL_END "\x00"

/* Receive answer */
#define QOOB_USB_CMD_GET_ANSWER "\x05"

/* Reading slot info and data*/
#define QOOB_USB_CMD_READ_SLOT "\x04"
#define QOOB_USB_CMD_READ_SLOT_INFO "\x01"
#define QOOB_USB_CMD_READ_SLOT_ALL "\x80"
#define QOOB_USB_CMD_READ_SLOT_ALL_HALF_WAY "\x80"

/* Writing slot */
#define QOOB_USB_CMD_WRITE_SLOT "\x03"
#define QOOB_USB_CMD_WRITE_SLOT_ALL "\x80"

#define QOOB_USB_CMD_ERASE "\x02"

qoob_error_t qoob_sync_usb_find (qoob_t *qoob);
qoob_error_t qoob_sync_usb_read (qoob_t *qoob,
                                 char *file,
                                 short int slotnum);
qoob_error_t qoob_sync_usb_write (qoob_t *qoob,
                                  char *file,
                                  short int slotnum);
qoob_error_t qoob_sync_usb_erase (qoob_t *qoob, 
                                  short int slot_num);
qoob_error_t qoob_sync_usb_erase_forced (qoob_t *qoob, 
                                         short int slot_from, 
                                         short int slot_to);
qoob_error_t qoob_sync_usb_list (qoob_t *qoob,
                                 qoob_slot_t **slots);

void qoob_sync_usb_clear (qoob_t *qoob);

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
