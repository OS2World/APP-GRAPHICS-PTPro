/***************************************************************************/
/*
 * Architecture-specific implementation
 *
 * Copyright (c) 2000-2003 Johannes Erdfelt <johannes@erdfelt.com>
 *
 * This library is covered by the LGPL, read LICENSE for details.
 *
 * Ported to OS/2 from the original Linux code by Richard L Walsh
 * Copyright (c) 2006 Richard L Walsh
 *
 */
/***************************************************************************/
/*
 * This port was designed to use USBCALLS as a static library.
 * As such, it has to open & close the device driver explicitly -
 * functions which are normally handled by the dll's _Init_Term.
 * To use this library with usbcalls.dll, simply comment-out the
 * contents of usb_os_init() & usb_os_term().
 *
 * An apology of sorts:  since both USB & Linux are largely a
 * mystery to me, the quality of this port is ..umm.. "uneven".
 * The stuff I've tested with my single USB app works fine;
 * use the remainder at-your-own-risk and/or fix it.
 *
 * Tested, seem OK:
 *  usb_os_open
 *  usb_os_close
 *  usb_set_configuration
 *  usb_control_msg
 *  usb_bulk_write
 *  usb_bulk_read
 *  usb_os_find_busses
 *  usb_os_find_devices
 *  usb_os_init
 *  usb_os_term     (my addition)
 *
 * Implemented incorrectly or not at all (and certainly not tested):
 *  usb_claim_interface
 *  usb_release_interface
 *  usb_set_altinterface
 *  usb_interrupt_write
 *  usb_interrupt_read
 *  usb_os_determine_children
 *  usb_resetep
 *  usb_clear_halt
 *  usb_reset
 *
 */
/***************************************************************************/

#define INCL_DOS
#include <os2.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "usbcalls.h"
#include "usbi.h"

/***************************************************************************/

// acquires a device & returns the equivalent of a file handle

int usb_os_open( usb_dev_handle *dev)
{
  int       rtn = 0;
  APIRET    rc;
  int       handle;

  rc = UsbOpen( (PUSBHANDLE)&handle,
                (USHORT)dev->device->descriptor.idVendor,
                (USHORT)dev->device->descriptor.idProduct,
                (USHORT)dev->device->descriptor.bcdDevice,
                (USHORT)USB_OPEN_FIRST_UNUSED);

  if (rc) {
    USB_ERROR_MSG( "unable to open device - id= %x/%x  rc= %x",
                   dev->device->descriptor.idVendor, 
                   dev->device->descriptor.idProduct, (int)rc);
    handle = -1;
    rtn = -1;
  }

  dev->fd = handle;

  return rtn;
}

/***************************************************************************/

// release a device

int usb_os_close( usb_dev_handle *dev)
{
  int       rtn = 0;
  APIRET    rc;

  if (dev->fd < 0)
    return rtn;

  rc = UsbClose( (USBHANDLE)dev->fd);
  if (rc) {
    USB_ERROR_MSG( "unable to close device - id= %x/%x  handle= %x  rc= %x",
                   dev->device->descriptor.idVendor, 
                   dev->device->descriptor.idProduct,
                   dev->fd, (int)rc);
    rtn = -1;
  }

  dev->fd = -1;

  return rtn;
}

/***************************************************************************/

// UsbDeviceSetConfiguration() is a macro that calls UsbCtrlMessage()

int usb_set_configuration( usb_dev_handle *dev, int configuration)
{
  int       rtn = 0;
  APIRET    rc;

  rc = UsbDeviceSetConfiguration( (USBHANDLE)dev->fd, (USHORT)configuration);
  if (rc) {
    USB_ERROR_MSG( "unable to set configuration - config= %d  rc= %x",
                   configuration, (int)rc);
    rtn = -1;
  }
  else
    dev->config = configuration;

  return rtn;
}

/***************************************************************************/

// USBRESM$ appears to handle this as part of opening the device

int usb_claim_interface( usb_dev_handle *dev, int interface)
{
  dev->interface = interface;
  return 0;
}

/***************************************************************************/

// USBRESM$ appears to handle this as part of opening the device

int usb_release_interface( usb_dev_handle *dev, int interface)
{
  dev->interface = ~0;
  return 0;
}

/***************************************************************************/

// UsbInterfaceSetAltSetting() is a macro that calls UsbCtrlMessage()
// I haven't a clue if this is correct

int usb_set_altinterface( usb_dev_handle *dev, int alternate)
{
  int       rtn = 0;
  APIRET    rc;

  rc = UsbInterfaceSetAltSetting( (USBHANDLE)dev->fd, (USHORT)dev->interface,
                                  (USHORT)alternate);
  if (rc) {
    USB_ERROR_MSG( "unable to set alt interface - intf= %d  alt= %d  rc= %x",
                   dev->interface, alternate, (int)rc);
    rtn = -1;
  }
  else
    dev->altsetting = alternate;

  return rtn;
}

/***************************************************************************/

// straight-forward, I think

int usb_control_msg( usb_dev_handle *dev, int requesttype,
                     int request, int value, int index,
                     char *bytes, int size, int timeout)
{
  int       rtn = 0;
  APIRET    rc;

  rc = UsbCtrlMessage( (USBHANDLE)dev->fd, (UCHAR)requesttype,
                       (UCHAR)request, (USHORT)value, (USHORT)index,
                       (USHORT)size, bytes, (ULONG)timeout);

  if (rc) {
    USB_ERROR_MSG( "unable to send control message - rc= %x", (int)rc);
    rtn = -1;
  }

  return rtn;
}

/***************************************************************************/

// UsbBulkWrite() will handle packet-izing the data so we don't have to

int usb_bulk_write( usb_dev_handle *dev, int ep, char *bytes,
                    int size, int timeout)
{
  int       rtn = size;
  APIRET    rc;

  rc = UsbBulkWrite( (USBHANDLE)dev->fd, (UCHAR)ep, (UCHAR)dev->interface,
                     (ULONG)size, bytes, (ULONG)timeout);
  if (rc) {
    USB_ERROR_MSG( "unable to write to bulk endpoint - size= %d  rc= %x",
                   size, (int)rc);
    rtn = -1;
  }

  return rtn;
}

/***************************************************************************/

// UsbBulkRead() will handle packet-izing the data so we don't have to

int usb_bulk_read( usb_dev_handle *dev, int ep, char *bytes,
                   int size, int timeout)
{
  int       rtn = size;
  APIRET    rc;

  rc = UsbBulkRead( (USBHANDLE)dev->fd, (UCHAR)ep, (UCHAR)dev->interface,
                    (PULONG)&rtn, bytes, (ULONG)timeout);

  // Froloff keeps changing the return code
  if (rc && rc != USB_ERROR_LESSTRANSFERED && rc != 0x8004 && rc != 7004) {
    USB_ERROR_MSG( "unable to read from bulk endpoint - size= %d  rtn= %d  rc= %x",
                   size, rtn, (int)rc);
    rtn = -1;
  }

  return rtn;
}

/***************************************************************************/

// usbresmg.sys dated 2006/02/21 is supposed to support interrupt
// handling correctly but all of my attempts to use it have failed;
// consequently, this function remains unimplemented

int usb_interrupt_write( usb_dev_handle *dev, int ep, char *bytes,
                         int size, int timeout)
{
  USB_ERROR_MSG( "usb_interrupt_write not implemented");
  return -1;
}

/***************************************************************************/

// usbresmg.sys dated 2006/02/21 is supposed to support interrupt
// handling correctly but all of my attempts to use it have failed;
// consequently, this function remains unimplemented

int usb_interrupt_read( usb_dev_handle *dev, int ep, char *bytes,
                        int size, int timeout)
{
  USB_ERROR_MSG( "usb_interrupt_read not implemented");
  return -1;
}

/***************************************************************************/

// USBRESM$ presents a single, unified bus;  since most apps expect
// it to have a number, I've assigned it "1"

int usb_os_find_busses( struct usb_bus **busses)
{
  int       rtn = 0;

  *busses = (struct usb_bus *)calloc( 1, sizeof(struct usb_bus));

  if (!*busses) {
    USB_ERROR_MSG( "unable to allocate memory in usb_os_find_busses");
    rtn = -1;
  }
  else
    strcpy( (*busses)->dirname, "1");

  return rtn;
}

/***************************************************************************/

// for each device, UsbQueryDeviceReport() returns a device descriptor
// followed by a series of configuration descriptors which this parses

int usb_os_find_devices( struct usb_bus *bus, struct usb_device **devices)
{
  int       rtn = 0;
  APIRET    rc;
  ULONG     cntDev;
  ULONG     ctr;
  ULONG     i;
  ULONG     len;
  struct usb_device *   fdev = NULL;
  struct usb_device *   dev;
  char      report[1024];

  rc = UsbQueryNumberDevices( &cntDev);
  if (rc) {
    USB_ERROR_MSG( "unable to query number of USB devices - rc= %x", (int)rc);
    return -1;
  }

  for (ctr = 1; ctr <= cntDev; ctr++) {

    len = sizeof(report);
    rc = UsbQueryDeviceReport( ctr, &len, report);
    if (rc) {
      USB_ERROR_MSG( "unable to query device report - device= %d  rc= %x",
                     (int)ctr, (int)rc);
      return -1;
    }

    dev = calloc( 1, sizeof(struct usb_device));
    if (!dev) {
      USB_ERROR_MSG( "unable to allocate memory in usb_os_find_devices");
      return -1;
    }

    dev->bus = bus;
    dev->devnum = ctr;
    itoa( ctr, dev->filename, 10);
    memcpy( &dev->descriptor, report, sizeof(dev->descriptor));

    LIST_ADD( fdev, dev);

    USB_DEBUG_MSG( "Found device %s on bus %s", dev->filename, bus->dirname);

    if (dev->descriptor.bNumConfigurations > USB_MAXCONFIG ||
        dev->descriptor.bNumConfigurations < 1) {
      USB_DEBUG_MSG( "invalid number of device configurations - nbr= %d  continuing...",
                     dev->descriptor.bNumConfigurations);
      continue;
    }

    i = sizeof(struct usb_device_descriptor) +
         (dev->descriptor.bNumConfigurations *
          sizeof(struct usb_config_descriptor));
    if (len < i) {
      USB_DEBUG_MSG( "device report too short - expected= %d  actual= %d  continuing...",
                     (int)i, (int)len);
      continue;
    }

    dev->config = (struct usb_config_descriptor *)calloc(
        dev->descriptor.bNumConfigurations, sizeof(struct usb_config_descriptor));
    if (!dev->config) {
      USB_ERROR_MSG( "unable to allocate memory in usb_os_find_devices");
      return -1;
    }

    for (i = 0, len = sizeof(struct usb_device_descriptor);
         i < dev->descriptor.bNumConfigurations; i++) {

      char * ptr;

      ptr = &report[len];
      len += ((struct usb_config_descriptor *)ptr)->wTotalLength;
      rtn = usb_parse_configuration( &dev->config[i], ptr);

      if (rtn > 0)
        USB_DEBUG_MSG( "Descriptor data still left - bytes= %d", rtn);
      else
        if (rtn < 0)
          USB_DEBUG_MSG( "Unable to parse descriptors");
    } // configs

  } // devices

  *devices = fdev;

  return 0;
}

/***************************************************************************/

// if I had a hub to test with, I would have ported this

int usb_os_determine_children( struct usb_bus *bus)
{
//    USB_DEBUG_MSG( "usb_os_determine_children not implemented");
    return 0;
}

/***************************************************************************/

// modified to support use of usbcalls as a static library;
// to use the dll version of usbcalls, comment-out the code

int usb_os_init( void)
{
/*
  APIRET rc = UsbDriverInit();
  if (rc) {
    USB_ERROR_MSG( "unable to open USBRESMG.SYS - rc= %x", (int)rc);
    return (-1);
  }
*/
  return 0;
}

/***************************************************************************/

// added to support use of usbcalls as a static library;
// to use the dll version of usbcalls, comment-out the code

int usb_os_term( void)
{
/*
  APIRET rc = UsbDriverTerm();
  if (rc) {
    USB_ERROR_MSG( "unable to close USBRESMG.SYS - rc= %x", (int)rc);
    return (-1);
  }
*/
  return 0;
}

/***************************************************************************/

// this is probably correct

int usb_clear_halt( usb_dev_handle *dev, unsigned int ep)
{
  USB_DEBUG_MSG( "usb_clear_halt - ep= %x", ep);
  return (usb_control_msg( dev, USB_RECIP_ENDPOINT,
                           USB_REQ_CLEAR_FEATURE, 0, ep, 0, 0, 0));
}

/***************************************************************************/

// this is identical to usb_clear_halt() - I have no idea if it should be

int usb_resetep( usb_dev_handle *dev, unsigned int ep)
{
  USB_DEBUG_MSG( "usb_resetep - ep= %x", ep);
  return (usb_control_msg( dev, USB_RECIP_ENDPOINT,
                           USB_REQ_CLEAR_FEATURE, 0, ep, 0, 0, 0));
}

/***************************************************************************/

// this is almost guaranteed to be horribly wrong - fix me, please!

int usb_reset( usb_dev_handle *dev)
{
  USB_DEBUG_MSG( "usb_reset");
  return (usb_control_msg( dev, USB_TYPE_CLASS | USB_RECIP_DEVICE,
                           USB_REQ_CLEAR_FEATURE, 0, 0, 0, 0, 0));
}

/***************************************************************************/

