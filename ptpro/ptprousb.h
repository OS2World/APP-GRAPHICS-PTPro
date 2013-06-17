/***************************************************************************/
//  ptprousb.h
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

#ifndef _PTPROUSB_H_
#define _PTPROUSB_H_

#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "ptprocodes.h"

#pragma pack(1)

/***************************************************************************/

#ifndef _CAM_ALIASES
#define _CAM_ALIASES
typedef void *  CAMHandle;      // alias for CAMCameraPtr
typedef void *  CAMDevice;      // alias for struct usb_device *
#endif

/***************************************************************************/

typedef struct _CAMUsbInfo {
    CAMDevice   Device;
    char *      BusName;
    uint32_t    DeviceNbr;
    uint32_t    VendorId;
    uint32_t    ProductId;
} CAMUsbInfo;


#define CAM_MAXCAMERAS     4

typedef struct _CAMCameraList {
    uint32_t    cntCameras;
    CAMUsbInfo  Cameras[CAM_MAXCAMERAS];
} CAMCameraList;

typedef CAMCameraList *CAMCameraListPtr;

/***************************************************************************/

typedef struct _CAMDeviceInfo {
    uint32_t    StandardVersion;
    uint32_t    VendorExtensionID;
    uint32_t    VendorExtensionVersion;
    char *      VendorExtensionDesc;
    uint32_t    FunctionalMode;
    uint32_t    cntOperationsSupported;
    uint32_t *  OperationsSupported;
    uint32_t    cntEventsSupported;
    uint32_t *  EventsSupported;
    uint32_t    cntDevicePropertiesSupported;
    uint32_t *  DevicePropertiesSupported;
    uint32_t    cntCaptureFormats;
    uint32_t *  CaptureFormats;
    uint32_t    cntImageFormats;
    uint32_t *  ImageFormats;
    char *      Manufacturer;
    char *      Model;
    char *      DeviceVersion;
    char *      SerialNumber;
} CAMDeviceInfo;

typedef CAMDeviceInfo *CAMDeviceInfoPtr;

/***************************************************************************/

typedef struct _CAMObjectInfo {
    uint32_t    StorageID;
    uint32_t    ObjectFormat;
    uint32_t    ProtectionStatus;
    uint32_t    ObjectCompressedSize;
    uint32_t    ThumbFormat;
    uint32_t    ThumbCompressedSize;
    uint32_t    ThumbPixWidth;
    uint32_t    ThumbPixHeight;
    uint32_t    ImagePixWidth;
    uint32_t    ImagePixHeight;
    uint32_t    ImageBitDepth;
    uint32_t    ParentObject;
    uint32_t    AssociationType;
    uint32_t    AssociationDesc;
    uint32_t    SequenceNumber;
    char *      Filename;
    time_t      CaptureDate;
    time_t      ModificationDate;
    char *      Keywords;
} CAMObjectInfo;

typedef CAMObjectInfo *CAMObjectInfoPtr;

/***************************************************************************/

typedef struct _CAMStorageID {
    uint32_t    cntStorageIDs;
    uint32_t *  StorageIDs;
} CAMStorageID;

typedef CAMStorageID *CAMStorageIDPtr;

/***************************************************************************/

typedef struct _CAMStorageInfo {
    uint32_t    StorageType;
    uint32_t    FilesystemType;
    uint32_t    AccessCapability;
    uint64_t    MaxCapacity;
    uint64_t    FreeSpaceInBytes;
    uint32_t    FreeSpaceInImages;
    char *      StorageDescription;
    char *      VolumeLabel;
} CAMStorageInfo;

typedef CAMStorageInfo *CAMStorageInfoPtr;

/***************************************************************************/

typedef struct _CAMObjectHandle {
    uint32_t    cntHandles;
    uint32_t *  Handles;
} CAMObjectHandle;

typedef CAMObjectHandle *CAMObjectHandlePtr;

/***************************************************************************/

typedef struct _CAMPropertyValue {
    uint32_t    DataType;
    uint32_t    cntValue;
    char *      Value;
} CAMPropertyValue;

typedef CAMPropertyValue *CAMPropertyValuePtr;

/***************************************************************************/

typedef struct _CAMPropertyRange {
    uint32_t *  MinimumValue;
    uint32_t *  MaximumValue;
    uint32_t *  StepSize;
} CAMPropertyRange;

typedef CAMPropertyRange *CAMPropertyRangePtr;

typedef struct _CAMPropertyEnum {
    uint32_t    cntSupportedValues;
    uint32_t *  SupportedValues;
} CAMPropertyEnum;

typedef CAMPropertyEnum *CAMPropertyEnumPtr;

typedef struct _CAMPropertyDesc {
    uint32_t    DevicePropertyCode;
    uint32_t    DataType;
    uint32_t    GetSet;
    uint32_t    cntDefaultValue;
    char *      DefaultValue;
    uint32_t    cntCurrentValue;
    char *      CurrentValue;
    uint32_t    FormFlag;
    union {
        CAMPropertyRange  Range;
        CAMPropertyEnum   Enum;
    } Form;
} CAMPropertyDesc;

typedef CAMPropertyDesc *CAMPropertyDescPtr;

#define CAM_PROP_None                       0
#define CAM_PROP_RangeForm                  1
#define CAM_PROP_EnumForm                   2
#define CAM_PROP_NotSupported               0xff00

#define CAM_PROP_ReadOnly                   0
#define CAM_PROP_ReadWrite                  1

/***************************************************************************/
//  Miscellanea
/***************************************************************************/

#define CAM_STORAGE             1
#define CAM_FILESYSTEM          2
#define CAM_STORAGE_ACCESS      3
#define CAM_ASSOCIATION         4
#define CAM_PROTECTION          5
#define CAM_OBJECT_FORMAT       6
#define CAM_DATATYPE            7

/***************************************************************************/

#define CAM_TYPE_UNDEF                      0x0000
#define CAM_TYPE_INT8                       0x0001
#define CAM_TYPE_UINT8                      0x0002
#define CAM_TYPE_INT16                      0x0003
#define CAM_TYPE_UINT16                     0x0004
#define CAM_TYPE_INT32                      0x0005
#define CAM_TYPE_UINT32                     0x0006
#define CAM_TYPE_INT64                      0x0007
#define CAM_TYPE_UINT64                     0x0008
#define CAM_TYPE_INT128                     0x0009
#define CAM_TYPE_UINT128                    0x000A
#define CAM_TYPE_AINT8                      0x4001
#define CAM_TYPE_AUINT8                     0x4002
#define CAM_TYPE_AINT16                     0x4003
#define CAM_TYPE_AUINT16                    0x4004
#define CAM_TYPE_AINT32                     0x4005
#define CAM_TYPE_AUINT32                    0x4006
#define CAM_TYPE_AINT64                     0x4007
#define CAM_TYPE_AUINT64                    0x4008
#define CAM_TYPE_AINT128                    0x4009
#define CAM_TYPE_AUINT128                   0x400A
#define CAM_TYPE_STR                        0xFFFF

/***************************************************************************/
//  Functions
/***************************************************************************/

// in ptprousb.c

void        SetUsbDebug( uint32_t verbose);
uint32_t    InitUsb( void);
uint32_t    GetCameraList( CAMCameraListPtr* ppList, uint32_t force);
uint32_t    InitCamera( CAMDevice device, CAMHandle* phCam);
void        TermCamera( CAMHandle* phCam);
void        TermUsb( void);
uint32_t    GetObject( CAMHandle hCam, uint32_t handle, char** object);
uint32_t    GetThumb( CAMHandle hCam, uint32_t handle,  char** object);
uint32_t    DeleteObject( CAMHandle hCam, uint32_t handle, uint32_t ofc);
uint32_t    GetDeviceInfo( CAMHandle hCam, CAMDeviceInfoPtr* ppDevInfo);
uint32_t    GetObjectHandles( CAMHandle hCam, uint32_t storage,
                              uint32_t objectformatcode,
                              uint32_t associationOH,
                              CAMObjectHandlePtr* ppObjHandles);
uint32_t    GetObjectInfo( CAMHandle hCam, uint32_t handle,
                           CAMObjectInfoPtr* ppObjInfo);
uint32_t    GetStorageIds( CAMHandle hCam, CAMStorageIDPtr* ppStorageIDs);
uint32_t    GetStorageInfo( CAMHandle hCam, uint32_t storageid,
                            CAMStorageInfoPtr* ppStorageInfo);
uint32_t    GetPropertyDesc( CAMHandle hCam, uint32_t propcode, 
                             CAMPropertyDescPtr* ppPropertyDesc);
uint32_t    GetPropertyValue( CAMHandle hCam,
                              uint32_t propcode, uint32_t datatype,
                              CAMPropertyValuePtr* ppPropertyValue);
uint32_t    SetPropertyValue( CAMHandle hCam, uint32_t propcode,
                              CAMPropertyValuePtr pPropertyValue);
void        ResetCamera( CAMDevice device);
void        ClearStall( CAMHandle hCam);

void        camcli_error( const char* format, ...);
void    	camcli_debug( const char* format, ...);

// in camstrings.c

const char* GetCodeName( uint32_t code, int type);
const char* GetErrorName( CAMDeviceInfoPtr pDevInfo, uint32_t code);
const char* GetOperationName( CAMDeviceInfoPtr pDevInfo, uint32_t code);
const char* GetPropertyName( CAMDeviceInfoPtr pDevInfo, uint32_t code);

// in camdata.c

uint32_t    Build_CamDeviceInfo( void * pUnpack, CAMDeviceInfoPtr* ppCam);
uint32_t    Build_CamObjectInfo( void * pUnpack, CAMObjectInfoPtr* ppCam);
uint32_t    Build_CamStorageID( void * pUnpack, CAMStorageIDPtr* ppCam);
uint32_t    Build_CamStorageInfo( void * pUnpack, CAMStorageInfoPtr* ppCam);
uint32_t    Build_CamObjectHandle( void * pUnpack, CAMObjectHandlePtr* ppCam);
uint32_t    Build_CamPropertyDesc( void * pUnpack, CAMPropertyDescPtr* ppCam);
uint32_t    Build_CamPropertyValue( void * pUnpack, uint32_t type,
                                    CAMPropertyValuePtr* ppCam);
uint32_t    Build_RawPropertyValue( CAMPropertyValuePtr pCam,
                                    char ** ppPack, uint32_t* pSize);

/***************************************************************************/

#pragma pack()

#endif  //_PTPROUSB_H_

/***************************************************************************/

