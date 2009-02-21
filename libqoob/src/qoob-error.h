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

#ifndef _QOOB_ERROR_H_
#define _QOOB_ERROR_H_

typedef enum {
  QOOB_ERROR_OK = 0,
  QOOB_ERROR_NOT_FOUND,
  QOOB_ERROR_CLAIM_INTERFACE,
  QOOB_ERROR_ALT_INTERFACE,
  QOOB_ERROR_DEVICE_UNKNOWN1,
  QOOB_ERROR_INPUT_NOT_VALID,
  QOOB_ERROR_DEVICE_HANDLE_NOT_VALID,
  QOOB_ERROR_FILE_NOT_VALID,
  QOOB_ERROR_FILE_STAT,
  QOOB_ERROR_SLOT_OUT_OF_RANGE,
  QOOB_ERROR_SLOT_RANGE_NOT_VALID,
  QOOB_ERROR_SLOT_NOT_FIRST,
  QOOB_ERROR_FD_OPEN,
  QOOB_ERROR_FD_READ,
  QOOB_ERROR_FD_WRITE,
  QOOB_ERROR_FD_SEEK,
  QOOB_ERROR_SEND_DATA,
  QOOB_ERROR_TRYING_TO_OVERWRITE
} qoob_error_t;

const char *qoob_error_to_string (qoob_error_t e);

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
