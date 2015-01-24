/***************************************************************************/
//  ptpro.c
/***************************************************************************/
/*
 * Copyright (C) 2006 Richard L Walsh <rich@e-vertise.com>
 * Copyright (C) 2001-2005 Mariusz Woloszyn <emsi@ipartners.pl>
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
 * Copyright (c) 2015, bww bitwise works GmbH
 *
 * 2015-01-22 Silvan Scherrer version 1.1.1
 *            enhanced UsbQueryDeviceReport to 4096 byte 
 *            removed parameter 1 from CloseSession
 */
/***************************************************************************/
/*
 *  Avoiding the GPL...
 *
 *  PTPro v1.1 incorporates the know-how found in libptp2 without using
 *  any of its code.  The sole exception is this file (ptpro.c) which
 *  includes code taken verbatim from ptpcam.c.  Because ptpcam.c was
 *  licensed under the GPL, all of PTPro is burdened with it.
 *
 *  If you want to reuse PTPro's original code while avoiding the GPL,
 *  obtain the source for "Cameraderie".  It contains everything in
 *  PTPro (except ptpro.c) and is licensed under the less restrictive
 *  Mozilla Public License.
 *
 */
/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include "ptprousb.h"
#include "ptpro.h"

/***************************************************************************/

#pragma pack(1)

/***************************************************************************/

short   verbose = 0;
char    underline_buf[64];

/***************************************************************************/
/***************************************************************************/
/*
    top-level functions
        main & help
        show info about the camera(s)
        get / set camera properties
        show file info
        get / delete files
        reset camera / clear stalls
        display error / debug messages

*/
/***************************************************************************/
/***************************************************************************/
/*
    main & help

    main
        parse_options
        range_from_arg
    usage
    help

*/
/***************************************************************************/

int main( int argc, char ** argv)
{
    uint32_t         rtn = 0;
    uint32_t         action = 0;
    CAMCameraListPtr pList = 0;
    CAMDeviceInfoPtr pDevInfo = 0;
    CAMHandle        hCam = 0;
    CAMDevice        device;
    OPTS             opts;

    action = parse_options( &opts, argc, argv);
    if (!action)
        return 0;

do {
    SetUsbDebug( verbose);

    rtn = InitUsb();
    if (rtn)
        break;

    rtn = GetCameraList( &pList, (opts.flags & OPT_FORCE));
    if (rtn)
        break;

    if (action == ACT_LIST_DEVICES) {
        list_devices( pList);
        break;
    }

    device = SelectCamera( &opts, pList);
    if (!device)
        break;

    if (action == ACT_DEVICE_RESET) {
        ResetCamera( device);
        break;
    }

    rtn = InitCamera( device, &hCam);
    if (rtn)
        break;

    rtn = GetDeviceInfo( hCam, &pDevInfo);
    if (rtn)
        break;

    switch (action) {
        case ACT_SHOW_INFO:
            show_info( pDevInfo);
            break;
        case ACT_LIST_OPERATIONS:
            list_operations( pDevInfo);
            break;
        case ACT_STORAGE_INFO:
            show_storage_info( hCam, pDevInfo);
            break;
        case ACT_LIST_PROPERTIES:
            list_properties( pDevInfo);
            break;
        case ACT_GETSET_PROPERTY:
            getset_property( hCam, pDevInfo, &opts);
            break;
        case ACT_LIST_FILES:
            list_files( hCam, pDevInfo);
            break;
        case ACT_LIST_HANDLES:
            list_handles( hCam, pDevInfo);
            break;
        case ACT_HANDLE_INFO:
            show_handle_info( hCam, pDevInfo, &opts);
            break;
        case ACT_GET_FILE:
            get_files( hCam, pDevInfo, &opts);
            break;
        case ACT_DELETE_FILE:
            delete_files( hCam, pDevInfo, &opts);
            break;
    }

} while (0);

    if (rtn)
        camcli_error( GetErrorName( pDevInfo, rtn));

    free( pDevInfo);
    free( pList);
    TermCamera( &hCam);
    TermUsb();

    return 0;
}

/***************************************************************************/

uint32_t    parse_options( OPTS * pOpts, int argc, char ** argv)

{
    uint32_t    ret = 0;
    uint32_t    action = 0;
    uint32_t    option_index = 0;
    uint32_t    fOK = 1;
    int         opt = 0;

    static struct option loptions[] = {
        {"list-files",0,0,'l'},
        {"list-handles",0,0,'H'},   // this option is "undocumented"
        {"file-info",1,0,'i'},
        {"save-file",1,0,'f'},
        {"save-thumb",1,0,'t'},
        {"erase-file",1,0,'e'},
        {"replace",0,0,'r'},
        {"device-list",0,0,'d'},
        {"device",1,0,'D'},
        {"camera-info",0,0,'c'},
        {"storage-info",0,0,'s'},
        {"operation-list",0,0,'o'},
        {"property-list",0,0,'p'},
        {"getset-property",1,0,'g'},
        {"val",1,0,'V'},
        {"reset",0,0,'R'},
        {"verbose",2,0,'v'},
        {"help",0,0,'h'},
        {0,0,0,0}
    };

    if (argc == 1) {
        help();
        return 0;
    }

    memset( pOpts, 0, sizeof(OPTS));
    opterr = 0;

    while (fOK) {
        opt = getopt_long( argc, argv, "lHi:f:t:e:rdD:csopg:V:Rv::h",
                           loptions, &option_index);

        switch (opt) {

        // special cases
        case -1:
            if (optind+1 == argc && ret == ACT_GET_FILE && !pOpts->value)
                pOpts->value = strdup( argv[optind]);
            fOK = 0;
            break;
        case '?':
            camcli_error( "Option '-%c' is unknown or missing its arguments", optopt);
            ret = fOK = 0;
            break;
        case 'h':
            help();
            ret = fOK = 0;
            break;

        // modifiers
        case 0:
            if (!(strcmp( "val", loptions[option_index].name)))
                pOpts->value = strdup( optarg);
            break;
        case 'V':
            pOpts->value = strdup( optarg);
            break;
        case 'r':
            pOpts->flags |= OPT_REPLACE;
            break;
        case 'D':
            if (device_from_arg( optarg, &pOpts->dev))
                pOpts->flags |= OPT_FORCE;
            else
                ret = fOK = 0;
            break;
        case 'v':
            if (optarg) 
                verbose = strtol( optarg, NULL, 10);
            else
                verbose = 1;
            break;

        // actions
        case 'l':
            action = ACT_LIST_FILES;
            break;
        case 'H':
            action = ACT_LIST_HANDLES;
            break;
        case 'i':
            if (range_from_arg( optarg, &pOpts->first, &pOpts->last))
                action = ACT_HANDLE_INFO;
            else
                ret = fOK = 0;
            break;
        case 'f':
            if (range_from_arg( optarg, &pOpts->first, &pOpts->last))
                action = ACT_GET_FILE;
            else
                ret = fOK = 0;
            break;
        case 't':
            if (range_from_arg( optarg, &pOpts->first, &pOpts->last)) {
                action = ACT_GET_FILE;
                pOpts->flags |= OPT_THUMBNAIL;
            }
            else
                ret = fOK = 0;
            break;
        case 'e':
            if (range_from_arg( optarg, &pOpts->first, &pOpts->last))
                action = ACT_DELETE_FILE;
            else
                ret = fOK = 0;
            break;
        case 'c':
            action = ACT_SHOW_INFO;
            break;
        case 'd':
            action = ACT_LIST_DEVICES;
            break;
        case 's':
            action = ACT_STORAGE_INFO;
            break;
        case 'o':
            action = ACT_LIST_OPERATIONS;
            break;
        case 'p':
            action = ACT_LIST_PROPERTIES;
            break;
        case 'g':
            action = ACT_GETSET_PROPERTY;
            pOpts->property = strtol( optarg, NULL, 16);
            break;
        case 'R':
            action = ACT_DEVICE_RESET;
            break;
        default:
            camcli_error( "Getopt returned character code 0x%X", opt);
            break;
        } // end switch

        // prevent multiple commands
        if (action) {
            if (ret) {
                camcli_error( "Option '-%c' conflicts with previous option", opt);
                ret = fOK = 0;
            }
            else {
                ret = action;
                action = 0;
            }
        }
    } // end while

    return ret;
}

/***************************************************************************/

int device_from_arg( char* arg, uint32_t* dev)
{
    int     rtn = 0;

    *dev = strtoul( arg, 0, 10);

    if (*dev)
        rtn = 1;
    else
    if (arg[0] == '*') {
        *dev = ~0;
        rtn = 1;
    }
    else
        camcli_error( "Invalid device ID (%s) - must be a number or '*'", arg);

    return rtn;
}

/***************************************************************************/

int range_from_arg( char* arg, uint32_t* first, uint32_t* last)
{
    int     rtn = 0;
    char *  ptr;

do {
    *first = strtoul( arg, &ptr, 10);

    if (*first == 0) {
        if (arg[0] == '*')
            ptr = &arg[1];
        else
            if (arg[0] != '0')
                break;
    }

    if (*ptr == 0) {
        if (arg[0] == '*')
            *last = ~0;
        else
            *last = *first;
        rtn = 1;
        break;
    }

    if (*ptr != '-')
        break;

    ptr++;
    if (ptr[0] == '*' && ptr[1] == 0)
        *last = ~0;
    else
        *last = strtoul( ptr, 0, 10);

    if (*last > 0 && *last >= *first)
        rtn = 1;

} while (0);

    if (!rtn)
        camcli_error( "Invalid or malformed range of handles: '%s'", arg);

    return rtn;
}

/***************************************************************************/

CAMDevice   SelectCamera( OPTS * pOpts, CAMCameraListPtr pList)
{
    uint32_t    ctr;

    if (pOpts->dev == 0)
        return pList->Cameras[0].Device;

    for (ctr = 0; ctr < pList->cntCameras; ctr++)
        if (pList->Cameras[ctr].DeviceNbr == pOpts->dev)
            return pList->Cameras[ctr].Device;

    camcli_error( "device number %d can not be used", pOpts->dev);

    return 0;
}

/***************************************************************************/

void help()
{
    printf(
    "PTPro v1.1.1 a PTP camera utility - Copyright (c) 2006 RL Walsh et al.\n"
    "\n"
    "Usage:  PTPro -Option [-Option ...] [Save-as Filename]\n"
    "  -l, --list-files             List all files & their handles\n"
    "  -i, --file-info=Handles      Show extended file info\n"
    "  -f, --save-file=Handles      Save file(s)\n"
    "  -t, --save-thumb=Handles     Save thumbnail(s)\n"
    "  -e, --erase-file=Handles     Erase file(s) from camera\n"
    "  -r  --replace                Replace existing file when saving to disk\n"
    "  -d, --device-list            List all PTP cameras (use -d -D* to list\n"
    "  -D, --device=Number          Specify device        all USB devices)\n"
    "  -c, --camera-info            Show camera info\n"
    "  -s, --storage-info           Show storage (memory) info\n"
    "  -o, --operation-list         List supported operations\n"
    "  -p, --property-list          List all PTP device properties\n"
    "  -g, --getset-property=Number Get or set property value\n"
    "  -V  --val=Value              New property value (use with -g to set value)\n"
    "  -R, --reset                  Reset the device\n"
    "  -v, --verbose                Show error details (use -v2 for debug info)\n"
    "\n"
    "Handles:  [First-Last] or [First] or [*] (all files)\n"
    "Filename: [Path\\][Name][.Ext] - '*' copies the original Name or Ext\n"
    "          '#' inserts a sequence number in Name (e.g. cats#0123.*)\n");
}

/***************************************************************************/

// a less-than-compelling routine that creates an underline
// the length of the camera model name

char *  underline( const char *str)
{
    int i;

    i = strlen( str);
    if (i >= sizeof(underline_buf))
        i = sizeof(underline_buf - 1);
    memset( underline_buf, '=', i);
    underline_buf[i] = 0;

    return underline_buf;
}

/***************************************************************************/
/***************************************************************************/
/*
    show info about the camera(s)

    list_devices
    show_info
    list_operations
    show_storage_info

*/
/***************************************************************************/

void list_devices( CAMCameraListPtr pList)
{
    uint32_t         ctr;
    char *           pName;
    CAMHandle        hCam;
    CAMUsbInfo *     ptr;
    CAMDeviceInfoPtr pInfo;

    printf( "\n Device list\n ===========\n");
    printf( " Bus  Device  VendorID  ProductID  Camera\n");

    for (ctr=0, ptr=pList->Cameras; ctr < pList->cntCameras; ctr++, ptr++) {

        hCam = 0;
        pInfo = 0;

        if (InitCamera( ptr->Device, &hCam))
            pName = "could not open device";
        else
        if (GetDeviceInfo( hCam, &pInfo))
            pName = "could not get device info";
        else
            pName = pInfo->Model;

        printf( " %2s     % d     0x%04X    0x%04X    %s\n",
                ptr->BusName, ptr->DeviceNbr, ptr->VendorId,
                ptr->ProductId, pName);

        free( pInfo);
        TermCamera( &hCam);
    }

    return;
}

/***************************************************************************/

void show_info( CAMDeviceInfoPtr pDevInfo)
{
    printf("\n Camera information  %s\n",   pDevInfo->Model);
    printf(" ==================  %s\n",     underline(pDevInfo->Model));
    printf("      Manufacturer:  %s\n",     pDevInfo->Manufacturer);
    printf("      SerialNumber:  %s\n",     pDevInfo->SerialNumber);
    printf("     DeviceVersion:  %s\n",     pDevInfo->DeviceVersion);
    printf("       ExtensionID:  0x%04X\n", pDevInfo->VendorExtensionID);
    printf("  ExtensionVersion:  0x%04X\n", pDevInfo->VendorExtensionVersion);
    printf("    ExtDescription:  %s\n",     pDevInfo->VendorExtensionDesc);

    if (pDevInfo->cntCaptureFormats == 0)
        printf( "    CaptureFormats:  [unknown]\n");
    else {
        uint32_t ctr;
        uint16_t fmt;
        for (ctr = 0; ctr < pDevInfo->cntCaptureFormats; ctr++) {
            fmt = pDevInfo->CaptureFormats[ctr];
            printf( "%s0x%04X  %s\n",
                    (ctr ? "                     " : "    CaptureFormats:  "),
                    fmt, GetCodeName( fmt, CAM_OBJECT_FORMAT));
        }
    }

    if (pDevInfo->cntImageFormats == 0)
        printf( "       FileFormats:  [unknown]\n");
    else {
        uint32_t ctr;
        uint16_t fmt;
        for (ctr = 0; ctr < pDevInfo->cntImageFormats; ctr++) {
            fmt = pDevInfo->ImageFormats[ctr];
            printf( "%s0x%04X  %s\n",
                    (ctr ? "                     " : "       FileFormats:  "),
                    fmt, GetCodeName( fmt, CAM_OBJECT_FORMAT));
        }
    }

    return;
}

/***************************************************************************/

void list_operations( CAMDeviceInfoPtr pDevInfo)
{
    int             i;
    const char*     name;

    printf( "\n Operations  %s\n", pDevInfo->Model);
    printf( " ==========  %s\n", underline( pDevInfo->Model));
    for (i = 0; i < pDevInfo->cntOperationsSupported; i++) {
        name = GetOperationName( pDevInfo,
                            pDevInfo->OperationsSupported[i]);
        printf( "    0x%04X:  %s\n",
                pDevInfo->OperationsSupported[i],
                (name ? name : "[unknown]"));
    }
}

/***************************************************************************/

void show_storage_info( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo)
{
    CAMStorageInfoPtr   pStorInfo = 0;
    CAMStorageIDPtr     pStorID = 0;
    int             i;

do {
    if (GetStorageIds( hCam, &pStorID)) {
        camcli_error( "Could not get storage IDs.");
        break;
    }
    if (pStorID->cntStorageIDs == 0) {
        camcli_error( "No storage IDs returned.");
        break;
    }

    printf ( "\n Storage information  %s\n", pDevInfo->Model);
      printf ( " ===================  %s\n", underline(pDevInfo->Model));

    for (i = 0; i < pStorID->cntStorageIDs; i++) {

        if (GetStorageInfo( hCam, pStorID->StorageIDs[i], &pStorInfo)) {
            camcli_error( "Could not get storage info for id %d", pStorID->StorageIDs[i]);
            continue;
        }

        printf ( "          StorageID:  0x%04X\n", pStorID->StorageIDs[i]);
        printf ( "        StorageType:  0x%04X\t%s\n", pStorInfo->StorageType,
                                GetCodeName( pStorInfo->StorageType, CAM_STORAGE));
        printf ( "     FilesystemType:  0x%04X\t%s\n", pStorInfo->FilesystemType,
                                GetCodeName( pStorInfo->FilesystemType, CAM_FILESYSTEM));
        printf ( "   AccessCapability:  0x%04X\t%s\n", pStorInfo->AccessCapability,
                                GetCodeName( pStorInfo->AccessCapability, CAM_STORAGE_ACCESS));
        printf ( "        MaxCapacity:  %lld\n", pStorInfo->MaxCapacity);
        printf ( "   FreeSpaceInBytes:  %lld\n", pStorInfo->FreeSpaceInBytes);
        printf ( "  FreeSpaceInImages:  0x%04X\n", pStorInfo->FreeSpaceInImages);
        printf ( " StorageDescription:  %s\n", (pStorInfo->StorageDescription ?
                                pStorInfo->StorageDescription : "[none]"));
        printf ( "        VolumeLabel:  %s\n\n", (pStorInfo->VolumeLabel ?
                                pStorInfo->VolumeLabel : "[none]"));

        free( pStorInfo);
        pStorInfo = 0;
    }

} while (0);

    if (pStorInfo);
        free( pStorInfo);
    if (pStorID)
        free( pStorID);

    return;
}

/***************************************************************************/
/***************************************************************************/
/*
    get / set camera properties

    list_properties
    getset_property
        print_propval
        set_property

*/
/***************************************************************************/

void list_properties( CAMDeviceInfoPtr pDevInfo)
{
    uint32_t ctr;

    printf( "\n Properties  %s\n", pDevInfo->Model);
    printf( " ==========  %s\n", underline( pDevInfo->Model));

    for (ctr = 0; ctr < pDevInfo->cntDevicePropertiesSupported; ctr++){

        printf( "    0x%04X:  %s\n",
                pDevInfo->DevicePropertiesSupported[ctr],
                GetPropertyName( pDevInfo,
                                 pDevInfo->DevicePropertiesSupported[ctr]));
    }
}

/***************************************************************************/

void getset_property( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo, OPTS *pOpts)
{
    CAMPropertyDescPtr  pProp = 0;

    if (validate_property( pDevInfo, pOpts->property)) {
        camcli_error( "The device does not support property 0x%04X", pOpts->property);
        return;
    }

    if (GetPropertyDesc( hCam, pOpts->property, &pProp)) {
        camcli_error( "Could not get device property description for 0x%04X", pOpts->property);
        return;
    }

    printf( "\n Get/Set Property  %s\n", pDevInfo->Model);
    printf(   " ================  %s\n", underline( pDevInfo->Model));
    printf(   "        Property:  0x%04X\n", pProp->DevicePropertyCode);
    printf(   "     Description:  %s\n", GetPropertyName( pDevInfo, pOpts->property));
    printf(   "        DataType:  %s\n", GetCodeName( pProp->DataType, CAM_DATATYPE));

    printf(   "   %sValue:  ", (pOpts->value ? "Previous" : " Current"));
    print_propval( pProp->DataType, pProp->cntCurrentValue, pProp->CurrentValue,
                   (pProp->FormFlag == CAM_PROP_EnumForm));
    printf("\n");

    if (pOpts->value) {
        uint16_t ret;
        CAMPropertyValuePtr  pValue = 0;

        ret = set_property( hCam, pProp->DevicePropertyCode,
                            pOpts->value, pProp->DataType);

        if (ret == CAMERR_BADPARM)
            camcli_error( "Unable to set property value for this datatype");
        else
        if (ret)
            camcli_error( "Unable to set property value - rc= %X", ret);
        else {
            ret = GetPropertyValue( hCam, pProp->DevicePropertyCode,
                                    pProp->DataType, &pValue);

            if (ret == 0) {
                printf( "        NewValue:  ");
                print_propval( pValue->DataType, pValue->cntValue, pValue->Value,
                               (pProp->FormFlag == CAM_PROP_EnumForm));
                printf("\n");
            }
            else
                camcli_error( "Unable to get new property value - rc= %X", ret);
        }

        if (pValue)
            free( pValue);
        if (pProp)
            free( pProp);
        return;
    }

    printf( "    DefaultValue:  ");
    print_propval( pProp->DataType, pProp->cntDefaultValue, pProp->DefaultValue,
                   (pProp->FormFlag == CAM_PROP_EnumForm));
    printf("\n");

    if (pProp->GetSet == CAM_PROP_ReadOnly)
        printf( "      Read/Write:  read only\n");
    else
        printf( "      Read/Write:  read & write\n");

    if (pProp->FormFlag == CAM_PROP_EnumForm) {
        if (pProp->Form.Enum.cntSupportedValues) {
            printf( "     ValueFormat:  enumeration\n");
            printf( "   AllowedValues:  ");
            print_propval( pProp->DataType, pProp->Form.Enum.cntSupportedValues,
                           pProp->Form.Enum.SupportedValues, 1);
            printf( "\n");
        }
    }
    else
    if (pProp->FormFlag == CAM_PROP_RangeForm) {
        printf( "     ValueFormat:  range\n");
        printf( "   AllowedValues:  ");
        print_propval( pProp->DataType, 1, pProp->Form.Range.MinimumValue, 1);
        printf( " - ");
        print_propval( pProp->DataType, 1, pProp->Form.Range.MaximumValue, 1);
        printf( "; step size: ");
        print_propval( pProp->DataType, 1, pProp->Form.Range.StepSize, 1);
        printf( "\n");
    }
    else
    if (pProp->FormFlag != CAM_PROP_None)
        printf( "     ValueFormat:  %02X [not supported]\n", (uint8_t)pProp->FormFlag);

    if (pProp)
        free( pProp);

    return;
}

/***************************************************************************/

void print_propval( uint32_t datatype, uint32_t cntValue,
                    void* value, uint32_t dec)
{
    uint32_t    ctr;
    uint32_t *  ptr;

    if (!cntValue)
        return;

    if (datatype != CAM_TYPE_AINT8 && datatype != CAM_TYPE_AUINT8) {
        char *      pSpacer;

        if (cntValue > 1) {
            pSpacer = "\n                   ";
            printf( "%d values", cntValue);
        }
        else
            pSpacer = "";

        for (ctr = 0, ptr = (uint32_t*)value; ctr < cntValue; ctr++, ptr++) {

          switch (datatype) {
            case CAM_TYPE_STR:
                printf("%s%s", pSpacer, (char*)ptr);
                break;

            case CAM_TYPE_INT8:
            case CAM_TYPE_INT16:
            case CAM_TYPE_INT32:
            case CAM_TYPE_AINT16:
            case CAM_TYPE_AINT32:
              if (dec)
                  printf("%s%d", pSpacer, *ptr);
              else
                  printf("%s0x%04X    %d", pSpacer, *ptr, *ptr);
              break;

            case CAM_TYPE_UINT8:
            case CAM_TYPE_UINT16:
            case CAM_TYPE_UINT32:
            case CAM_TYPE_AUINT16:
            case CAM_TYPE_AUINT32:
              if (dec)
                  printf("%s%u", pSpacer, *ptr);
              else
                  printf("%s0x%04X    %u", pSpacer, *ptr, *ptr);
              break;

            case CAM_TYPE_INT64:
            case CAM_TYPE_AINT64:
              if (dec)
                  printf("%s%lld", pSpacer, *(int64_t*)ptr);
              else
                  printf("%s0x%04llX    %lld", pSpacer, *(uint64_t*)ptr, *(int64_t*)ptr);
              ptr++;
              break;

            case CAM_TYPE_UINT64:
            case CAM_TYPE_AUINT64:
              if (dec)
                  printf("%s%llu", pSpacer, *(uint64_t*)ptr);
              else
                  printf("%s0x%04llX    %llu", pSpacer, *(uint64_t*)ptr, *(uint64_t*)ptr);
              ptr++;
              break;

            case CAM_TYPE_INT128:
            case CAM_TYPE_UINT128:
            case CAM_TYPE_AINT128:
            case CAM_TYPE_AUINT128:
              printf("%s[not supported]", pSpacer);
              break;

          }     // switch
        }       // for
    }           // if

    // if the array appears to be a null-terminated string of
    // non-Unicode characters, it will be displayed as a string;
    // otherwise, it will be displayed as a series of hex characters

    else {
        for (ctr=0, ptr=(uint32_t*)value; ctr < cntValue; ctr++, ptr++)
            if (*ptr < 0x20 || *ptr >= 0xff)
                break;

        if (ctr == cntValue-1 && *ptr == 0) {
            char *  pBuf;
            char    buf[256];

            if (cntValue > 256)
                cntValue = 256;
            for (ctr=0, pBuf=buf, ptr=(uint32_t*)value;
                                ctr < cntValue; ctr++, pBuf++, ptr++)
                *pBuf = (char)*ptr;

            buf[cntValue-1] = 0;
            printf( "%s", buf);
        }
        else {
            uint32_t    cnt;

            printf( "%i value%s", cntValue, (cntValue == 1 ? "" : "s"));
            for (ctr=0, cnt=(cntValue & ~7); ctr < cnt; ctr+=8, ptr+=8)
                printf( "\n                   %02X %02X %02X %02X %02X %02X %02X %02X",
                        (uint8_t)ptr[0], (uint8_t)ptr[1], (uint8_t)ptr[2], (uint8_t)ptr[3],
                        (uint8_t)ptr[4], (uint8_t)ptr[5], (uint8_t)ptr[6], (uint8_t)ptr[7]);
            if (cntValue & 7) {
                printf( "\n                  ");
                for (ctr=0, cnt=(cntValue & 7); ctr < cnt; ctr++, ptr++)
                    printf( " %02X", (uint8_t)*ptr);
            }
        }
    }

    return;
}

/***************************************************************************/

uint32_t set_property( CAMHandle hCam, uint32_t property,
                       char* value, uint32_t datatype)
{
    uint32_t    rtn = 0;
    uint32_t    data;
    char *      ptr;
    char *      pBuf = 0;
    CAMPropertyValue cam;

do {
    cam.DataType = datatype;
    cam.cntValue = 1;
    cam.Value    = (char*)&data;

    if (datatype == CAM_TYPE_STR) {
        cam.Value = value;
        break;
    }

    if (datatype == CAM_TYPE_AINT8 || datatype == CAM_TYPE_AUINT8) {
        uint32_t    ctr;
        uint32_t *  p32;

        cam.cntValue = strlen( value);
        if (cam.cntValue == 0) {
            cam.Value = 0;
            break;
        }
        cam.cntValue++;

        pBuf = malloc( cam.cntValue * sizeof(uint32_t));
        if (!pBuf) {
            rtn = CAMERR_MALLOC;
            break;
        }
        memset( pBuf, 0, cam.cntValue * sizeof(uint32_t));

        for (ctr = 0, p32 = (uint32_t*)pBuf; ctr < cam.cntValue; ctr++)
            *p32++ = (uint32_t)*value++;

        cam.Value = pBuf;
        break;
    }

    // avoid interpreting value as octal
    for (ptr = value; *ptr == '0'; )
        ptr++;
    if (ptr != value && (*ptr == 'x' || *ptr == 'X'))
        ptr--;

    if (datatype == CAM_TYPE_INT8 || datatype == CAM_TYPE_INT16 ||
                                     datatype == CAM_TYPE_INT32) {
        data = (uint32_t)strtol( ptr, 0, 0);
        break;
    }

    if (datatype == CAM_TYPE_UINT8 || datatype == CAM_TYPE_UINT16 ||
                                      datatype == CAM_TYPE_UINT32) {
        data = strtoul( ptr, 0, 0);
        break;
    }

    rtn = CAMERR_BADPARM;

} while (0);

    if (!rtn)
        rtn = SetPropertyValue( hCam, property, &cam);

    if (pBuf)
        free( pBuf);

    return rtn;
}

/***************************************************************************/

uint32_t    validate_property( CAMDeviceInfoPtr pDevInfo, uint32_t property)
{
    uint32_t    ctr;

    for (ctr = 0; ctr < pDevInfo->cntDevicePropertiesSupported; ctr++)
        if (pDevInfo->DevicePropertiesSupported[ctr] == property)
            return 0;

    return CAMERR_NOTSUPPORTED;
}

/***************************************************************************/
/***************************************************************************/
/*
    show file info

    list_files
    show_handle_info
        validate_range

*/
/***************************************************************************/

void list_handles( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo)
{
    int                i;
    CAMObjectHandlePtr pHandles = 0;

    if (GetObjectHandles( hCam, 0xffffffff, 0, 0, &pHandles)) {
        camcli_error( "Could not get object handles");
        return;
    }

    printf( "\n Handle list  %s\n", pDevInfo->Model);
    printf(   " ===========  %s\n", underline( pDevInfo->Model));
    printf( "\n Handles      Count= %d\n", pHandles->cntHandles);

    for (i = 0; i < pHandles->cntHandles; i++)
        printf( " % 4d\n", pHandles->Handles[i]);

    if (pHandles)
        free( pHandles);

    return;
}

/***************************************************************************/

void list_files( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo)
{
    int                i;
    struct tm *        ptm;
    CAMObjectHandlePtr pHandles = 0;
    CAMObjectInfoPtr   pObjInfo = 0;

    if (GetObjectHandles( hCam, 0xffffffff, 0, 0, &pHandles)) {
        camcli_error( "Could not get object handles");
        return;
    }

    printf( "\n File list  %s\n", pDevInfo->Model);
    printf(   " =========  %s\n", underline( pDevInfo->Model));
    printf("\n Handle\t     Size     Date     Time   Name\n");

    for (i = 0; i < pHandles->cntHandles; i++) {

        if (GetObjectInfo( hCam, pHandles->Handles[i], &pObjInfo)) {
            camcli_error( "Could not get object info for handle %d",
                          pHandles->Handles[i]);
            break;
        }

        if (pObjInfo->ObjectFormat != PTP_OFC_Association) {
            ptm = localtime( &pObjInfo->CaptureDate);
            printf( " % 4d\t% 9d  %04d-%02d-%02d  %02d:%02d  %s\n",
                    pHandles->Handles[i], pObjInfo->ObjectCompressedSize,
                    ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                    ptm->tm_hour, ptm->tm_min, pObjInfo->Filename);
        }

        free( pObjInfo);
        pObjInfo = 0;
    }

    if (pObjInfo)
        free( pObjInfo);
    if (pHandles)
        free( pHandles);

    return;
}

/***************************************************************************/

void show_handle_info( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo, OPTS *pOpts)
{
    uint32_t       *pHandle;
    uint32_t       *pEnd;
    struct tm       *ptm;
    CAMObjectHandlePtr pHandles = 0;
    CAMObjectInfoPtr   pObjInfo = 0;

    if (GetObjectHandles( hCam, 0xffffffff, 0, 0, &pHandles)) {
        camcli_error( "Could not get object handles");
        return;
    }

    if (!validate_range( pHandles, pOpts, &pHandle, &pEnd)) {
        if (pHandles)
            free( pHandles);
        return;
    }

    printf ( "\n     File information  %s\n", pDevInfo->Model);
    printf (   "     ================  %s\n", underline( pDevInfo->Model));

    for (; pHandle <= pEnd; pHandle++) {

        if (GetObjectInfo( hCam, *pHandle, &pObjInfo)) {
            camcli_error( "Could not get object info for handle %d", *pHandle);
            continue;
        }

        printf ( "              Handle:  %04d\n", *pHandle);
        printf ( "            Filename:  %s\n", (pObjInfo->Filename ? pObjInfo->Filename : "[none]"));

        if (pObjInfo->CaptureDate == 0)
            ptm = 0;
        else
            ptm = localtime( &pObjInfo->CaptureDate);
        if (!ptm)
            printf ( "         CaptureDate:  [unknown]\n");
        else
            printf ( "         CaptureDate:  %04d-%02d-%02d  %02d:%02d:%02d\n",
                     ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                     ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

        if (pObjInfo->ModificationDate == 0)
            ptm = 0;
        else
            ptm = localtime( &pObjInfo->ModificationDate);
        if (!ptm)
            printf ( "    ModificationDate:  [unknown]\n");
        else
            printf ( "    ModificationDate:  %04d-%02d-%02d  %02d:%02d:%02d\n",
                     ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
                     ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

        printf ( "            Keywords:  %s\n", (pObjInfo->Keywords ? pObjInfo->Keywords : "[none]"));
        printf ( "    ProtectionStatus:  0x%04X\t%s\n", pObjInfo->ProtectionStatus, GetCodeName( pObjInfo->ProtectionStatus, CAM_PROTECTION));
        printf ( "ObjectCompressedSize:  %d\n", pObjInfo->ObjectCompressedSize);
        printf ( "        ObjectFormat:  0x%04X\t%s\n", pObjInfo->ObjectFormat, GetCodeName( pObjInfo->ObjectFormat, CAM_OBJECT_FORMAT));
        printf ( "       ImagePixWidth:  %d\n", pObjInfo->ImagePixWidth);
        printf ( "      ImagePixHeight:  %d\n", pObjInfo->ImagePixHeight);
        printf ( "       ImageBitDepth:  %d\n", pObjInfo->ImageBitDepth);
        printf ( " ThumbCompressedSize:  %d\n", pObjInfo->ThumbCompressedSize);
        printf ( "         ThumbFormat:  0x%04X\t%s\n", pObjInfo->ThumbFormat, GetCodeName( pObjInfo->ThumbFormat, CAM_OBJECT_FORMAT));
        printf ( "       ThumbPixWidth:  %d\n", pObjInfo->ThumbPixWidth);
        printf ( "      ThumbPixHeight:  %d\n", pObjInfo->ThumbPixHeight);
        printf ( "        ParentObject:  %04d\n", pObjInfo->ParentObject);
        printf ( "     AssociationType:  0x%04X\t%s\n", pObjInfo->AssociationType, GetCodeName( pObjInfo->AssociationType, CAM_ASSOCIATION));
        printf ( "     AssociationDesc:  0x%04X\n", pObjInfo->AssociationDesc);
        printf ( "      SequenceNumber:  0x%04X\n", pObjInfo->SequenceNumber);
        printf ( "           StorageID:  0x%04X\n\n", pObjInfo->StorageID);

        free( pObjInfo);
        pObjInfo = 0;
    }

    if (pObjInfo)
        free( pObjInfo);
    if (pHandles)
        free( pHandles);

    return;
}

/***************************************************************************/

int validate_range( CAMObjectHandlePtr pHandles, OPTS *pOpts,
                    uint32_t **ppFirst, uint32_t **ppLast)
{
    int fErr;

    *ppFirst = pHandles->Handles;
    *ppLast = &(*ppFirst)[pHandles->cntHandles - 1];

    // 0 means "start at the first handle"
    if (pOpts->first == 0)
        pOpts->first = **ppFirst;

    // -1 means "end at the last handle"
    if (pOpts->last == ~0)
        pOpts->last = **ppLast;

    // are we completely out of range?
    if (pOpts->first > **ppLast || pOpts->last < **ppFirst) {
        camcli_error( "Invalid range specified - must be from %d thru %d",
                      **ppFirst, **ppLast);
        return 0;
    }

    // are we partially out of range?
    fErr = 0;
    if (pOpts->first < **ppFirst) {
        pOpts->first = **ppFirst;
        fErr = 1;
    }
    if (pOpts->last > **ppLast) {
        pOpts->last = **ppLast;
        fErr = 1;
    }
    if (fErr)
        camcli_error( "Invalid range specified - changing it to %d thru %d",
                      pOpts->first, pOpts->last);

    // adjust pointers to beginning & end of range
    while (**ppFirst < pOpts->first)
        (*ppFirst)++;
    while (**ppLast > pOpts->last)
        (*ppLast)--;

    return 1;
}

/***************************************************************************/
/***************************************************************************/
/*
    get / delete files

    delete_files
    get_files
        parse_filename
        make_filename

*/
/***************************************************************************/

void delete_files( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo, OPTS *pOpts)
{
    int             ret;
    uint32_t       *pHandle;
    uint32_t       *pEnd;
    CAMObjectHandlePtr pHandles = 0;
    CAMObjectInfoPtr   pObjInfo = 0;

    if (GetObjectHandles( hCam, 0xffffffff, 0, 0, &pHandles)) {
        camcli_error( "Could not get object handles");
        return;
    }

    if (!validate_range( pHandles, pOpts, &pHandle, &pEnd)) {
        if (pHandles)
            free( pHandles);
        return;
    }

    printf ( "\n Erase files  %s\n", pDevInfo->Model);
    printf (   " ===========  %s\n", underline( pDevInfo->Model));

    for (; pHandle <= pEnd; pHandle++) {
        ret = GetObjectInfo( hCam, *pHandle, &pObjInfo);
        if (ret) {
            camcli_error( "Could not get info for handle %d", *pHandle);
            if (ret == CAMERR_USBFAILURE)
                ClearStall( hCam);
            continue;
        }

        if (pObjInfo->ObjectFormat == PTP_OFC_Association)
            printf( " Handle %d is not a file - skipping...\n", *pHandle);
        else {
            ret = DeleteObject( hCam, *pHandle, 0);
            if (ret == 0)
                printf(" Erasing %3d  (%s)... done\n", *pHandle, pObjInfo->Filename);
            else {
                camcli_error( "Could not delete handle %d - rc= %X",
                              *pHandle, ret);
                if (ret == CAMERR_USBFAILURE)
                    ClearStall( hCam);
            }
        }

        free( pObjInfo);
        pObjInfo = 0;
    }

    if (pObjInfo)
        free( pObjInfo);
    if (pHandles)
        free( pHandles);

    return;
}

/***************************************************************************/

void get_files( CAMHandle hCam, CAMDeviceInfoPtr pDevInfo, OPTS *pOpts)
{
    int             ret;
    int             fName;
    char           *pName;
    uint32_t       *pHandle;
    uint32_t       *pEnd;
    CAMObjectHandlePtr pHandles = 0;
    CAMObjectInfoPtr   pObjInfo = 0;
    FNSTRUCT        fns;
    char            path[260];

    if (GetObjectHandles( hCam, 0xffffffff, 0, 0, &pHandles)) {
        camcli_error( "Could not get object handles");
        return;
    }

    fName = parse_filename( pOpts->value, &fns);
    if (fName)
        pName = path;
    else
        pName = 0;

    if (!validate_range( pHandles, pOpts, &pHandle, &pEnd)) {
        if (pHandles)
            free( pHandles);
        return;
    }

    if (pOpts->flags & OPT_THUMBNAIL) {
        printf ( "\n Save thumbnails  %s\n", pDevInfo->Model);
        printf (   " ===============  %s\n", underline( pDevInfo->Model));
    }
    else {
        printf ( "\n Save files  %s\n", pDevInfo->Model);
        printf (   " ==========  %s\n", underline( pDevInfo->Model));
    }

    for (; pHandle <= pEnd; pHandle++) {

        ret = GetObjectInfo( hCam, *pHandle, &pObjInfo);
        if (ret) {
            camcli_error( "Could not get info for handle %d", *pHandle);
            if (ret == CAMERR_USBFAILURE)
                ClearStall( hCam);
            continue;
        }

        if (pObjInfo->ObjectFormat == PTP_OFC_Association)
            printf( " Handle %d is not a file - skipping...\n", *pHandle);
        else {
            if (fName)
                make_filename( pName, pObjInfo->Filename, &fns);
            ret = os2_get_file( hCam, pObjInfo, *pHandle, pName,
                               (pOpts->flags & OPT_REPLACE),
                               (pOpts->flags & OPT_THUMBNAIL));

            if (ret == CAMERR_USBFAILURE)
                ClearStall( hCam);
        }

        free( pObjInfo);
        pObjInfo = 0;
    }

    if (pObjInfo)
        free( pObjInfo);
    if (pHandles)
        free( pHandles);

    return;
}

/***************************************************************************/

int parse_filename( char *filename, FNSTRUCT *fns)
{
    char *  pBksl;
    char *  pDot;
    char *  ptr;
    char *  pOut;
    uint    cnt;
    uint    posCtr;

    if (!filename || (filename[0] == '*' && filename[1] == 0) ||
        strcmp( filename, "*.*") == 0)
        return 0;

do {
    memset( fns, 0, sizeof(FNSTRUCT));

    pBksl = strrchr( filename, '\\');
    if (!pBksl)
        pBksl = filename;
    else
        pBksl++;

    pDot = strrchr( pBksl, '.');
    if (pDot) {
        if (pDot == pBksl)
            pDot = 0;
        else
            *pDot++ = 0;
    }

    pOut = fns->format;

    cnt = pBksl - filename;
    if (cnt) {
        memcpy( pOut, filename, cnt);
        pOut += cnt;
    }

    posCtr = 0;
    ptr = pBksl;
    while (*ptr) {

        if (*ptr == '*') {
            ptr++;
            if (fns->namePos == 0) {
                fns->namePos = ++posCtr;
                *pOut++ = '%';
                *pOut++ = 's';
            }
            continue;
        }

        if (*ptr == '#' && fns->nbrPos == 0) {
            ptr++;
            fns->nbrPos = ++posCtr;

            fns->seqNbr = atol( ptr);
            if (fns->seqNbr == 0)
                fns->seqNbr = 1;

            cnt = 0;
            while (*ptr >= '0' && *ptr <= '9' ) {
                cnt++;
                ptr++;
            }
            if (cnt == 0)
                cnt++;
            pOut += sprintf( pOut, "%%0%dd", cnt);
            continue;
        }

        *pOut++ = *ptr++;
    }

    if (ptr == pBksl) {
        strcpy( pOut, "%s.%s");
        fns->namePos = ++posCtr;
        fns->extPos = ++posCtr;
        break;
    }

    if (!pDot || *pDot == 0) {
        fns->extPos = ++posCtr;
        strcpy( pOut, ".%s");
        break;
    }

    *pOut++ = '.';
    while (*pDot) {
        if (*pDot == '*') {
            pDot++;
            if (fns->extPos == 0) {
                fns->extPos = ++posCtr;
                *pOut++ = '%';
                *pOut++ = 's';
            }
        }
        else
            *pOut++ = *pDot++;
    }
    *pOut = 0;

} while (0);

    return 1;
}

/***************************************************************************/

void make_filename( char* pOut, char* pIn, FNSTRUCT *fns)
{
    char *  pExt;
    char *  x1 = "";
    char *  x2 = "";
    char *  x3 = "";

    pExt = strrchr( pIn, '.');
    if (pExt)
        *pExt = 0;

    if (fns->namePos == 1)
        x1 = pIn;
    else
    if (fns->namePos == 2)
        x2 = pIn;

    if (fns->nbrPos == 1)
        x1 = (char*)fns->seqNbr++;
    else
    if (fns->nbrPos == 2)
        x2 = (char*)fns->seqNbr++;

    if (pExt) {
        if (fns->extPos == 1)
            x1 = pExt+1;
        else
        if (fns->extPos == 2)
            x2 = pExt+1;
        else
        if (fns->extPos == 3)
            x3 = pExt+1;
    }

    sprintf( pOut, fns->format, x1, x2, x3);

    if (pExt)
        *pExt = '.';

    return;
}

/***************************************************************************/
/***************************************************************************/
/*
    camcli_error
    camcli_debug
*/
/***************************************************************************/
#if 0
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

    if (verbose < 2)
        return;

    strcpy( buf, "PTPRO: ");
    strcat( buf, format);
    strcat( buf, "\n");

    va_start( args, format);
    vfprintf( stderr, buf, args);
    fflush( stderr);
    va_end (args);
}
#endif
/***************************************************************************/

#pragma pack()

/***************************************************************************/

