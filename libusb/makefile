#****************************************************************************/
#  makefile for libusb
#
#  - tools used by the author:  gnu make 3.81, gcc 3.3.5, emxomfar 0.6.1
#
#****************************************************************************/

.SUFFIXES :
.SUFFIXES : .c .o

CC = gcc
AR = emxomfar

CFLAGS = -c -s -Zomf -Wall -I../usbcalls

libusb.lib : descriptors.o error.o os2.o usb.o
	$(AR) r $@ $^

descriptors.o error.o os2.o usb.o : \
    usb.h usbi.h error.h ../usbcalls/usbcalls.h makefile

