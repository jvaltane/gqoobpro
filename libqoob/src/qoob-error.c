/* 
 * Copyright (C) 2009-2018 Joni Valtanen <jvaltane@kapsi.fi>
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
    return "No error.";
  case QOOB_ERROR_NOT_FOUND:
    return "Error: Device not found via USB.";
  case QOOB_ERROR_CLAIM_INTERFACE:
    return "Error: Could not claim USB interface.";
  case QOOB_ERROR_ALT_INTERFACE:
    return "Error: Could not set USB alt interface.";
  case QOOB_ERROR_DEVICE_UNKNOWN1:
    return "Error: Unknown device error 1.";
  case QOOB_ERROR_INPUT_NOT_VALID:
    return "Error: Function parameter invalid.";
  case QOOB_ERROR_DEVICE_HANDLE_NOT_VALID:
    return "Error: USB device handle invalid.";
  case QOOB_ERROR_FILE_NOT_VALID:
    return "Error: Name of the file is invalid.";
  case QOOB_ERROR_FILE_STAT:
    return "Error: Stat fails. Probably because missing file.";
  case QOOB_ERROR_SLOT_OUT_OF_RANGE:
    return "Error: Slot is out of range.";
  case QOOB_ERROR_SLOT_RANGE_NOT_VALID:
    return "Error: Given slot range is not valid.";
  case QOOB_ERROR_SLOT_NOT_FIRST:
    return "Error: No application starts from given slot.";
  case QOOB_ERROR_FD_OPEN:
    return "Error: Could not open file.";
  case QOOB_ERROR_FD_READ:
    return "Error: Could not read from the file.";
  case QOOB_ERROR_FD_WRITE:
    return "Error: Could not write to the file.";
  case QOOB_ERROR_FD_SEEK:
    return "Error: Seeking file fails.";
  case QOOB_ERROR_SEND_DATA:
    return "Error: Sending data to device fails.";
  case QOOB_ERROR_TRYING_TO_OVERWRITE:
    return "Error: Trying to overwrite existing data.";
  case QOOB_ERROR_NOT_SUPPORTED_FILE_FORMAT:
    return "Error: File format is not supported.";
  case QOOB_ERROR_TOO_BIG_DATA:
    return "Error: Data is too big. Not enough space at flash.";
  default:
    break;
  }
  return "If you see this report a bug.";
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
