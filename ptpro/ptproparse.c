/***************************************************************************/
//  ptproparse.c
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
#include "ptprousb.h"

#pragma pack(1)

/***************************************************************************/

typedef struct _RAWObjectInfo {
    uint32_t    StorageID;
    uint16_t    ObjectFormat;
    uint16_t    ProtectionStatus;
    uint32_t    ObjectCompressedSize;
    uint16_t    ThumbFormat;
    uint32_t    ThumbCompressedSize;
    uint32_t    ThumbPixWidth;
    uint32_t    ThumbPixHeight;
    uint32_t    ImagePixWidth;
    uint32_t    ImagePixHeight;
    uint32_t    ImageBitDepth;
    uint32_t    ParentObject;
    uint16_t    AssociationType;
    uint32_t    AssociationDesc;
    uint32_t    SequenceNumber;
    char        Filename[1];
    char        CaptureDate[1];
    char        ModificationDate[1];
    char        Keywords[1];
} RAWObjectInfo;

typedef RAWObjectInfo *RAWObjectInfoPtr;

/***************************************************************************/

typedef struct _RAWDeviceInfo {
    uint16_t    StandardVersion;
    uint32_t    VendorExtensionID;
    uint16_t    VendorExtensionVersion;
    char        VendorExtensionDesc[1];
    uint16_t    FunctionalMode;
    uint32_t    OperationsSupported;
    uint32_t    EventsSupported;
    uint32_t    DevicePropertiesSupported;
    uint32_t    CaptureFormats;
    uint32_t    ImageFormats;
    char        Manufacturer[1];
    char        Model[1];
    char        DeviceVersion[1];
    char        SerialNumber[1];
} RAWDeviceInfo;

typedef RAWDeviceInfo * RAWDeviceInfoPtr;

/***************************************************************************/

typedef struct _RAWStorageInfo {
    uint16_t    StorageType;
    uint16_t    FilesystemType;
    uint16_t    AccessCapability;
    uint64_t    MaxCapacity;
    uint64_t    FreeSpaceInBytes;
    uint32_t    FreeSpaceInImages;
    char        StorageDescription[1];
    char        VolumeLabel[1];
} RAWStorageInfo;

typedef RAWStorageInfo *RAWStorageInfoPtr;

/***************************************************************************/

typedef struct _RAWObjectHandle {
    uint32_t    Handles;
} RAWObjectHandle;

typedef RAWObjectHandle *RAWObjectHandlePtr;

/***************************************************************************/

typedef struct _RAWStorageID {
    uint32_t    StorageIDs;
} RAWStorageID;

typedef RAWStorageID *RAWStorageIDPtr;

/***************************************************************************/

typedef struct _RAWPropertyDesc {
    uint16_t    DevicePropertyCode;
    uint16_t    DataType;
    uint8_t     GetSet;
    char        DefaultValue[1];
    char        CurrentValue[1];
    uint8_t     FormFlag;
    char        Form[1];
} RAWPropertyDesc;

typedef RAWPropertyDesc *RAWPropertyDescPtr;

/***************************************************************************/

static uint32_t     CalcCamSize( uint32_t type, char ** pptr);
static uint32_t     MoveCamData( uint32_t type, char ** pptr, char ** ppData);
static time_t       MoveCamDateTime( char ** pptr);

static uint32_t     CalcRawSize( uint32_t type, uint32_t cnt, char * pCam);
static void         MoveRawData( uint32_t type, uint32_t cnt,
                                 char * pCam, char ** ppRaw);

/***************************************************************************/

uint32_t    Build_CamDeviceInfo( void * pUnpack, CAMDeviceInfoPtr* ppCam)
{
    uint32_t    tot;
    uint32_t    ctr;
    char *      ptr;
    char *      pData;
    uint32_t *  pMember;
    CAMDeviceInfoPtr pCam;
    RAWDeviceInfoPtr pRaw = (RAWDeviceInfoPtr)pUnpack;

    // calculate the aggregate size of the structure, arrays, & strings
    tot = sizeof(CAMDeviceInfo);

    // get the size of the first string
    // then skip over FunctionalMode
    ptr = &pRaw->VendorExtensionDesc[0];
    tot += CalcCamSize( CAM_TYPE_STR, &ptr);
    ptr += sizeof(uint16_t);

    // get the sizes of the 5 arrays of datacodes
    for (ctr = 0; ctr < 5; ctr++)
        tot += CalcCamSize( CAM_TYPE_AUINT16, &ptr);

    // get the sizes of the 4 unicode strings
    for (ctr = 0; ctr < 4; ctr++)
        tot += CalcCamSize( CAM_TYPE_STR, &ptr);

    // now that we have the combined size of everything, allocate it,
    // clear it, then set pData to the first byte after the structure
    pCam = (CAMDeviceInfoPtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMDeviceInfo);

    // lay in the data
    pCam->StandardVersion           = pRaw->StandardVersion;
    pCam->VendorExtensionID         = pRaw->VendorExtensionID;
    pCam->VendorExtensionVersion    = pRaw->VendorExtensionVersion;

    ptr = &pRaw->VendorExtensionDesc[0];

    pCam->VendorExtensionDesc = pData;
    if (!MoveCamData( CAM_TYPE_STR, &ptr, &pData))
        pCam->VendorExtensionDesc = 0;

    pCam->FunctionalMode = *((uint16_t*)ptr);
    ptr += sizeof(uint16_t);

    // copy the 5 arrays of datacodes into 5 32-bit arrays
    pMember = &pCam->cntOperationsSupported;
    for (ctr = 0; ctr < 5; ctr++, pMember += 2) {
        pMember[1] = (uint32_t)pData;
        pMember[0] = MoveCamData( CAM_TYPE_AUINT16, &ptr, &pData);
        if (!pMember[0])
            pMember[1] = 0;
    }

    // copy the 4 unicode strings
    pMember = (uint32_t*)&pCam->Manufacturer;
    for (ctr = 0; ctr < 4; ctr++, pMember++) {
        *pMember = (uint32_t)pData;
        if (!MoveCamData( CAM_TYPE_STR, &ptr, &pData))
            *pMember = 0;
    }

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/

uint32_t    Build_CamObjectInfo( void * pUnpack, CAMObjectInfoPtr* ppCam)
{
    uint32_t    tot;
    char *      ptr;
    char *      pData;
    CAMObjectInfoPtr pCam;
    RAWObjectInfoPtr pRaw = (RAWObjectInfoPtr)pUnpack;

    tot = sizeof(CAMObjectInfo);
    ptr = &pRaw->Filename[0];

    // for filename & keywords, calc additional space needed;
    // for capture date & mod date, just advance ptr past them
    tot += CalcCamSize( CAM_TYPE_STR, &ptr);
    CalcCamSize( CAM_TYPE_STR, &ptr);
    CalcCamSize( CAM_TYPE_STR, &ptr);
    tot += CalcCamSize( CAM_TYPE_STR, &ptr);

    pCam = (CAMObjectInfoPtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMObjectInfo);

    pCam->StorageID            = pRaw->StorageID;
    pCam->ObjectFormat         = pRaw->ObjectFormat;
    pCam->ProtectionStatus     = pRaw->ProtectionStatus;
    pCam->ObjectCompressedSize = pRaw->ObjectCompressedSize;
    pCam->ThumbFormat          = pRaw->ThumbFormat;
    pCam->ThumbCompressedSize  = pRaw->ThumbCompressedSize;
    pCam->ThumbPixWidth        = pRaw->ThumbPixWidth;
    pCam->ThumbPixHeight       = pRaw->ThumbPixHeight;
    pCam->ImagePixWidth        = pRaw->ImagePixWidth;
    pCam->ImagePixHeight       = pRaw->ImagePixHeight;
    pCam->ImageBitDepth        = pRaw->ImageBitDepth;
    pCam->ParentObject         = pRaw->ParentObject;
    pCam->AssociationType      = pRaw->AssociationType;
    pCam->AssociationDesc      = pRaw->AssociationDesc;
    pCam->SequenceNumber       = pRaw->SequenceNumber;

    ptr = &pRaw->Filename[0];

    pCam->Filename = pData;
    if (!MoveCamData( CAM_TYPE_STR, &ptr, &pData))
        pCam->Filename = 0;

    pCam->CaptureDate      = MoveCamDateTime( &ptr);
    pCam->ModificationDate = MoveCamDateTime( &ptr);

    pCam->Keywords = pData;
    if (!MoveCamData( CAM_TYPE_STR, &ptr, &pData))
        pCam->Keywords = 0;

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/

uint32_t    Build_CamStorageID( void * pUnpack, CAMStorageIDPtr* ppCam)
{
    uint32_t    tot;
    char *      ptr;
    char *      pData;
    CAMStorageIDPtr pCam;
    RAWStorageIDPtr pRaw = (RAWStorageIDPtr)pUnpack;

    tot = sizeof(CAMStorageID);

    ptr = (char*)&pRaw->StorageIDs;
    tot += CalcCamSize( CAM_TYPE_AUINT32, &ptr);

    pCam = (CAMStorageIDPtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMStorageID);

    ptr = (char*)&pRaw->StorageIDs;
    pCam->StorageIDs = (uint32_t*)pData;
    pCam->cntStorageIDs = MoveCamData( CAM_TYPE_AUINT32, &ptr, &pData);
    if (!pCam->cntStorageIDs)
        pCam->StorageIDs = 0;

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/

uint32_t    Build_CamStorageInfo( void * pUnpack, CAMStorageInfoPtr* ppCam)
{
    uint32_t    tot;
    char *      ptr;
    char *      pData;
    CAMStorageInfoPtr pCam;
    RAWStorageInfoPtr pRaw = (RAWStorageInfoPtr)pUnpack;

    // calculate the aggregate size of the structure & strings
    tot = sizeof(CAMStorageInfo);

    // get the size of the 2 strings
    ptr = &pRaw->StorageDescription[0];
    tot += CalcCamSize( CAM_TYPE_STR, &ptr);
    tot += CalcCamSize( CAM_TYPE_STR, &ptr);

    // allocate & clear mem then set pData to the
    // first byte after the structure
    pCam = (CAMStorageInfoPtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMStorageInfo);

    // copy the fixed position data
    pCam->StorageType       = pRaw->StorageType;
    pCam->FilesystemType    = pRaw->FilesystemType;
    pCam->AccessCapability  = pRaw->AccessCapability;
    pCam->MaxCapacity       = pRaw->MaxCapacity;
    pCam->FreeSpaceInBytes  = pRaw->FreeSpaceInBytes;
    pCam->FreeSpaceInImages = pRaw->FreeSpaceInImages;

    // copy the two unicode strings
    ptr = &pRaw->StorageDescription[0];

    pCam->StorageDescription = pData;
    if (!MoveCamData( CAM_TYPE_STR, &ptr, &pData))
        pCam->StorageDescription = 0;

    pCam->VolumeLabel = pData;
    if (!MoveCamData( CAM_TYPE_STR, &ptr, &pData))
        pCam->VolumeLabel = 0;

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/

uint32_t    Build_CamObjectHandle( void * pUnpack, CAMObjectHandlePtr* ppCam)
{
    uint32_t    tot;
    char *      ptr;
    char *      pData;
    CAMObjectHandlePtr pCam;
    RAWObjectHandlePtr pRaw = (RAWObjectHandlePtr)pUnpack;

    tot = sizeof(CAMObjectHandle);

    ptr = (char*)&pRaw->Handles;
    tot += CalcCamSize( CAM_TYPE_AUINT32, &ptr);

    pCam = (CAMObjectHandlePtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMObjectHandle);

    ptr = (char*)&pRaw->Handles;
    pCam->Handles = (uint32_t*)pData;
    pCam->cntHandles = MoveCamData( CAM_TYPE_AUINT32, &ptr, &pData);
    if (!pCam->cntHandles)
        pCam->Handles = 0;

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/

uint32_t    Build_CamPropertyDesc( void * pUnpack, CAMPropertyDescPtr* ppCam)
{
    uint32_t    tot;
    uint32_t    cnt;
    uint32_t    ctr;
    uint32_t    type;
    char *      ptr;
    char *      pData;
    CAMPropertyDescPtr pCam;
    RAWPropertyDescPtr pRaw = (RAWPropertyDescPtr)pUnpack;

    // exit if we don't support the datatype
    type = pRaw->DataType;
    switch (type) {
        case CAM_TYPE_UNDEF:
        case CAM_TYPE_INT128:
        case CAM_TYPE_UINT128:
        case CAM_TYPE_AINT128:
        case CAM_TYPE_AUINT128:
            return CAMERR_NOTSUPPORTED;
    }

    tot = sizeof(CAMPropertyDesc);

    // calc the size of the default & current values
    ptr = &pRaw->DefaultValue[0];
    tot += CalcCamSize( type, &ptr);
    tot += CalcCamSize( type, &ptr);

    // get the form flag
    ctr = (uint32_t)*ptr++;

    // for ranges, this only supports single integers;
    // for enumerations, this supports single integers & strings
    if (ctr == CAM_PROP_RangeForm) {
        if (type <= CAM_TYPE_UINT64)
            tot += 3 * CalcCamSize( type, &ptr);
    }
    else
    if (ctr == CAM_PROP_EnumForm) {
        cnt = *((uint16_t*)ptr);
        ptr += sizeof(uint16_t);

        if (type <= CAM_TYPE_UINT64)
            tot += cnt * CalcCamSize( type, &ptr);
        else
        if (type == CAM_TYPE_STR) {
            for (ctr = 0; ctr < cnt; ctr++)
                tot += CalcCamSize( type, &ptr);
        }
    }

    // now that we have the combined size of everything, allocate it,
    // clear it, then set pData to the first byte after the structure
    pCam = (CAMPropertyDescPtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMPropertyDesc);

    // lay in the data
    pCam->DevicePropertyCode  = pRaw->DevicePropertyCode;
    pCam->DataType            = pRaw->DataType;
    pCam->GetSet              = pRaw->GetSet;

    ptr = &pRaw->DefaultValue[0];

    pCam->DefaultValue = pData;
    pCam->cntDefaultValue = MoveCamData( type, &ptr, &pData);
    if (pCam->cntDefaultValue == 0)
        pCam->DefaultValue = 0;

    pCam->CurrentValue = pData;
    pCam->cntCurrentValue = MoveCamData( type, &ptr, &pData);
    if (pCam->cntCurrentValue == 0)
        pCam->CurrentValue = 0;

    // get the form flag
    pCam->FormFlag = (uint32_t)*ptr++;

    if (pCam->FormFlag == CAM_PROP_RangeForm) {
        if (type > CAM_TYPE_UINT64)
            pCam->FormFlag += CAM_PROP_NotSupported;
        else {
            pCam->Form.Range.MinimumValue = (uint32_t*)pData;
            MoveCamData( type, &ptr, &pData);
            pCam->Form.Range.MaximumValue = (uint32_t*)pData;
            MoveCamData( type, &ptr, &pData);
            pCam->Form.Range.StepSize = (uint32_t*)pData;
            MoveCamData( type, &ptr, &pData);
        }
    }
    else
    if (pCam->FormFlag == CAM_PROP_EnumForm) {
        cnt = *((uint16_t*)ptr);
        if (cnt && (type <= CAM_TYPE_UINT64 || type == CAM_TYPE_STR)) {
            ptr += sizeof(uint16_t);
            pCam->Form.Enum.cntSupportedValues = cnt;
            pCam->Form.Enum.SupportedValues = (uint32_t*)pData;
            for (ctr = 0; ctr < cnt; ctr++)
                MoveCamData( type, &ptr, &pData);
        }
        else
            pCam->FormFlag += CAM_PROP_NotSupported;
    }

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/

uint32_t    Build_CamPropertyValue( void * pUnpack, uint32_t type,
                                    CAMPropertyValuePtr* ppCam)
{
    uint32_t    tot;
    char *      pData;
    CAMPropertyValuePtr pCam;
    char *      pRaw = (char*)pUnpack;

    // exit if we don't support the datatype
    switch (type) {
        case CAM_TYPE_UNDEF:
        case CAM_TYPE_INT128:
        case CAM_TYPE_UINT128:
        case CAM_TYPE_AINT128:
        case CAM_TYPE_AUINT128:
           return CAMERR_NOTSUPPORTED;
    }

    tot = sizeof(CAMPropertyValue);
    tot += CalcCamSize( type, &pRaw);

    pCam = (CAMPropertyValuePtr)malloc( tot);
    if (!pCam)
        return CAMERR_MALLOC;
    memset( pCam, 0, tot);
    pData = ((char*)pCam) + sizeof(CAMPropertyValue);

    pRaw = (char*)pUnpack;
    pCam->DataType = type;
    pCam->Value = pData;
    pCam->cntValue = MoveCamData( type, &pRaw, &pData);
    if (!pCam->cntValue)
        pCam->Value = 0;

    *ppCam = pCam;
    return 0;
}

/***************************************************************************/
/***************************************************************************/

uint32_t    Build_RawPropertyValue( CAMPropertyValuePtr pCam,
                                    char ** ppPack, uint32_t* pSize)
{
    char *      pRaw;
    char *      ptr;

    *ppPack = 0;
    *pSize = CalcRawSize( pCam->DataType, pCam->cntValue, pCam->Value);
    if (*pSize == 0)
        return 0;

    pRaw = malloc( *pSize);
    if (!pRaw)
        return CAMERR_MALLOC;
    memset( pRaw, 0, *pSize);

    ptr = pRaw;
    MoveRawData( pCam->DataType, pCam->cntValue, pCam->Value, &ptr);
    *ppPack = pRaw;

    return 0;
}

/***************************************************************************/
/***************************************************************************/

static uint32_t CalcCamSize( uint32_t type, char ** pptr)
{
    uint32_t  cnt;

    switch (type) {
        case CAM_TYPE_STR:
            cnt = (uint32_t)**pptr;
            *pptr += cnt * sizeof(uint16_t) + sizeof(char);
            if (cnt == 0)
                cnt = 1;
            return (cnt * sizeof(char));

        case CAM_TYPE_INT8:
        case CAM_TYPE_UINT8:
            *pptr += sizeof(uint8_t);
            return sizeof(uint32_t);

        case CAM_TYPE_INT16:
        case CAM_TYPE_UINT16:
            *pptr += sizeof(uint16_t);
            return sizeof(uint32_t);

        case CAM_TYPE_INT32:
        case CAM_TYPE_UINT32:
            *pptr += sizeof(uint32_t);
            return sizeof(uint32_t);

        case CAM_TYPE_INT64:
        case CAM_TYPE_UINT64:
            *pptr += sizeof(uint64_t);
            return sizeof(uint64_t);

        case CAM_TYPE_INT128:
        case CAM_TYPE_UINT128:
            *pptr += 2 * sizeof(uint64_t);
            return 0;

        case CAM_TYPE_AINT8:
        case CAM_TYPE_AUINT8:
            cnt = *((uint32_t*)*pptr);
            *pptr += cnt * sizeof(uint8_t) + sizeof(uint32_t);
            return (cnt * sizeof(uint32_t));

        case CAM_TYPE_AINT16:
        case CAM_TYPE_AUINT16:
            cnt = *((uint32_t*)*pptr);
            *pptr += cnt * sizeof(uint16_t) + sizeof(uint32_t);
            return (cnt * sizeof(uint32_t));

        case CAM_TYPE_AINT32:
        case CAM_TYPE_AUINT32:
            cnt = *((uint32_t*)*pptr);
            *pptr += cnt * sizeof(uint32_t) + sizeof(uint32_t);
            return (cnt * sizeof(uint32_t));

        case CAM_TYPE_AINT64:
        case CAM_TYPE_AUINT64:
            cnt = *((uint32_t*)*pptr);
            *pptr += cnt * sizeof(uint64_t) + sizeof(uint32_t);
            return (cnt * sizeof(uint64_t));

        case CAM_TYPE_AINT128:
        case CAM_TYPE_AUINT128:
            cnt = *((uint32_t*)*pptr);
            *pptr += cnt * 2 * sizeof(uint64_t) + sizeof(uint32_t);
            return 0;
    }

    return 0;
}

/***************************************************************************/

static uint32_t MoveCamData( uint32_t type, char ** pptr, char ** ppData)
{
    uint32_t  cnt;
    uint32_t  ctr;

    if (type <= CAM_TYPE_UINT128) {
        ctr = sizeof(uint32_t);
        cnt = 1;
        switch (type) {

        case CAM_TYPE_INT8:
            *((int32_t*)*ppData) = *((int8_t*)*pptr);
            *pptr += sizeof(uint8_t);
            break;

        case CAM_TYPE_UINT8:
            *((uint32_t*)*ppData) = *((uint8_t*)*pptr);
            *pptr += sizeof(uint8_t);
            break;

        case CAM_TYPE_INT16:
            *((int32_t*)*ppData) = *((int16_t*)*pptr);
            *pptr += sizeof(uint16_t);
            break;

        case CAM_TYPE_UINT16:
            *((uint32_t*)*ppData) = *((uint16_t*)*pptr);
            *pptr += sizeof(uint16_t);
            break;

        case CAM_TYPE_INT32:
        case CAM_TYPE_UINT32:
            *((uint32_t*)*ppData) = *((uint32_t*)*pptr);
            *pptr += sizeof(uint32_t);
            break;

        case CAM_TYPE_INT64:
        case CAM_TYPE_UINT64:
            *((uint64_t*)*ppData) = *((uint64_t*)*pptr);
            *pptr += sizeof(uint64_t);
            ctr = sizeof(uint64_t);
            break;

        case CAM_TYPE_INT128:
        case CAM_TYPE_UINT128:
            *pptr += 2 * sizeof(uint64_t);
            ctr = 0;
            cnt = 0;
            break;
        }
        
        *ppData += ctr;
        return cnt;
    }

    if (type <= CAM_TYPE_AUINT128) {
        cnt = *((uint32_t*)*pptr);
        *pptr += sizeof(uint32_t);
        ctr = 0;
        switch (type) {

        case CAM_TYPE_AINT8:
            while (ctr++ < cnt) {
                *((int32_t*)*ppData) = *((int8_t*)*pptr);
                *ppData += sizeof(uint32_t);
                *pptr += sizeof(uint8_t);
            }
            break;

        case CAM_TYPE_AUINT8:
            while (ctr++ < cnt) {
                *((uint32_t*)*ppData) = *((uint8_t*)*pptr);
                *ppData += sizeof(uint32_t);
                *pptr += sizeof(uint8_t);
            }
            break;

        case CAM_TYPE_AINT16:
            while (ctr++ < cnt) {
                *((int32_t*)*ppData) = *((int16_t*)*pptr);
                *ppData += sizeof(uint32_t);
                *pptr += sizeof(uint16_t);
            }
            break;

        case CAM_TYPE_AUINT16:
            while (ctr++ < cnt) {
                *((uint32_t*)*ppData) = *((uint16_t*)*pptr);
                *ppData += sizeof(uint32_t);
                *pptr += sizeof(uint16_t);
            }
            break;

        case CAM_TYPE_AINT32:
        case CAM_TYPE_AUINT32:
            while (ctr++ < cnt) {
                *((uint32_t*)*ppData) = *((uint32_t*)*pptr);
                *ppData += sizeof(uint32_t);
                *pptr += sizeof(uint32_t);
            }
            break;

        case CAM_TYPE_AINT64:
        case CAM_TYPE_AUINT64:
            while (ctr++ < cnt) {
                *((uint64_t*)*ppData) = *((uint64_t*)*pptr);
                *ppData += sizeof(uint64_t);
                *pptr += sizeof(uint64_t);
            }
            break;

        case CAM_TYPE_AINT128:
        case CAM_TYPE_AUINT128:
            *pptr += cnt * 2 * sizeof(uint64_t);
            cnt = 0;
            break;
        }

        return cnt;
    }

    if (type == CAM_TYPE_STR) {
        cnt = (uint32_t)**pptr;
        *pptr += sizeof(char);
        if (cnt == 0) {
            **ppData = 0;
            *ppData += sizeof(char);
        }
        else
        for (ctr = 0; ctr < cnt; ctr++) {
            **ppData = **pptr;
            *ppData += sizeof(char);
            *pptr += sizeof(uint16_t);
        }
        return 1;
    }

    return 0;
}

/***************************************************************************/

// pDate points at a unicode string in the form YYYYMMDDThhmmss where
// the 'T' between date & time is literally a 'T';  also, there may
// be additional info after 'ss' but we ignore it

static time_t  MoveCamDateTime( char ** pptr)
{
    struct tm   dt;
    char        buf[8];
    char *      pDate;

    pDate = *pptr;
    *pptr += *pDate;
    if (*pDate++ < 16)
        return 0;

    memset( &dt, 0, sizeof(dt));

    buf[0] = pDate[0];
    buf[1] = pDate[2];
    buf[2] = pDate[4];
    buf[3] = pDate[6];
    buf[4] = 0;
    dt.tm_year = atoi( buf) - 1900;
    pDate += 8;

    buf[0] = pDate[0];
    buf[1] = pDate[2];
    buf[2] = 0;
    dt.tm_mon = atoi( buf) - 1;
    pDate += 4;

    buf[0] = pDate[0];
    buf[1] = pDate[2];
    dt.tm_mday = atoi( buf);
    pDate += 6;     // skip over the 'T' separating date from time

    buf[0] = pDate[0];
    buf[1] = pDate[2];
    dt.tm_hour = atoi( buf);
    pDate += 4;

    buf[0] = pDate[0];
    buf[1] = pDate[2];
    dt.tm_min = atoi( buf);
    pDate += 4;

    buf[0] = pDate[0];
    buf[1] = pDate[2];
    dt.tm_sec = atoi( buf);

    return mktime( &dt);
}

/***************************************************************************/
/***************************************************************************/

static uint32_t CalcRawSize( uint32_t type, uint32_t cnt, char * pCam)
{

    switch (type) {
        case CAM_TYPE_STR: {
            uint32_t  size = 0;

            if (pCam)
                size = strlen( pCam);
            if (size == 0)
                return 1;

            if (++size > 255)
                size = 255;
            return (size * sizeof(uint16_t) + 1);
        }

        case CAM_TYPE_INT8:
        case CAM_TYPE_UINT8:
            return sizeof(uint8_t);

        case CAM_TYPE_INT16:
        case CAM_TYPE_UINT16:
            return sizeof(uint16_t);

        case CAM_TYPE_INT32:
        case CAM_TYPE_UINT32:
            return sizeof(uint32_t);

        case CAM_TYPE_INT64:
        case CAM_TYPE_UINT64:
            return sizeof(uint64_t);

        case CAM_TYPE_INT128:
        case CAM_TYPE_UINT128:
            return (2 * sizeof(uint64_t));

        case CAM_TYPE_AINT8:
        case CAM_TYPE_AUINT8:
            return (cnt * sizeof(uint8_t) + sizeof(uint32_t));

        case CAM_TYPE_AINT16:
        case CAM_TYPE_AUINT16:
            return (cnt * sizeof(uint16_t) + sizeof(uint32_t));

        case CAM_TYPE_AINT32:
        case CAM_TYPE_AUINT32:
            return (cnt * sizeof(uint32_t) + sizeof(uint32_t));

        case CAM_TYPE_AINT64:
        case CAM_TYPE_AUINT64:
            return (cnt * sizeof(uint64_t) + sizeof(uint32_t));

        case CAM_TYPE_AINT128:
        case CAM_TYPE_AUINT128:
            return (cnt * 2 * sizeof(uint64_t) + sizeof(uint32_t));
    }

    return 0;
}

/***************************************************************************/

static void MoveRawData( uint32_t type, uint32_t cnt, char * pCam, char ** ppRaw)
{
    uint32_t  ctr;

    if (type <= CAM_TYPE_UINT128) {

      switch (type) {
        case CAM_TYPE_INT8:
        case CAM_TYPE_UINT8:
            *((uint8_t*)*ppRaw) = *((uint8_t*)pCam);
            *ppRaw += sizeof(uint8_t);
            return;

        case CAM_TYPE_INT16:
        case CAM_TYPE_UINT16:
            *((uint16_t*)*ppRaw) = *((uint16_t*)pCam);
            *ppRaw += sizeof(uint16_t);
            return;

        case CAM_TYPE_INT32:
        case CAM_TYPE_UINT32:
            *((uint32_t*)*ppRaw) = *((uint32_t*)pCam);
            *ppRaw += sizeof(uint32_t);
            return;

        case CAM_TYPE_INT64:
        case CAM_TYPE_UINT64:
            *((uint64_t*)*ppRaw) = *((uint64_t*)pCam);
            *ppRaw += sizeof(uint64_t);
            return;

        case CAM_TYPE_INT128:
        case CAM_TYPE_UINT128:
            *((uint64_t*)*ppRaw) = *((uint64_t*)pCam);
            *ppRaw += sizeof(uint64_t);
            pCam += sizeof(uint64_t);
            *((uint64_t*)*ppRaw) = *((uint64_t*)pCam);
            *ppRaw += sizeof(uint64_t);
            return;
      }
      return;
    }

    if (type <= CAM_TYPE_AUINT128) {

        *((uint32_t*)*ppRaw) = cnt;
        *ppRaw += sizeof(uint32_t);
        ctr = 0;

        switch (type) {

            case CAM_TYPE_AINT8:
            case CAM_TYPE_AUINT8:
                while (ctr++ < cnt) {
                    *((uint8_t*)*ppRaw) = *((uint8_t*)pCam);
                    *ppRaw += sizeof(uint8_t);
                    pCam += sizeof(uint32_t);
                }
                return;

            case CAM_TYPE_AINT16:
            case CAM_TYPE_AUINT16:
                while (ctr++ < cnt) {
                    *((uint16_t*)*ppRaw) = *((uint16_t*)pCam);
                    *ppRaw += sizeof(uint16_t);
                    pCam += sizeof(uint32_t);
                }
                return;

            case CAM_TYPE_AINT32:
            case CAM_TYPE_AUINT32:
                while (ctr++ < cnt) {
                    *((uint32_t*)*ppRaw) = *((uint32_t*)pCam);
                    *ppRaw += sizeof(uint32_t);
                    pCam += sizeof(uint32_t);
                }
                return;

            case CAM_TYPE_AINT64:
            case CAM_TYPE_AUINT64:
                while (ctr++ < cnt) {
                    *((uint64_t*)*ppRaw) = *((uint64_t*)pCam);
                    *ppRaw += sizeof(uint64_t);
                    pCam += sizeof(uint64_t);
                }
                return;

            case CAM_TYPE_AINT128:
            case CAM_TYPE_AUINT128:
                while (ctr++ < cnt) {
                    *((uint64_t*)*ppRaw) = *((uint64_t*)pCam);
                    *ppRaw += sizeof(uint64_t);
                    pCam += sizeof(uint64_t);
                    *((uint64_t*)*ppRaw) = *((uint64_t*)pCam);
                    *ppRaw += sizeof(uint64_t);
                    pCam += sizeof(uint64_t);
                }
        }
        return;
    }

    if (type == CAM_TYPE_STR) {
        uint32_t  size = 0;

        if (pCam)
            size = strlen( (char*)pCam);
        if (size == 0) {
            *((uint8_t*)*ppRaw) = 0;
            *ppRaw += sizeof(uint8_t);
            return;
        }

        if (++size > 255)
            size = 255;
        *((uint8_t*)*ppRaw) = (uint8_t)size;
        *ppRaw += sizeof(uint8_t);

        for (ctr = 0; ctr < size; ctr++, pCam++, *ppRaw += sizeof(uint16_t))
            **ppRaw = *pCam;

        *ppRaw[-sizeof(uint16_t)] = 0;
        return;
    }

    return;
}

/***************************************************************************/

#pragma pack()


