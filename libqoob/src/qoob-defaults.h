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

#ifndef _QOOB_DEFAULTS_H_
#define _QOOB_DEFAULTS_H_

/* Defaults listed */

/* Total QoobPro memory is 16Mbits (2048 bytes) */
#define QOOB_PRO_SLOTS 32
#define QOOB_PRO_SLOT_SIZE (QOOB_PRO_MAX_BUFFER*1024) /* 64 kbytes/slot */
#define QOOB_PRO_TOTAL_SIZE (QOOB_PRO_SLOT_SIZE*QOOB_PRO_SLOTS)

#define QOOB_PRO_MAX_BUFFER 0x40 /* How much is written or read at once */

#define QOOB_GCB_HEADER_SIZE 0x100 /* Size of the GCB file 'header' */

#define QOOB_PREFIX_SEPARATOR '.'
#define QOOB_DIRECTORY_SEPARATOR '/'

typedef enum {
  QOOB_BINARY_TYPE_VOID,
  QOOB_BINARY_TYPE_CONFIG,
  QOOB_BINARY_TYPE_BACKGROUND,
  QOOB_BINARY_TYPE_GCB,
  QOOB_BINARY_TYPE_ELF,
  QOOB_BINARY_TYPE_DOL
} binary_type_t;

#define FALSE 0
#define TRUE !FALSE

/* Syncronous callback types */
typedef enum {
  QOOB_SYNC_CALLBACK_READ_SLOT = 0,
  QOOB_SYNC_CALLBACK_READ_CONTENT,
  QOOB_SYNC_CALLBACK_WRITE_SLOT,
  QOOB_SYNC_CALLBACK_WRITE_CONTENT,
  QOOB_SYNC_CALLBACK_ERASE,
  QOOB_SYNC_CALLBACK_LIST,
} qoob_sync_callback_t;

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
