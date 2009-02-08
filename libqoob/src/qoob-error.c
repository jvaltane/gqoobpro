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

#include "qoob-error.h"

const char *
qoob_error_to_string (qoob_error_t e)
{
  switch (e) {
  case QOOB_ERROR_OK:
    return "Everything is OK!!!";
  case QOOB_ERROR_NOT_FOUND:
    return "Device not found via USB!!!";
  case QOOB_ERROR_CLAIM_INTERFACE:
    return "Could not claim usb interface!!!";
  case QOOB_ERROR_ALT_INTERFACE:
    return "Could not set altinterface!!!";
  case QOOB_ERROR_DEVICE_UNKNOWN1:
    return "Unknown device error1!!!";
  case QOOB_ERROR_INPUT_NOT_VALID:
    return "Function parameter  invalid!!!";
  case QOOB_ERROR_DEVICE_HANDLE_NOT_VALID:
    return "USB device handle invalid!!!";
  case QOOB_ERROR_FILE_NOT_VALID:
    return "Name of the file is invalid!!!";
  case QOOB_ERROR_FILE_STAT:
    return "Stat fails. Probably because missing file!!!";
  case QOOB_ERROR_SLOT_OUT_OF_RANGE:
    return "Slot out of range!!!";
  case QOOB_ERROR_SLOT_RANGE_NOT_VALID:
    return "Given slot range is not valid!!!";
  case QOOB_ERROR_SLOT_NOT_FIRST:
    return "Not slot started by any application!!!";
  case QOOB_ERROR_FD_OPEN:
    return"Could not open file descriptor!!!";
  case QOOB_ERROR_FD_READ:
    return "Could not read from the file!!!";
  case QOOB_ERROR_FD_WRITE:
    return "Could not write to the file!!!";
  case QOOB_ERROR_FD_SEEK:
    return "Seek fails!!!";
  case QOOB_ERROR_SEND_DATA:
    return "Sending data to device fails!!!";
  default:
    break;
  }
  return "If you see this report a bug!!!";
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
