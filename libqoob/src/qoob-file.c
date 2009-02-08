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
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "qoob-file.h"

qoob_error_t 
qoob_file_format_parse (qoob_t *qoob)
{
  int fd;
  char buf[4] = {0xff,0xff,0xff,0xff};
  ssize_t read_return;

  if (qoob == NULL)
    return QOOB_ERROR_INPUT_NOT_VALID;
  if (qoob->file == NULL)
    return QOOB_ERROR_FILE_NOT_VALID;

  if ((fd = open (qoob->file, O_RDONLY)) == -1) {
    return QOOB_ERROR_FD_OPEN;
  }

  read_return = read (fd, buf, 4);
  if (read_return < 0) {
    close (fd);
    return QOOB_ERROR_FD_READ;
  }

  /* ELF */
  if (buf[1] == (char)0x45 && 
      buf[2] == (char)0x4c && 
      buf[3] == (char)0x46) {
    qoob->binary_type = QOOB_BINARY_TYPE_ELF;

  /* Qoob binary types */
  /* QoobELF - This can be ELF or DOL */
  } else if (buf[0] == (char)0x45 && 
             buf[1] == (char)0x4c && 
             buf[2] == (char)0x46 &&
             buf[3] == (char) 0x00) {
    qoob->binary_type = QOOB_BINARY_TYPE_GCB;
  /* QoobBin */
  } else if (buf[0] == (char)0x28 &&
             buf[1] == (char)0x43 && 
             buf[2] == (char)0x29 &&
             buf[3] == (char) 0x20) {
    qoob->binary_type = QOOB_BINARY_TYPE_GCB;

  /* Reast are not supported (void) */
  } else {
    qoob->binary_type = QOOB_BINARY_TYPE_VOID;
  }

  close (fd);
  
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
