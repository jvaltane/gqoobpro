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

#include "qoob-struct.h"
#include "qoob-error.h"

#ifndef _QOOB_SYNC_H_
#define _QOOB_SYNC_H_

qoob_error_t qoob_sync_init (qoob_t *qoob);
void qoob_sync_deinit (qoob_t *qoob);

qoob_error_t qoob_sync_file_format_set (qoob_t *qoob, binary_type_t type);
qoob_error_t qoob_sync_file_format_get (qoob_t *qoob, binary_type_t *type);

qoob_slot_t *qoob_sync_slot_copy (qoob_slot_t *slot);
void qoob_sync_slot_free (qoob_slot_t *slot);

/* callbacks */
qoob_error_t qoob_sync_set_callback (qoob_t *qoob,
                                     void (*cb)(qoob_sync_callback_t type, 
                                                int r, 
                                                int t, 
                                                void *user_data),
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
