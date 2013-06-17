/*
 * USB Error messages
 *
 * Copyright (c) 2000-2001 Johannes Erdfelt <johannes@erdfelt.com>
 *
 * This library is covered by the LGPL, read LICENSE for details.
 *
 * Changes made by Richard L Walsh:
 * - commented-out the original contents of this file since none of it
 *   is ever used by libusb itself (it is used by the C++ support files
 *   and by some of the tests - if you build them, remove the '#if 0')
 * - added 2 new macros that better identify libusb as the source of
 *   error & debug messages than the existing fprintf()s
 * - added copyright banner
 * Copyright (c) 2006 Richard L Walsh
 *
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#define USB_ERROR_MSG(format, args...) \
  do { \
    if (usb_debug) \
      fprintf(stderr, "LIBUSB: " format "\n", ## args); \
  } while (0)

#define USB_DEBUG_MSG(format, args...) \
  do { \
    if (usb_debug > 1) \
      fprintf(stderr, "LIBUSB: " format "\n", ## args); \
  } while (0)

/***************************************************************************/
#if 0

typedef enum {
  USB_ERROR_TYPE_NONE = 0,
  USB_ERROR_TYPE_STRING,
  USB_ERROR_TYPE_ERRNO,
} usb_error_type_t;

extern char usb_error_str[1024];
extern int usb_error_errno;
extern usb_error_type_t usb_error_type;

#define USB_ERROR(x) \
    do { \
        usb_error_type = USB_ERROR_TYPE_ERRNO; \
        usb_error_errno = x; \
        return x; \
    } while (0)

#define USB_ERROR_STR(x, format, args...) \
    do { \
        usb_error_type = USB_ERROR_TYPE_STRING; \
        snprintf(usb_error_str, sizeof(usb_error_str) - 1, format, ## args); \
        if (usb_debug >= 2) \
            fprintf(stderr, "USB error: %s\n", usb_error_str); \
        return x; \
    } while (0)

#endif /* #if 0 */
/***************************************************************************/

#endif /* _ERROR_H_ */

