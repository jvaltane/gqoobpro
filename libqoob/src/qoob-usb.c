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
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <usb.h>

#include "qoob.h"
#include "qoob-defaults.h"
#include "qoob-error.h"
#include "qoob-usb.h"

#define EMPTY_SLOT_NAME "    Empty"
#define CONFIG_SLOT_NAME "    Config"
#define CONTINUING_TEXT " [%02d]"
#define SLOTS_IN_USE_INDEX 2

#define DEFAULT_TIMEOUT 1000
#define SEND_REQUEST 0x9
#define SEND_VALUE 0x200
#define RECV_REQUEST 0x1
#define RECV_VALUE 0x300

#define QOOB_READ_LOOP_DEFAULT (1024+16)
#define QOOB_READ_LOOP_MISSING_BYTES 16
#define QOOB_READ_LOOP_HALF_WAY 520

#define QOOB_WRITE_LOOP_DEFAULT (1024+16+2)
#define QOOB_WRITE_LOOP_MISSING_BYTES 16
#define QOOB_WRITE_LOOP_HALF_WAY 522

#define QOOB_DEFAULT_SEEK 0x8000

#define QOOB_START_OK 0x01

#define QOOB_START(x,y)                            \
  do {                                              \
    send_command ((x),                              \
                  QOOB_USB_CMD_CONTROL,            \
                  QOOB_USB_CMD_CONTROL_START,      \
                  QOOB_USB_CMD_ZERO,               \
                  0,                                \
                  (y));                             \
    send_command ((x),                              \
                  QOOB_USB_CMD_GET_ANSWER,         \
                  QOOB_USB_CMD_ZERO,               \
                  QOOB_USB_CMD_ZERO,               \
                  0,                                \
                  (y));                             \
  } while(0)

#define QOOB_END(x,y) \
       do {\
         send_command ((x),                     \
                       QOOB_USB_CMD_CONTROL,     \
                       QOOB_USB_CMD_CONTROL_END, \
                       QOOB_USB_CMD_ZERO,          \
                       0,                           \
                       (y));                        \
         send_command ((x),                         \
                       QOOB_USB_CMD_GET_ANSWER,    \
                       QOOB_USB_CMD_ZERO,          \
                       QOOB_USB_CMD_ZERO,          \
                       0,                           \
                       (y));\
       } while(0)


static int send_command (usb_dev_handle *devh, 
                         char *cmd1, 
                         char *cmd2, 
                         char *cmd3, 
                         unsigned char slot,
                         char *outbuf);

static int send_data (usb_dev_handle *devh, 
                      char *outbuf);

static int receive_answer (usb_dev_handle *devh, 
                           char *inbuf);

static void add_to_slot_array (qoob_t *qoob, 
                              int slot, 
                              char *name, 
                              char *info);

static qoob_error_t write_with_header_to_tmp_file (qoob_t *qoob, 
                                                   char *file,
                                                   size_t size);

qoob_error_t
qoob_usb_find (qoob_t *qoob)
{
  struct usb_bus *bus;

  /* Go trough all hubs etc. to find right device */
  for (bus = qoob->busses; bus; bus = bus->next) {
    struct usb_device *dev;
    usb_dev_handle *devh;

    for (dev = bus->devices; dev; dev = dev->next) {

      /* If usb device is QoobPro */
      if (dev->descriptor.idVendor == QOOB_PRO_VENDOR && 
          dev->descriptor.idProduct == QOOB_PRO_PRODUCT) {
        int ret;

        if (qoob->verbose > 0) {
          printf ("Qoob Pro found...\n");
        }

        /* Get interface to us */
        devh = usb_open (dev);

        if ((ret = usb_claim_interface (devh, 0)) < 0) {
          return QOOB_ERROR_CLAIM_INTERFACE;
        }

#ifndef HAVE_DARWIN
        /* There is only one altinterface. !!!Used shortcut!!! */
        if (usb_set_altinterface (devh, 0) < 0) {
          return QOOB_ERROR_ALT_INTERFACE;
        }
#endif

        qoob->dev = dev;
        qoob->devh = devh;

        if (qoob->verbose > 0) {
          printf ("Qoob Pro initializing OK...\n");
        }

        return QOOB_ERROR_OK;
      }
    }
  }

  return QOOB_ERROR_NOT_FOUND;
}

/* TODO: Check and reqrite asap with better knowledge about usb and flasher */
qoob_error_t 
qoob_usb_list (qoob_t *qoob, 
               qoob_slot_t **slots)
{
  char slot = 0;
  int tmpptr = 0;
  char buf[QOOB_PRO_MAX_BUFFER] = {0,};
  char tmpbuf[QOOB_PRO_MAX_BUFFER*4] = {0,};

  if (qoob->verbose > 0) {
    printf ("\nReading slots.\n");
  }

  /* TODO: implement system, which tries to 'connect' three times if fails */
#if 0   /* Do not remove this part */
  /* tests */
  send_command (qoob->devh, 
                QOOB_USB_CMD_GET_ANSWER, 
                QOOB_USB_CMD_ZERO, 
                QOOB_USB_CMD_ZERO, 
                0, 
                buf);
  receive_answer (qoob->devh, buf);
  send_command (qoob->devh, 
                QOOB_USB_CMD_GET_ANSWER, 
                QOOB_USB_CMD_ZERO, 
                QOOB_USB_CMD_ZERO, 
                0, 
                buf);
  receive_answer (qoob->devh, buf);
#endif

  /* Actual reading */
  while (1) {
    int i;
    printf (".");
    fflush (stdout);

    tmpptr = 0;
    QOOB_START (qoob->devh, buf);
    receive_answer (qoob->devh, buf);
    if (buf[2] != QOOB_START_OK)
    {
      return QOOB_ERROR_DEVICE_UNKNOWN1;
    }

    /* Slot reading */
    send_command (qoob->devh, 
                  QOOB_USB_CMD_READ_SLOT, 
                  QOOB_USB_CMD_ZERO, 
                  QOOB_USB_CMD_READ_SLOT_INFO,
                  slot,
                  buf);

    for (i=0;i<4;i++) {
      receive_answer (qoob->devh, buf);
      memcpy (tmpbuf+tmpptr, buf+1, QOOB_PRO_MAX_BUFFER-1);
      tmpptr += QOOB_PRO_MAX_BUFFER;
    }

    /* Other information */
    receive_answer (qoob->devh, buf);

    /* Add name to slot */
    if ((buf[SLOTS_IN_USE_INDEX] > 0) && 
        (buf[SLOTS_IN_USE_INDEX] <= QOOB_PRO_SLOTS)) {
      add_to_slot_array (qoob, (int)slot, tmpbuf, buf);
      /* Takes buf[2] slots */
      slot += (char)buf[SLOTS_IN_USE_INDEX];
    } else {

      /* Add empty slot */
      memset (tmpbuf, 0, QOOB_PRO_MAX_BUFFER*4);
      memcpy (tmpbuf, EMPTY_SLOT_NAME, strlen (EMPTY_SLOT_NAME));
      memset (buf, 0, QOOB_PRO_MAX_BUFFER);
      buf[SLOTS_IN_USE_INDEX] = 1;
      add_to_slot_array (qoob, (int)slot, tmpbuf, buf);
      slot++;
    }

    QOOB_END (qoob->devh, buf);
    receive_answer (qoob->devh, buf);

    if (slot > 31)
      break;
  }
  printf ("\n");

  *slots = qoob->slot;

  return QOOB_ERROR_OK;
}

qoob_error_t 
qoob_usb_read (qoob_t *qoob,
               char *file,
               short int slotnum)
{
  int ret,i,j;
  char buf[QOOB_PRO_MAX_BUFFER] = {0,};
  int fd;
  int count;
  size_t wrote;

  if (qoob == NULL) {
    return QOOB_ERROR_INPUT_NOT_VALID;;
  }

  if (qoob->devh == NULL) {
    return QOOB_ERROR_DEVICE_HANDLE_NOT_VALID;
  }

  if (file == NULL) {
    return QOOB_ERROR_FILE_NOT_VALID;
  }
  
  if (slotnum >= QOOB_PRO_SLOTS || slotnum < 0) {
    return QOOB_ERROR_SLOT_OUT_OF_RANGE;
  }

  if (qoob->slot[slotnum].first != TRUE) {
    return QOOB_ERROR_SLOT_NOT_FIRST;
  }

#ifdef DEBUG
  printf ("file: %s - slot [%02d] - slots used: %d\n", 
          file, 
          slotnum, 
          qoob->slot[slotnum].slots_used);
#endif

  fd = open (file, 
             (O_WRONLY|O_CREAT|O_TRUNC), 
             (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));

  if (fd == -1) {
    return QOOB_ERROR_FD_OPEN;
  }

  printf ("\nReading file '%s' starting at slot [%02d]", 
          file, 
          slotnum);

  QOOB_START (qoob->devh, buf);
  receive_answer (qoob->devh, buf);

  /* No need to size of the file with gcb fileformat. 
     Just read slots used by app 
   */
  for (i = (int)slotnum; 
       i < (int)(slotnum + 
                 qoob->slot[slotnum].slots_used); 
       i++) {
    count = 1;


    printf ("\nslot [%02d]\n", i);

    /* Reading app to file */
    send_command (qoob->devh, 
                  QOOB_USB_CMD_READ_SLOT, 
                  QOOB_USB_CMD_ZERO, 
                  QOOB_USB_CMD_READ_SLOT_ALL,
                  (char)i,
                  buf);

    /* Get 64 byte packet, but first byte is always zero. 
     * only 63 bytes is valid data to read.
     */
    for (j=0; j<QOOB_READ_LOOP_DEFAULT; j++) { /*about 1040 */
      if ((j%16)==0) {
        printf (".");
        fflush (stdout);
      }

      ret = receive_answer (qoob->devh, buf);
      
      wrote = write (fd, buf+1, (size_t)(ret-1));
      if (wrote == -1) {
        close (fd);
        return QOOB_ERROR_FD_WRITE;
      }

#if 0
      /* Works without this, not with!! Weird thing because 
         windows flasher sends this kind of command about 
         half of the slot position.
       */
      if (count == QOOB_READ_LOOP_HALF_WAY) {
        printf ("******** Reached half way of slot\n");
        send_command (qoob->devh, 
                      QOOB_USB_CMD_READ_SLOT, 
                      QOOB_USB_CMD_READ_SLOT_ALL_HALF_WAY,
                      QOOB_USB_CMD_READ_SLOT_ALL,
                      i,
                      buf);
      }
#endif
      count++;
    }
    ret = receive_answer (qoob->devh, buf);
    wrote = write (fd, buf+1, (size_t)(QOOB_READ_LOOP_MISSING_BYTES));
    if (wrote == -1) {
      close (fd);
      return QOOB_ERROR_FD_WRITE;
    }

  }

  QOOB_END (qoob->devh, buf);
  receive_answer (qoob->devh, buf);

  close (fd);

  printf ("\n");
  printf ("\nGCB file saved succesfully to '%s'.\n\n", file);

  return QOOB_ERROR_OK;
}

qoob_error_t 
qoob_usb_erase_forced (qoob_t *qoob, 
                       short int slot_from, 
                       short int slot_to)
{
  int i;
  char buf[QOOB_PRO_MAX_BUFFER];

  if (qoob == NULL) {
    return QOOB_ERROR_INPUT_NOT_VALID;
  }

  if (qoob->devh == NULL) {
    return QOOB_ERROR_DEVICE_HANDLE_NOT_VALID;
  }

  if (slot_from >= QOOB_PRO_SLOTS || slot_from < 0) {
    return QOOB_ERROR_SLOT_OUT_OF_RANGE;
  }

  if (slot_to >= QOOB_PRO_SLOTS || slot_to < 0) {
    return QOOB_ERROR_SLOT_OUT_OF_RANGE;
  }

  if (slot_from > slot_to ) {
    return QOOB_ERROR_SLOT_RANGE_NOT_VALID;
  }

  printf ("\nErasing app starting at slot [%02d].\n", slot_from);

  for (i=slot_from; i<=slot_to; i++) {

    printf (".");
    fflush (stdout);

    QOOB_START (qoob->devh, buf);
    receive_answer (qoob->devh, buf);

    send_command (qoob->devh, 
                  QOOB_USB_CMD_ERASE, 
                  QOOB_USB_CMD_ZERO, 
                  QOOB_USB_CMD_ZERO,
                  (char)i,
                  buf);
    send_command (qoob->devh,
                  QOOB_USB_CMD_GET_ANSWER,
                  QOOB_USB_CMD_ZERO,
                  QOOB_USB_CMD_ZERO,
                  0,
                  buf);

    QOOB_END (qoob->devh, buf);
    receive_answer (qoob->devh, buf);
  }

  return QOOB_ERROR_OK;
}

qoob_error_t 
qoob_usb_erase (qoob_t *qoob, 
                short int slot_num) 
{

  /* TODO: add checks */
  if (qoob == NULL) {
    return QOOB_ERROR_INPUT_NOT_VALID;;
  }

  if (qoob->slot[slot_num].first != TRUE) {
    return QOOB_ERROR_SLOT_NOT_FIRST;
  }

  return qoob_usb_erase_forced (qoob, 
                                 slot_num, 
                                 slot_num +
                                   qoob->slot[slot_num].slots_used - 
                                   1);

  return QOOB_ERROR_OK;
}

qoob_error_t 
qoob_usb_write (qoob_t *qoob,
                char *file,
                short int slotnum)
{
  int ret,i,j;
  char buf[QOOB_PRO_MAX_BUFFER];
  char tmpbuf[QOOB_PRO_MAX_BUFFER];
  int fd;
  int used_slots;
  off_t seek_to = 0;
  qoob_error_t err;
  struct stat sbuf;

  if (qoob == NULL) {
    return QOOB_ERROR_INPUT_NOT_VALID;;
  }

  if (qoob->devh == NULL) {
    return QOOB_ERROR_DEVICE_HANDLE_NOT_VALID;
  }

  if (file == NULL) {
    return QOOB_ERROR_FILE_NOT_VALID;
  }
  
  if (slotnum >= QOOB_PRO_SLOTS || slotnum < 0) {
    return QOOB_ERROR_SLOT_OUT_OF_RANGE;
  }

  qoob->real_file = strdup (file);

  /* If flashed non GCB file have to add 'header' */
  if (qoob->binary_type == QOOB_BINARY_TYPE_ELF ||
      qoob->binary_type == QOOB_BINARY_TYPE_DOL) {

    if (stat (qoob->real_file, &sbuf) == -1) {
      free (qoob->real_file);
      qoob->real_file = NULL;
      return QOOB_ERROR_FILE_STAT;
    }

    err = write_with_header_to_tmp_file (qoob, file, sbuf.st_size);
    if (err != QOOB_ERROR_OK) {
      free (qoob->real_file);
      qoob->real_file = NULL;
      return err;
    }
  }

  printf ("Real file: '%s'\n", qoob->real_file);

  /* FIXME: add more cases */
  if (stat (qoob->real_file, &sbuf) == -1) {
    free (qoob->real_file);
    qoob->real_file = NULL;
    return QOOB_ERROR_FILE_STAT;
  }

   /* Get how many slots is used */
  used_slots = sbuf.st_size/QOOB_PRO_SLOT_SIZE;
  if (sbuf.st_size > (QOOB_PRO_SLOT_SIZE) && 
      sbuf.st_size%(QOOB_PRO_SLOT_SIZE)) {
    used_slots++;
  }

#ifdef DEBUG
  printf ("Slots used: %d\n", used_slots);
#endif

  fd = open (qoob->real_file, O_RDONLY);
  if (fd == -1) {
    free (qoob->real_file);
    qoob->real_file = NULL;
    return QOOB_ERROR_FD_OPEN;
  }
  
  QOOB_START (qoob->devh, buf);
  receive_answer (qoob->devh, buf);

  printf ("\nWriting file '%s' starting at slot [%02d].\n", 
          file, slotnum);

  for (i=slotnum; i<(slotnum+used_slots); i++) {
    size_t written = 0;
    int runned = FALSE;

    printf ("\nslot[%d]\n",i); 

    memset (tmpbuf, 0, QOOB_PRO_MAX_BUFFER);

    send_command (qoob->devh, 
                  QOOB_USB_CMD_WRITE_SLOT, 
                  QOOB_USB_CMD_ZERO, 
                  QOOB_USB_CMD_WRITE_SLOT_ALL,
                  (char)i,
                  buf);

    ret = lseek (fd, seek_to, SEEK_SET);
    if (ret < 0) {
      close (fd);
      free (qoob->real_file);
      qoob->real_file = NULL;
      return QOOB_ERROR_FD_SEEK;
    }
    seek_to = seek_to + QOOB_DEFAULT_SEEK;

    for (j=0; j<QOOB_WRITE_LOOP_DEFAULT; j++) {
      size_t r;
      if ((j%32)==0) {
        printf (".");
        fflush (stdout);
      }


#ifdef DEBUG
      printf ("[%d] written %ld, sbuf.st_size %ld\n", 
              j, (long int)written, (long int)sbuf.st_size); 
#endif

      memset (buf, 0, QOOB_PRO_MAX_BUFFER);

      if ((runned == FALSE) && 
          (((j+1)%QOOB_WRITE_LOOP_HALF_WAY) == 0)) {
        off_t ret = 0;
        runned = TRUE;

#ifdef DEBUG
        printf ("About half way!\n");
#endif
        send_command (qoob->devh, 
                      QOOB_USB_CMD_WRITE_SLOT, 
                      QOOB_USB_CMD_WRITE_SLOT_ALL,
                      QOOB_USB_CMD_WRITE_SLOT_ALL,
                      (char)i,
                      buf);
        memset (buf, 0, QOOB_PRO_MAX_BUFFER);

        ret = lseek (fd, seek_to, SEEK_SET);
        if (ret < 0) {
          close (fd);
          free (qoob->real_file);
          qoob->real_file = NULL;
          return QOOB_ERROR_FD_SEEK;
        }

#ifdef DEBUG
        printf ("seek_to: 0x%lx\n", (unsigned long int)seek_to);
#endif
        seek_to = seek_to + QOOB_DEFAULT_SEEK;
      }

      r = read (fd, buf+1, QOOB_PRO_MAX_BUFFER-1);
      if (r == -1) {
        close (fd);
        free (qoob->real_file);
        qoob->real_file = NULL;
        return QOOB_ERROR_FD_READ;
      }

      ret = send_data (qoob->devh, buf);
      if (ret < 0) {
        close (fd);
        free (qoob->real_file);
        qoob->real_file = NULL;
        return QOOB_ERROR_SEND_DATA;
      }

      written += (ret - 1);
    }
  }

  QOOB_END (qoob->devh, buf);
  receive_answer (qoob->devh, buf);

  close (fd);
  free (qoob->real_file);
  qoob->real_file = NULL;
  return QOOB_ERROR_OK;

}

void
qoob_usb_clear (qoob_t *qoob)
{
  if (qoob->devh != NULL) {
    usb_release_interface (qoob->devh, 0);
    usb_close (qoob->devh);
    qoob->devh = NULL;
  }
}

/* Static functions */
static int 
send_command (usb_dev_handle *devh, 
              char *cmd1, 
              char *cmd2, 
              char *cmd3, 
              unsigned char slot, 
              char *outbuf) 
{

  /* build send command */
  memset (outbuf, 0, QOOB_PRO_MAX_BUFFER);
  if (cmd2 != NULL)
    *outbuf = (unsigned char)(*cmd1);     /* actual command */
  if (slot != 0)
    *(outbuf+1) = (unsigned char)slot;
  if (cmd2 != NULL)
    *(outbuf+2) = (unsigned char)(*cmd2); /* HI-byte of seek */
 /* *(outbuf+3)                              LO-byte of seek */
  if (cmd3 != NULL)
    *(outbuf+4) = (unsigned char)(*cmd3); /* ???? */

#ifdef DEBUG
  printf ("%s - outbuf:\n", __FUNCTION__);
  {
    int i;
    for (i=0;i<QOOB_PRO_MAX_BUFFER;i++) {
      printf ("0x%02x ", (unsigned char)outbuf[i]);
    }
    printf ("\n");
  }
#endif

  /* FIXME: check names to defines from USB-specification */
  return usb_control_msg (devh, 
                          USB_TYPE_CLASS+USB_RECIP_INTERFACE, 
                          SEND_REQUEST, 
                          SEND_VALUE, 
                          0, 
                          outbuf,
                          QOOB_PRO_MAX_BUFFER,
                          DEFAULT_TIMEOUT);

}

static int 
send_data (usb_dev_handle *devh, 
           char *outbuf) 
{
  int ret;

  /* FIXME: check names to defines from USB-specification */
  ret = usb_control_msg (devh, 
                         USB_TYPE_CLASS+USB_RECIP_INTERFACE, 
                         SEND_REQUEST, 
                         SEND_VALUE,
                         0,
                         outbuf,
                         QOOB_PRO_MAX_BUFFER,
                         DEFAULT_TIMEOUT);

  if (ret < 0) {
    fprintf (stderr, "Could not send data to QoobPro!!! Error: %d\n", ret);
  }

#ifdef DEBUG
  printf ("\n%s - outbuf:\n", __FUNCTION__);
  {
    int i;
    for (i=0;i<QOOB_PRO_MAX_BUFFER;i++) {
      printf ("0x%02x ", (unsigned char)outbuf[i]);
    }
    printf ("\n");
  }
#endif

  return ret;
}

static int receive_answer (usb_dev_handle *devh, 
                           char *inbuf)
{
  int ret;

  /* FIXME: check names to defines from USB-specification */
  ret = usb_control_msg (devh, 
                         USB_TYPE_CLASS+USB_RECIP_INTERFACE+USB_ENDPOINT_IN, 
                         RECV_REQUEST, 
                         RECV_VALUE, 
                         0, 
                         inbuf,
                         QOOB_PRO_MAX_BUFFER,
                         DEFAULT_TIMEOUT);

#ifdef DEBUG
  printf ("\n%s - inbuf:\n", __FUNCTION__);
  {
    int i;
    for (i=0;i<QOOB_PRO_MAX_BUFFER;i++) {
      printf ("0x%02x ", (unsigned char)inbuf[i]);
    }
    printf ("\n");
  }
#endif

  return ret;


}

static void
add_to_slot_array (qoob_t *qoob, 
                   int slot_number, 
                   char *name, 
                   char *info)
{
  int i;
  unsigned short int used_slots;
  int name_start = 0;
  char continuing [QOOB_PRO_MAX_BUFFER];

  used_slots = info[SLOTS_IN_USE_INDEX];
  name_start = 4;

  if (name[0]=='E' &&
      name[1]=='L' &&
      name[2]=='F' &&
      info[6]=='E' &&
      info[7]=='L' &&
      info[8]=='F') {
    qoob->slot[slot_number].type = QOOB_BINARY_TYPE_ELF;
  } else if (name[0]=='E' &&
             name[1]=='L' &&
             name[2]=='F') {
    qoob->slot[slot_number].type = QOOB_BINARY_TYPE_DOL;
  } else if (name[0]=='(' &&
             name[1]=='C' &&
             name[2]==')') {
    qoob->slot[slot_number].type = QOOB_BINARY_TYPE_GCB;
    /* TODO: check is this correct or just this case */
  } else if (slot_number == 31 &&
             name[0]==(char)0x51 &&
             name[1]==(char)0x43 &&
             name[2]==(char)0x46 &&
             name[3]==(char)0x47) {
    strncpy (name, CONFIG_SLOT_NAME, strlen (CONFIG_SLOT_NAME));
    qoob->slot[slot_number].type = QOOB_BINARY_TYPE_CONFIG;
  } else {
    qoob->slot[slot_number].type = QOOB_BINARY_TYPE_VOID;
  }


  for (i=0; i<used_slots; i++) {
    memcpy (qoob->slot[slot_number+i].name, 
            name+name_start, 
            QOOB_PRO_MAX_BUFFER*4);
    qoob->slot[slot_number+i].slots_used = used_slots;

    if (i != 0) {
      sprintf (continuing, CONTINUING_TEXT, i+1);
      memcpy (qoob->slot[slot_number+i].name +
                strlen (qoob->slot[slot_number+i].name), 
              continuing, 
              strlen (continuing));
      qoob->slot[slot_number+i].first = FALSE;
    }
  }
}

static qoob_error_t 
write_with_header_to_tmp_file (qoob_t *qoob,
                               char *file,
                               size_t size)
{
  size_t i = 4;
  int fd, fd_orig;
  char *new_name;
  size_t w;
  size_t r;
  char *end = NULL;
  char *start = NULL;
  char buf[4096];
  char used_slots;
  int left = QOOB_PRO_SLOT_SIZE-((QOOB_GCB_HEADER_SIZE+size)%QOOB_PRO_SLOT_SIZE);

  used_slots = (char)((size+QOOB_GCB_HEADER_SIZE+left)/QOOB_PRO_SLOT_SIZE);
  if (((size+QOOB_GCB_HEADER_SIZE+left)>QOOB_PRO_SLOT_SIZE) && 
      ((size+QOOB_GCB_HEADER_SIZE+left)%QOOB_PRO_SLOT_SIZE)) {    
    used_slots++;
  }
  
  end = file + strlen (file)-1;

  /* tmpnam might be unsafe */
  new_name = tmpnam (NULL);

  fd = open (new_name,
             (O_WRONLY|O_CREAT|O_TRUNC), 
             (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH));

  fd_orig = open (file, O_RDONLY);
  if (fd_orig == -1) {
    close (fd);
    return QOOB_ERROR_FD_OPEN;
  }

  /* 1. Write ELF\0 for both ELF and DOL there is such thing */
  w = write (fd, "ELF\0", 4);
  if (w != 4) {
    goto werror;
  }
  
  /* 2. Filename */
  /* Get filename */
  for (; end != file && *end != QOOB_PREFIX_SEPARATOR; end--);
  if (end == file) {
    end = file + strlen (file)-1;
  }
  start = end;
  for (; start != file && *start != QOOB_DIRECTORY_SEPARATOR; start--);
  if (*start == QOOB_DIRECTORY_SEPARATOR) {
    start++;
  }

  /* Write filename */
  i += (size_t)(end-start);
  w = write (fd, start, (size_t)(end-start));
  if (w != (size_t)(end-start)) {
    goto werror;
  }

  /* 3. Fill rest of the header */
  /* Write zeros to rest of header except to 0xfd is written how many slots 
     is used 
   */
  for (; i<QOOB_GCB_HEADER_SIZE; i++) {
    if (i == 0xfd) {
      w = write (fd, (char *)&used_slots, 1);
    }else {
      w = write (fd, "\0", 1);
    }
    if (w != 1) {
      goto werror;
    }
  }

  /* 4. Write given file */
  while ((r = read (fd_orig, buf, 4095)) == 4095) {
    w = write (fd, buf, r);
    if (w != r) {
      goto werror;
    }
    i += w;
  }
  w = write (fd, buf, r);
  if (w != r) {
    goto werror;
  }
  i += w;

  /* 5. Fill the rest of the last slot */
  while (i%QOOB_PRO_SLOT_SIZE) {
    w = write (fd, "\0", 1);
    i += w;
    if (w != 1) {
      goto werror;
    }
  }

  free (qoob->real_file);
  qoob->real_file = strdup (new_name);

  /* If situation is that there are no mem.
     Better to leave whole application.
   */
  if (qoob->real_file == NULL) {
    fprintf (stderr, "Not enough memory!!!\n");
    exit (112);
  }

  close (fd);
  close (fd_orig);


  return QOOB_ERROR_OK;
 werror:
  close (fd);
  close (fd_orig);
  return QOOB_ERROR_FD_WRITE;
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
