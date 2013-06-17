/***************************************************************************/
//  ptprousb.c
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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "ptprousb.h"
#include "usb.h"

uint32_t _System DosSleep( uint32_t);

#pragma pack(1)

/***************************************************************************/
//  Miscellaneous macros
/***************************************************************************/

#define CAM_TIMEOUT                 4000

#define USB_CLASS_IMAGE             6

// still-image class-specific requests
#define USB_REQ_DEVICE_RESET        0x66
#define USB_REQ_GET_DEVICE_STATUS   0x67

#define USB_FEATURE_ENDPOINT_HALT   0x00

#define USB_HOST_TO_DEVICE          (0x00 << 7)
#define USB_DEVICE_TO_HOST          (0x01 << 7)

/***************************************************************************/

// generic request
#define CAM_GetStatus(HANDLE, EP, STATUS) \
    usb_control_msg(HANDLE, (USB_DEVICE_TO_HOST|USB_RECIP_ENDPOINT), \
                    USB_REQ_GET_STATUS, 0, EP, \
                    (char*)(&STATUS), sizeof(STATUS), CAM_TIMEOUT)

// generic request
#define CAM_ClearEpStall(HANDLE,EP) \
    usb_control_msg(HANDLE, USB_RECIP_ENDPOINT, USB_REQ_CLEAR_FEATURE, \
                    USB_FEATURE_ENDPOINT_HALT, EP, 0, 0, CAM_TIMEOUT)

// class-specific request
#define CAM_GetDeviceStatus(HANDLE, STATUS) \
    usb_control_msg(HANDLE, \
                    (USB_TYPE_CLASS|USB_DEVICE_TO_HOST|USB_RECIP_INTERFACE), \
                    USB_REQ_GET_DEVICE_STATUS, 0, 0, (char*)(&STATUS), \
                    (sizeof(STATUS)), CAM_TIMEOUT)

/***************************************************************************/
//  USB Device info structure
/***************************************************************************/

// created by InitCamera() to hold USB data;
// referenced externally as CAMHandle, a void *

typedef struct _CAMCamera {
    struct usb_device * device;
    usb_dev_handle *    handle;
    int                 epIn;
    int                 epOut;
    int                 epInt;
    uint32_t            maxInPkt;
    uint32_t            maxOutPkt;
    uint32_t            session;
    uint32_t            transaction;
} CAMCamera;

typedef CAMCamera *CAMCameraPtr;

/***************************************************************************/
//  Container structures & macros
/***************************************************************************/

// types of containers used to communicate with camera
#define CAMCNRTYPE_UNDEFINED    0
#define CAMCNRTYPE_COMMAND      1
#define CAMCNRTYPE_DATA         2
#define CAMCNRTYPE_RESPONSE     3
#define CAMCNRTYPE_EVENT        4

// some size macros
#define CAMCNR_MAXSIZE          512
#define CAMCNR_HDRSIZE          (sizeof(CAMCnrHdr))
#define CAMCNR_DATASIZE         (sizeof(CAMDataCnr) - CAMCNR_HDRSIZE)
#define CAMCNR_CALCSIZE(x)      (CAMCNR_HDRSIZE + ((x) * sizeof(uint32_t)))

/***************************************************************************/

// used solely to get the length of the header on the other containers
typedef struct _CAMCnrHdr {
    uint32_t    length;
    uint16_t    type;
    uint16_t    opcode;
    uint32_t    transaction;
} CAMCnrHdr;

// passed to WriteCommand() & ReadResponse() to hold control info;
// the 'dummmy' member ensures the struct is not a multiple of any
// standard USB packet size (needed by ReadResponse())
typedef struct _CAMCmdCnr {
    uint32_t    length;
    uint16_t    type;
    uint16_t    opcode;
    uint32_t    transaction;
    uint32_t    param1;
    uint32_t    param2;
    uint32_t    param3;
    uint32_t    param4;
    uint32_t    param5;
    uint32_t    dummy;
} CAMCmdCnr;

typedef CAMCmdCnr *CAMCmdCnrPtr;

// used internally by ReadData() & WriteData() to get & send data
typedef struct _CAMDataCnr {
    uint32_t    length;
    uint16_t    type;
    uint16_t    opcode;
    uint32_t    transaction;
    char        data[CAMCNR_MAXSIZE - CAMCNR_HDRSIZE];
} CAMDataCnr;

/***************************************************************************/
//  Internal functions
/***************************************************************************/

static uint32_t InitCameraInternal( CAMDevice device, CAMHandle* phCam,
                                    uint32_t opensession);
static void     GetEndpoints( CAMCameraPtr pCam);

static uint32_t OpenSession( CAMCameraPtr pCam, uint32_t session);
static uint32_t CloseSession( CAMCameraPtr pCam);

static uint32_t WriteCommand( CAMCameraPtr pCam, CAMCmdCnrPtr pCnr,
                              uint16_t opcode);
static uint32_t ReadData( CAMCameraPtr pCam, uint16_t opcode, char **data);
static uint32_t WriteData( CAMCameraPtr pCam, uint16_t opcode, char* data,
                           uint32_t size);
static uint32_t ReadResponse( CAMCameraPtr pCam, CAMCmdCnrPtr pCnr,
                              uint16_t opcode);

/***************************************************************************/
//  Static data
/***************************************************************************/

static uint32_t     verbosity = 0;

/***************************************************************************/
//  Init / Term
/***************************************************************************/

void        SetUsbDebug( uint32_t verbose)
{
    verbosity = verbose;
    usb_set_debug( verbose);
    return;
}

/***************************************************************************/

uint32_t    InitUsb( void)
{
    uint32_t    rtn = 0;

    if (usb_init() < 0)
        rtn = CAMERR_USBINIT;
    else
    if (usb_find_busses() < 0 || usb_find_devices() < 0)
        rtn = CAMERR_USBENUM;

    return rtn;
}

/***************************************************************************/

uint32_t    GetCameraList( CAMCameraListPtr* ppList, uint32_t force)
{
    uint32_t            rtn = 0;
    struct usb_bus *    bus;
    struct usb_device * dev;
    CAMUsbInfo *        ptr;
    uint32_t *          pCnt;

    *ppList = (CAMCameraListPtr)malloc( sizeof(CAMCameraList));
    if (!*ppList)
        return CAMERR_MALLOC;

    memset( *ppList, 0, sizeof(CAMCameraList));
    pCnt = &(*ppList)->cntCameras;
    ptr = (*ppList)->Cameras;

    for (bus = usb_get_busses(); bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {

            if (dev->descriptor.bDeviceClass == USB_CLASS_HUB)
                continue;

            if (dev->config->interface->altsetting->bInterfaceClass ==
                USB_CLASS_IMAGE || force) {
                ptr->Device = dev;
                ptr->BusName = bus->dirname;
                ptr->DeviceNbr = strtoul( dev->filename, 0, 10);
                ptr->VendorId = dev->descriptor.idVendor;
                ptr->ProductId = dev->descriptor.idProduct;
                ptr++;
                (*pCnt)++;

                if (*pCnt >= CAM_MAXCAMERAS)
                    break;
            }
        }
        if (*pCnt >= CAM_MAXCAMERAS)
            break;
    }

    if (*pCnt == 0) {
        rtn = CAMERR_NOCAMERAS;
        free( *ppList);
        *ppList = 0;
    }

    return rtn;
}

/***************************************************************************/

uint32_t    InitCamera( CAMDevice device, CAMHandle* phCam)
{
    return (InitCameraInternal( device, phCam, 1));
}

/***************************************************************************/

static uint32_t InitCameraInternal( CAMDevice device, CAMHandle* phCam,
                                    uint32_t opensession)
{
    uint32_t            rtn = 0;
    CAMCameraPtr        pCam;
    struct usb_device * dev = (struct usb_device*)device;

do {
    *phCam = 0;

    pCam = malloc( sizeof(CAMCamera));
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, sizeof(CAMCamera));

    pCam->device = dev;
    pCam->handle = usb_open( dev);
    if (!pCam->handle) {
        rtn = CAMERR_OPENDEVICE;
        break;
    }

    GetEndpoints( pCam);
    usb_claim_interface( pCam->handle,
                         dev->config->interface->altsetting->bInterfaceNumber);

    usb_set_configuration( pCam->handle, 1);

    if (opensession)
        if (OpenSession( pCam, 1)) {
            rtn = CAMERR_OPENSESSION;
            break;
        }

} while (0);

    *phCam = pCam;

    return rtn;
}

/***************************************************************************/

void    GetEndpoints( CAMCameraPtr pCam)
{
    uint32_t    ctr;
    uint32_t    cnt;
    struct usb_endpoint_descriptor *ep;

    ep = pCam->device->config->interface->altsetting->endpoint;
    cnt = pCam->device->config->interface->altsetting->bNumEndpoints;

    for (ctr = 0; ctr < cnt; ctr++) {
        if (ep[ctr].bmAttributes == USB_ENDPOINT_TYPE_BULK) {
            if ((ep[ctr].bEndpointAddress & USB_ENDPOINT_DIR_MASK) ==
                                            USB_ENDPOINT_DIR_MASK) {
                pCam->epIn = ep[ctr].bEndpointAddress;
                pCam->maxInPkt = ep[ctr].wMaxPacketSize;
            }
            else {
                pCam->epOut = ep[ctr].bEndpointAddress;
                pCam->maxOutPkt = ep[ctr].wMaxPacketSize;
            }
        }
        else
        if (ep[ctr].bmAttributes == USB_ENDPOINT_TYPE_INTERRUPT &&
            (ep[ctr].bEndpointAddress & USB_ENDPOINT_DIR_MASK) ==
                                        USB_ENDPOINT_DIR_MASK)
            pCam->epInt = ep[ctr].bEndpointAddress;
    }

    return;
}

/***************************************************************************/

void    TermCamera( CAMHandle * phCam)
{
    CAMCameraPtr pCam = (CAMCameraPtr)*phCam;

    if (pCam) {
        if (pCam->handle) {
            if (pCam->session)
                if (CloseSession( pCam))
                    camcli_debug( "Could not close session.");

            ClearStall( pCam);
            usb_release_interface( pCam->handle,
                pCam->device->config->interface->altsetting->bInterfaceNumber);
            usb_close( pCam->handle);
        }
        free( pCam);
        *phCam = 0;
    }

    return;
}

/***************************************************************************/

void    TermUsb( void)
{
    usb_term();
    return;
}

/***************************************************************************/
//  Camera Operations
/***************************************************************************/

static uint32_t OpenSession( CAMCameraPtr pCam, uint32_t session)
{
    uint32_t        rtn;
    uint16_t        opcode;
    CAMCmdCnr       cnr;

    pCam->session = 0;
    pCam->transaction = 0;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_OpenSession;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = session;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK) {
            pCam->session = session;
            rtn = 0;
    }

    return rtn;
}

/***************************************************************************/

static uint32_t CloseSession( CAMCameraPtr pCam)
{
    uint32_t        rtn;
    uint16_t        opcode;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    cnr.length = CAMCNR_CALCSIZE(1);
    opcode     = PTP_OC_CloseSession;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK) {
            pCam->session = 0;
            rtn = 0;
    }

    return rtn;
}

/***************************************************************************/

uint32_t    GetObject( CAMHandle hCam, uint32_t handle, char** object)
{
    uint32_t        rtn;
    uint16_t        opcode;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetObject;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = handle;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, object))   == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK &&
        *object)
            rtn = 0;

    return rtn;
}

/***************************************************************************/

uint32_t    GetThumb( CAMHandle hCam, uint32_t handle, char** object)
{
    uint32_t        rtn;
    uint16_t        opcode;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetThumb;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = handle;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, object))   == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK &&
        *object)
            rtn = 0;

    return rtn;
}

/***************************************************************************/

uint32_t 	DeleteObject( CAMHandle hCam, uint32_t handle, uint32_t ofc)
{
    uint32_t        rtn;
    uint16_t        opcode;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_DeleteObject;
    cnr.length = CAMCNR_CALCSIZE(2);
    cnr.param1 = handle;
    cnr.param2 = ofc;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = 0;

    return rtn;
}

/***************************************************************************/

uint32_t    GetDeviceInfo( CAMHandle hCam, CAMDeviceInfoPtr* ppDevInfo)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    cnr.length = CAMCNR_CALCSIZE(0);
    opcode     = PTP_OC_GetDeviceInfo;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamDeviceInfo( data, ppDevInfo);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    GetObjectHandles( CAMHandle hCam, uint32_t storage,
                              uint32_t objectformatcode,
                              uint32_t associationOH,
                              CAMObjectHandlePtr* ppObjHandles)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetObjectHandles;
    cnr.length = CAMCNR_CALCSIZE(3);
    cnr.param1 = storage;
    cnr.param2 = objectformatcode;
    cnr.param3 = associationOH;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamObjectHandle( data, ppObjHandles);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    GetObjectInfo( CAMHandle hCam, uint32_t handle,
                           CAMObjectInfoPtr* ppObjInfo)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetObjectInfo;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = handle;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamObjectInfo( data, ppObjInfo);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    GetStorageIds( CAMHandle hCam, CAMStorageIDPtr* ppStorageIDs)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetStorageIDs;
    cnr.length = CAMCNR_CALCSIZE(0);

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamStorageID( data, ppStorageIDs);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    GetStorageInfo( CAMHandle hCam, uint32_t storageid,
                            CAMStorageInfoPtr* ppStorageInfo)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetStorageInfo;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = storageid;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamStorageInfo( data, ppStorageInfo);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    GetPropertyDesc( CAMHandle hCam, uint32_t propcode, 
                             CAMPropertyDescPtr* ppPropertyDesc)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetDevicePropDesc;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = propcode;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamPropertyDesc( data, ppPropertyDesc);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    GetPropertyValue( CAMHandle hCam,
                              uint32_t propcode, uint32_t datatype,
                              CAMPropertyValuePtr* ppPropertyValue)
{
    uint32_t        rtn;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_GetDevicePropValue;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = propcode;

    if ((rtn = WriteCommand( pCam, &cnr, opcode)) == PTP_RC_OK &&
        (rtn = ReadData( pCam, opcode, &data))    == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode)) == PTP_RC_OK)
            rtn = Build_CamPropertyValue( data, datatype, ppPropertyValue);

    free( data);
    return rtn;
}

/***************************************************************************/

uint32_t    SetPropertyValue( CAMHandle hCam, uint32_t propcode,
                              CAMPropertyValuePtr pPropertyValue)
{
    uint32_t        rtn;
    uint32_t        size;
    uint16_t        opcode;
    char *          data = 0;
    CAMCameraPtr    pCam = (CAMCameraPtr)hCam;
    CAMCmdCnr       cnr;

    memset( &cnr, 0, sizeof(CAMCmdCnr));
    opcode     = PTP_OC_SetDevicePropValue;
    cnr.length = CAMCNR_CALCSIZE(1);
    cnr.param1 = propcode;

    if ((rtn = Build_RawPropertyValue( pPropertyValue, &data, &size)) == 0 &&
        (rtn = WriteCommand( pCam, &cnr, opcode))    == PTP_RC_OK &&
        (rtn = WriteData( pCam, opcode, data, size)) == PTP_RC_OK &&
        (rtn = ReadResponse( pCam, &cnr, opcode))    == PTP_RC_OK)
            rtn = 0;

    free( data);
    return rtn;
}

/***************************************************************************/
//  USB Device commands
/***************************************************************************/

void        ResetCamera( CAMDevice device)
{
    uint16_t         status[2] = {0,0};
    int              ret;
    CAMHandle        hCam = 0;
    usb_dev_handle * handle;

do {
    if (InitCameraInternal( device, &hCam, 0))
        break;

    handle = ((CAMCameraPtr)hCam)->handle;

    ret = CAM_GetDeviceStatus( handle, status);
    camcli_debug( "first get_device_status returned %x", ret);

    ClearStall( hCam);

    ret = CAM_GetDeviceStatus( handle, status);
    if (ret < 0) 
        camcli_error( "Unable to get device status");
    else    {
        if (status[1] == PTP_RC_OK) 
            printf( "Device status OK\n");
        else
            camcli_error( "Device status 0x%04X", status[1]);
    }
    
    ret = usb_control_msg( handle, (USB_HOST_TO_DEVICE |
                           USB_TYPE_CLASS | USB_RECIP_INTERFACE),
                           USB_REQ_DEVICE_RESET, 0, 0,
                           0, 0, CAM_TIMEOUT);
    if (ret < 0)
        camcli_error( "Unable to reset device");

    ret = CAM_GetDeviceStatus( handle, status);
    camcli_debug( "second get_device_status returned %x", ret);

} while (0);

    TermCamera( &hCam);
    return;
}

/***************************************************************************/

void    ClearStall( CAMHandle hCam)
{
    uint16_t     status;
    int          ret;
    CAMCameraPtr pCam = (CAMCameraPtr)hCam;

    // check the in endpoint status
    if (pCam->epIn) {
        status = 0;
        ret = CAM_GetStatus( pCam->handle, pCam->epIn, status);

        if (ret >= 0 && status)
            ret = CAM_ClearEpStall( pCam->handle, pCam->epIn);

        if (ret < 0)
            camcli_error( "Unable to reset input pipe.");
    }
    
    // check the out endpoint status
    if (pCam->epOut) {
        status = 0;
        ret = CAM_GetStatus( pCam->handle, pCam->epOut, status);

        if (ret >= 0 && status)
            ret = CAM_ClearEpStall( pCam->handle, pCam->epOut);

        if (ret < 0)
            camcli_error( "Unable to reset output pipe.");
    }
    
    // check the interrupt endpoint status
    if (pCam->epInt) {
        status = 0;
        ret = CAM_GetStatus( pCam->handle, pCam->epInt, status);

        if (ret >= 0 && status)
            ret = CAM_ClearEpStall( pCam->handle, pCam->epInt);

        if (ret < 0)
            camcli_error( "Unable to reset interrupt pipe.");
    }

    return;
}

/***************************************************************************/
//  Camera I/O functions
/***************************************************************************/

static uint32_t WriteCommand( CAMCameraPtr pCam, CAMCmdCnrPtr pCnr,
                              uint16_t opcode)
{
    int         result;
    uint32_t    rtn = PTP_RC_OK;

    pCnr->type        = CAMCNRTYPE_COMMAND;
    pCnr->opcode      = opcode;
    pCnr->transaction = ++(pCam->transaction);

    result = usb_bulk_write( pCam->handle, pCam->epOut, (char*)pCnr,
                             pCnr->length, CAM_TIMEOUT);
    if (result <= 0) {
        camcli_debug( "USB write command failed - code= 0x%04x  rc= 0x%04x",
                     opcode, result);
        rtn = CAMERR_USBFAILURE;
    }

    return rtn;
}

/***************************************************************************/

static uint32_t ReadData( CAMCameraPtr pCam, uint16_t opcode, char **data)
{
    uint32_t    rtn = PTP_RC_OK;
    uint32_t    cnt;
    uint32_t    tot;
    CAMDataCnr  cnr;

do {
    memset( &cnr, 0, sizeof(CAMDataCnr));

    if (usb_bulk_read( pCam->handle, pCam->epIn, (char*)&cnr,
                       sizeof(CAMDataCnr), CAM_TIMEOUT) <= 0) {
        rtn = CAMERR_USBFAILURE;
        break;
    }

    if (cnr.type != CAMCNRTYPE_DATA) {
        rtn = CAMERR_NODATA;
        break;
    }

    if (cnr.opcode != opcode) {
        rtn = cnr.opcode;
        if (rtn != PTP_RC_OK)
            break;
    }

    tot = cnr.length - CAMCNR_HDRSIZE;
    if (*data == 0) {
        *data = malloc( tot);
        memset( *data, 0, tot);
    }

    cnt = (tot > CAMCNR_DATASIZE) ? CAMCNR_DATASIZE : tot;
    tot -= cnt;
    memcpy( *data, cnr.data, cnt);

    if (cnr.length % pCam->maxInPkt == 0) {
        camcli_debug( "ReadData:  forcing short or null packet - tot= %d  maxPkt= %d",
                (int)tot, (int)pCam->maxInPkt);
        tot++;
    }

    if (tot == 0)
        break;

    if (usb_bulk_read( pCam->handle, pCam->epIn, &(*data)[cnt],
                       tot, CAM_TIMEOUT) < 0) {
        rtn = CAMERR_USBFAILURE;
        break;
    }

} while (0);

    if (rtn != PTP_RC_OK) {
        camcli_debug( "USB read data failed - code= 0x%04x  rc= 0x%04x",
                     opcode, rtn);
        rtn = CAMERR_USBFAILURE;
    }

    return rtn;
}

/***************************************************************************/

static uint32_t WriteData( CAMCameraPtr pCam, uint16_t opcode, char* data,
                           uint32_t size)
{
    uint32_t    rtn = PTP_RC_OK;
    uint32_t    cnt;
    CAMDataCnr  cnr;

do {
    cnr.length      = CAMCNR_HDRSIZE + size;
    cnr.type        = CAMCNRTYPE_DATA;
    cnr.opcode      = opcode;
    cnr.transaction = pCam->transaction;

    cnt = (size > CAMCNR_DATASIZE) ? CAMCNR_DATASIZE : size;
    size -= cnt;
    memcpy( cnr.data, data, cnt);

    if (usb_bulk_write( pCam->handle, pCam->epOut, (char*)&cnr,
                        cnt+CAMCNR_HDRSIZE, CAM_TIMEOUT) <= 0) {
        rtn = CAMERR_USBFAILURE;
        break;
    }

    if (size == 0)
        break;

    if (usb_bulk_write( pCam->handle, pCam->epOut, &data[cnt],
                        size, CAM_TIMEOUT) <= 0) {
        rtn = CAMERR_USBFAILURE;
        break;
    }

} while (0);

    if (rtn != PTP_RC_OK) {
        camcli_debug( "USB write data failed - code= 0x%04x  rc= 0x%04x",
                     opcode, rtn);
        rtn = CAMERR_USBFAILURE;
    }

    return rtn;
}

/***************************************************************************/

static uint32_t ReadResponse( CAMCameraPtr pCam, CAMCmdCnrPtr pCnr,
                              uint16_t opcode)
{
    uint32_t    rtn = PTP_RC_OK;

    memset( pCnr, 0, sizeof(CAMCmdCnr));

    if (usb_bulk_read( pCam->handle, pCam->epIn, (char*)pCnr,
                       sizeof(CAMCmdCnr), CAM_TIMEOUT) <= 0)
        rtn = CAMERR_USBFAILURE;
    else
        if (pCnr->type != CAMCNRTYPE_RESPONSE)
            rtn = CAMERR_NORESPONSE;
        else
            if (pCnr->opcode != opcode)
                rtn = pCnr->opcode;

    if (rtn != PTP_RC_OK) {
        camcli_debug( "USB read response failed - code= 0x%04x  rc= 0x%04x",
                      opcode, rtn);
        rtn = CAMERR_USBFAILURE;
    }

    return rtn;
}

/***************************************************************************/
//  Error & Debug Messages
/***************************************************************************/

void camcli_error( const char* format, ...)
{
    va_list args;
    char    buf[256];

    strcpy( buf, "ERROR: ");
    strcat( buf, format);
    strcat( buf, "\n");

    va_start( args, format);
    vfprintf( stderr, buf, args);
    fflush( stderr);
    va_end (args);
}

/***************************************************************************/

void camcli_debug( const char* format, ...)
{
    va_list args;
    char    buf[256];

    if (verbosity < 2)
        return;

    strcpy( buf, "PTPRO: ");
    strcat( buf, format);
    strcat( buf, "\n");

    va_start( args, format);
    vfprintf( stderr, buf, args);
    fflush( stderr);
    va_end (args);
}

/***************************************************************************/

#pragma pack()

/***************************************************************************/

