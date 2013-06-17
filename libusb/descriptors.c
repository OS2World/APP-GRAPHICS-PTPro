/*
 * Parses descriptors
 *
 * Copyright (c) 2001 Johannes Erdfelt <johannes@erdfelt.com>
 *
 * This library is covered by the LGPL, read LICENSE for details.
 *
 * Changes made by Richard L Walsh:
 * - replaced fprintf()s with 2 macros: USB_ERROR_MSG() and USB_DEBUG_MSG()
 * Copyright (c) 2006 Richard L Walsh <rich@e-vertise.com>
 *
 */

#include <stdio.h>
#include <string.h>
#include "usbi.h"

int usb_get_descriptor_by_endpoint(usb_dev_handle *udev, int ep,
    unsigned char type, unsigned char index, void *buf, int size)
{
  memset(buf, 0, size);

  return usb_control_msg(udev, ep | USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
                        (type << 8) + index, 0, buf, size, 1000);
}

int usb_get_descriptor(usb_dev_handle *udev, unsigned char type,
    unsigned char index, void *buf, int size)
{
  memset(buf, 0, size);

  return usb_control_msg(udev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
                        (type << 8) + index, 0, buf, size, 1000);
}

/*
 * This code looks surprisingly similar to the code I wrote for the Linux
 * kernel. It's not a coincidence :)
 */

static int usb_parse_endpoint(struct usb_endpoint_descriptor *endpoint, unsigned char *buffer, int size)
{
  struct usb_descriptor_header *header;
  unsigned char *begin;
  int parsed = 0, len, numskipped;

  header = (struct usb_descriptor_header *)buffer;

  /* Everything should be fine being passed into here, but we sanity */
  /*  check JIC */
  if (header->bLength > size) {
    USB_ERROR_MSG( "ran out of descriptors parsing");
    return -1;
  }
                
  if (header->bDescriptorType != USB_DT_ENDPOINT) {
    USB_DEBUG_MSG( "unexpected descriptor 0x%X, expecting endpoint descriptor, type 0x%X",
                   endpoint->bDescriptorType, USB_DT_ENDPOINT);
    return parsed;
  }
  if (header->bLength == USB_DT_ENDPOINT_AUDIO_SIZE)
    memcpy(endpoint, buffer, USB_DT_ENDPOINT_AUDIO_SIZE);
  else
    memcpy(endpoint, buffer, USB_DT_ENDPOINT_SIZE);
        
  USB_LE16_TO_CPU(endpoint->wMaxPacketSize);

  buffer += header->bLength;
  size -= header->bLength;
  parsed += header->bLength;

  /* Skip over the rest of the Class Specific or Vendor Specific */
  /*  descriptors */
  begin = buffer;
  numskipped = 0;
  while (size >= sizeof(struct usb_descriptor_header)) {
    header = (struct usb_descriptor_header *)buffer;

    if (header->bLength < 2) {
      USB_ERROR_MSG( "invalid descriptor length of %d", header->bLength);
      return -1;
    }

    /* If we find another "proper" descriptor then we're done  */
    if ((header->bDescriptorType == USB_DT_ENDPOINT) ||
        (header->bDescriptorType == USB_DT_INTERFACE) ||
        (header->bDescriptorType == USB_DT_CONFIG) ||
        (header->bDescriptorType == USB_DT_DEVICE))
      break;

    USB_ERROR_MSG( "skipping descriptor 0x%X", header->bDescriptorType);
    numskipped++;

    buffer += header->bLength;
    size -= header->bLength;
    parsed += header->bLength;
  }

  if (numskipped)
    USB_DEBUG_MSG( "skipped %d class/vendor specific endpoint descriptors", numskipped);

  /* Copy any unknown descriptors into a storage area for drivers */
  /*  to later parse */
  len = (int)(buffer - begin);
  if (!len) {
    endpoint->extra = NULL;
    endpoint->extralen = 0;
    return parsed;
  }

  endpoint->extra = malloc(len);
  if (!endpoint->extra) {
    USB_ERROR_MSG( "couldn't allocate memory for endpoint extra descriptors");
    endpoint->extralen = 0;
    return parsed;
  }

  memcpy(endpoint->extra, begin, len);
  endpoint->extralen = len;

  return parsed;
}

static int usb_parse_interface(struct usb_interface *interface,
    unsigned char *buffer, int size)
{
  int i, len, numskipped, retval, parsed = 0;
  struct usb_descriptor_header *header;
  struct usb_interface_descriptor *ifp;
  unsigned char *begin;

  interface->num_altsetting = 0;

  while (size > 0) {
    interface->altsetting = realloc(interface->altsetting, sizeof(struct usb_interface_descriptor) * (interface->num_altsetting + 1));
    if (!interface->altsetting) {
      USB_ERROR_MSG( "couldn't malloc interface->altsetting");
      return -1;
    }

    ifp = interface->altsetting + interface->num_altsetting;
    interface->num_altsetting++;

    memcpy(ifp, buffer, USB_DT_INTERFACE_SIZE);

    /* Skip over the interface */
    buffer += ifp->bLength;
    parsed += ifp->bLength;
    size -= ifp->bLength;

    begin = buffer;
    numskipped = 0;

    /* Skip over any interface, class or vendor descriptors */
    while (size >= sizeof(struct usb_descriptor_header)) {
      header = (struct usb_descriptor_header *)buffer;

      if (header->bLength < 2) {
        USB_ERROR_MSG( "invalid descriptor length of %d", header->bLength);
        return -1;
      }

      /* If we find another "proper" descriptor then we're done */
      if ((header->bDescriptorType == USB_DT_INTERFACE) ||
          (header->bDescriptorType == USB_DT_ENDPOINT) ||
          (header->bDescriptorType == USB_DT_CONFIG) ||
          (header->bDescriptorType == USB_DT_DEVICE))
        break;

      numskipped++;

      buffer += header->bLength;
      parsed += header->bLength;
      size -= header->bLength;
    }

    if (numskipped)
      USB_DEBUG_MSG( "skipped %d class/vendor specific interface descriptors", numskipped);

    /* Copy any unknown descriptors into a storage area for */
    /*  drivers to later parse */
    len = (int)(buffer - begin);
    if (!len) {
      ifp->extra = NULL;
      ifp->extralen = 0;
    } else {
      ifp->extra = malloc(len);
      if (!ifp->extra) {
        USB_ERROR_MSG( "couldn't allocate memory for interface extra descriptors");
        ifp->extralen = 0;
        return -1;
      }
      memcpy(ifp->extra, begin, len);
      ifp->extralen = len;
    }

    /* Did we hit an unexpected descriptor? */
    header = (struct usb_descriptor_header *)buffer;
    if ((size >= sizeof(struct usb_descriptor_header)) &&
        ((header->bDescriptorType == USB_DT_CONFIG) ||
        (header->bDescriptorType == USB_DT_DEVICE)))
      return parsed;

    if (ifp->bNumEndpoints > USB_MAXENDPOINTS) {
      USB_ERROR_MSG( "too many endpoints");
      return -1;
    }

    ifp->endpoint = (struct usb_endpoint_descriptor *)
                     malloc(ifp->bNumEndpoints *
                     sizeof(struct usb_endpoint_descriptor));
    if (!ifp->endpoint) {
      USB_ERROR_MSG( "couldn't allocate memory for ifp->endpoint");
      return -1;      
    }

    memset(ifp->endpoint, 0, ifp->bNumEndpoints *
           sizeof(struct usb_endpoint_descriptor));

    for (i = 0; i < ifp->bNumEndpoints; i++) {
      header = (struct usb_descriptor_header *)buffer;

      if (header->bLength > size) {
        USB_ERROR_MSG( "ran out of descriptors parsing");
        return -1;
      }
                
      retval = usb_parse_endpoint(ifp->endpoint + i, buffer, size);
      if (retval < 0)
        return retval;

      buffer += retval;
      parsed += retval;
      size -= retval;
    }

    /* We check to see if it's an alternate to this one */
    ifp = (struct usb_interface_descriptor *)buffer;
    if (size < USB_DT_INTERFACE_SIZE ||
        ifp->bDescriptorType != USB_DT_INTERFACE ||
        !ifp->bAlternateSetting)
      return parsed;
  }

  return parsed;
}

int usb_parse_configuration(struct usb_config_descriptor *config, char *buffer)
{
  int i, retval, size;
  struct usb_descriptor_header *header;

  memcpy(config, buffer, USB_DT_CONFIG_SIZE);
  USB_LE16_TO_CPU(config->wTotalLength);
  size = config->wTotalLength;

  if (config->bNumInterfaces > USB_MAXINTERFACES) {
    USB_ERROR_MSG( "too many interfaces");
    return -1;
  }

  config->interface = (struct usb_interface *)
                       malloc(config->bNumInterfaces *
                       sizeof(struct usb_interface));
  if (!config->interface) {
    USB_ERROR_MSG( "out of memory");
    return -1;      
  }

  memset(config->interface, 0, config->bNumInterfaces * sizeof(struct usb_interface));

  buffer += config->bLength;
  size -= config->bLength;
        
  config->extra = NULL;
  config->extralen = 0;

  for (i = 0; i < config->bNumInterfaces; i++) {
    int numskipped, len;
    char *begin;

    /* Skip over the rest of the Class Specific or Vendor */
    /*  Specific descriptors */
    begin = buffer;
    numskipped = 0;
    while (size >= sizeof(struct usb_descriptor_header)) {
      header = (struct usb_descriptor_header *)buffer;

      if ((header->bLength > size) || (header->bLength < 2)) {
        USB_ERROR_MSG( "invalid descriptor length of %d", header->bLength);
        return -1;
      }

      /* If we find another "proper" descriptor then we're done */
      if ((header->bDescriptorType == USB_DT_ENDPOINT) ||
          (header->bDescriptorType == USB_DT_INTERFACE) ||
          (header->bDescriptorType == USB_DT_CONFIG) ||
          (header->bDescriptorType == USB_DT_DEVICE))
        break;

      USB_DEBUG_MSG( "skipping descriptor 0x%X", header->bDescriptorType);
      numskipped++;

      buffer += header->bLength;
      size -= header->bLength;
    }

    if (numskipped)
      USB_DEBUG_MSG( "skipped %d class/vendor specific endpoint descriptors", numskipped);

    /* Copy any unknown descriptors into a storage area for */
    /*  drivers to later parse */
    len = (int)(buffer - begin);
    if (len) {
      /* FIXME: We should realloc and append here */
      if (!config->extralen) {
        config->extra = malloc(len);
        if (!config->extra) {
          USB_ERROR_MSG( "couldn't allocate memory for config extra descriptors");
          config->extralen = 0;
          return -1;
        }

        memcpy(config->extra, begin, len);
        config->extralen = len;
      }
    }

    retval = usb_parse_interface(config->interface + i, buffer, size);
    if (retval < 0)
      return retval;

    buffer += retval;
    size -= retval;
  }

  return size;
}

void usb_destroy_configuration(struct usb_device *dev)
{
  int c, i, j, k;
        
  if (!dev->config)
    return;

  for (c = 0; c < dev->descriptor.bNumConfigurations; c++) {
    struct usb_config_descriptor *cf = &dev->config[c];

    if (!cf->interface)
      break;

    for (i = 0; i < cf->bNumInterfaces; i++) {
      struct usb_interface *ifp = &cf->interface[i];
                                
      if (!ifp->altsetting)
        break;

      for (j = 0; j < ifp->num_altsetting; j++) {
        struct usb_interface_descriptor *as = &ifp->altsetting[j];
                                        
        if (as->extra)
          free(as->extra);

        if (!as->endpoint)
          break;
                                        
        for (k = 0; k < as->bNumEndpoints; k++) {
          if (as->endpoint[k].extra)
            free(as->endpoint[k].extra);
        }       
        free(as->endpoint);
      }

      free(ifp->altsetting);
    }

    free(cf->interface);
  }

  free(dev->config);
}

void usb_fetch_and_parse_descriptors(usb_dev_handle *udev)
{
  struct usb_device *dev = udev->device;
  int i;

  if (dev->descriptor.bNumConfigurations > USB_MAXCONFIG) {
    USB_ERROR_MSG( "Too many configurations (%d > %d)", dev->descriptor.bNumConfigurations, USB_MAXCONFIG);
    return;
  }

  if (dev->descriptor.bNumConfigurations < 1) {
    USB_ERROR_MSG( "Not enough configurations (%d < %d)", dev->descriptor.bNumConfigurations, 1);
    return;
  }

  dev->config = (struct usb_config_descriptor *)malloc(dev->descriptor.bNumConfigurations * sizeof(struct usb_config_descriptor));
  if (!dev->config) {
    USB_ERROR_MSG( "Unable to allocate memory for config descriptor");
    return;
  }

  memset(dev->config, 0, dev->descriptor.bNumConfigurations *
         sizeof(struct usb_config_descriptor));

  for (i = 0; i < dev->descriptor.bNumConfigurations; i++) {
    char buffer[8], *bigbuffer;
    struct usb_config_descriptor *desc = (struct usb_config_descriptor *)buffer;
    int res;

    /* Get the first 8 bytes so we can figure out what the total length is */
    res = usb_get_descriptor(udev, USB_DT_CONFIG, i, buffer, 8);
    if (res < 8) {
      if (res < 0)
        USB_ERROR_MSG( "Unable to get descriptor (%d)", res);
      else
        USB_ERROR_MSG( "Config descriptor too short (expected %d, got %d)", 8, res);

      goto err;
    }

    USB_LE16_TO_CPU(desc->wTotalLength);

    bigbuffer = malloc(desc->wTotalLength);
    if (!bigbuffer) {
      USB_ERROR_MSG( "Unable to allocate memory for descriptors");
      goto err;
    }

    res = usb_get_descriptor(udev, USB_DT_CONFIG, i, bigbuffer, desc->wTotalLength);
    if (res < desc->wTotalLength) {
      if (res < 0)
        USB_ERROR_MSG( "Unable to get descriptor (%d)", res);
      else
        USB_ERROR_MSG( "Config descriptor too short (expected %d, got %d)", desc->wTotalLength, res);

      free(bigbuffer);
      goto err;
    }

    res = usb_parse_configuration(&dev->config[i], bigbuffer);
    if (res > 0)
      USB_DEBUG_MSG( "Descriptor data still left");
    else if (res < 0)
      USB_DEBUG_MSG( "Unable to parse descriptors");

    free(bigbuffer);
  }

  return;

err:
  free(dev->config);

  dev->config = NULL;
}

