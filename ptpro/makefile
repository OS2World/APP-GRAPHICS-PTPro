#****************************************************************************/
#  makefile for PTPro
#
#  - tools used by the author:  gnu make 3.81, gcc 3.3.5, ilink 5.0
#
#****************************************************************************/
#
#  Copyright (C) 2006 Richard L Walsh <rich@e-vertise.com>
# 
#   This file is part of ptpro.
# 
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
# 
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
# 
#
#****************************************************************************/

.SUFFIXES :
.SUFFIXES : .c .o

CC = gcc

CFLAGS = -c -s -Zomf -Wall
LDFLAGS = -s -Zomf -Zexe -Zmap -Zlinker /EXEPACK:2 -Zlinker /PM:VIO

# everything except ptprousb.o
OBJECTS = ptpro.o ptproos2.o ptproparse.o ptprostrings.o

LIBS = ../usbcalls/usbcalls.lib ../libusb/libusb.lib

ptpro.exe : $(OBJECTS) ptprousb.o
	$(CC) -o $@ $(LDFLAGS) $(LIBS) $^

ptprousb.o : ptprousb.c
	$(CC) $(CFLAGS) -I../libusb -o $@ $<

ptpro.o ptproos2.o ptproparse.o ptprostrings.o ptprousb : \
    ptprousb.h ptprocodes.h makefile

ptpro.o : ptpro.h

