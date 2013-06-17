/***************************************************************************/
// ptpro.h
/***************************************************************************/
/*
 * Copyright (C) 2006 Richard L Walsh <rich@e-vertise.com>
 *
 *  This file is part of ptpro.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */
/***************************************************************************/

#ifndef _PTPRO_H_
#define _PTPRO_H_

#pragma pack(1)

/***************************************************************************/

//  commandline option flags
#define OPT_FORCE               1
#define OPT_REPLACE             2
#define OPT_THUMBNAIL           4

//  requested actions
#define ACT_DEVICE_RESET        0x01
#define ACT_LIST_DEVICES        0x02
#define ACT_LIST_PROPERTIES     0x03
#define ACT_LIST_OPERATIONS     0x04
#define ACT_GETSET_PROPERTY     0x05
#define ACT_SHOW_INFO           0x06
#define ACT_LIST_FILES          0x07
#define ACT_GET_FILE            0x08
#define ACT_DELETE_FILE         0x09
#define ACT_HANDLE_INFO         0x0A
#define ACT_STORAGE_INFO        0x0B
#define ACT_LIST_HANDLES        0x0C

/***************************************************************************/

//  structures

typedef struct _OPTS {
    uint32_t    dev;
    uint32_t    flags;
    uint32_t    first;
    uint32_t    last;
    uint32_t    property;
    char *      value;
} OPTS;

typedef struct _FNSTRUCT {
    uint32_t    namePos;
    uint32_t    nbrPos;
    uint32_t    extPos;
    uint32_t    seqNbr;
    char        format[260];
} FNSTRUCT;

/***************************************************************************/

//  functions

// in ptpro.c
uint32_t    parse_options( OPTS * pOpts, int argc, char ** argv);
int         device_from_arg( char* arg, uint32_t* dev);
int         range_from_arg( char* arg, uint32_t* first, uint32_t* last);
CAMDevice   SelectCamera( OPTS * pOpts, CAMCameraListPtr pList);
void        help();
char *      underline( const char *str);
void        list_devices( CAMCameraListPtr pList);
void        show_info( CAMDeviceInfoPtr pDevInfo);
void        list_operations( CAMDeviceInfoPtr pDevInfo);
void        show_storage_info( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo);
void        list_properties( CAMDeviceInfoPtr pDevInfo);
void        getset_property( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo,
                             OPTS *pOpts);
void        print_propval( uint32_t datatype, uint32_t cntValue,
                           void* value, uint32_t dec);
uint32_t    set_property( CAMHandle hCam, uint32_t property,
                          char* value, uint32_t datatype);
uint32_t    validate_property( CAMDeviceInfoPtr pDevInfo, uint32_t property);
void        list_handles( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo);
void        list_files( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo);
void        show_handle_info( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo,
                              OPTS *pOpts);
int         validate_range( CAMObjectHandlePtr pHandles, OPTS *pOpts,
                            uint32_t **ppFirst, uint32_t **ppLast);
void        delete_files( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo,
                          OPTS *pOpts);
void        get_files( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo, OPTS *pOpts);
int         parse_filename( char *filename, FNSTRUCT *fns);
void        make_filename( char* pOut, char* pIn, FNSTRUCT *fns);
void        camcli_error( const char* format, ...);
void        camcli_debug( const char* format, ...);

// in ptproos2.c
uint32_t    os2_get_file( CAMHandle hCam, CAMObjectInfoPtr pObjInfo,
                          uint32_t handle, char* filename, int replace,
                          int thumb);

/***************************************************************************/

#pragma pack()

#endif  //_PTPRO_H_

/***************************************************************************/

