/***************************************************************************/
//  ptprostrings.c
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

#include "ptprousb.h"

#pragma pack(1)

/***************************************************************************/

typedef struct _CAMCodeName
{
    uint32_t    code;
    const char *name;
} CAMCodeName;

/***************************************************************************/
// Errors
/***************************************************************************/

CAMCodeName ptp_errors[] = {
    {CAMERR_OK,                         "OK"},
    {CAMERR_MALLOC,                     "memory allocation failed"},
    {CAMERR_USBINIT,                    "USB initialization failed"},
    {CAMERR_USBENUM,                    "device enumeration failed"},
    {CAMERR_NOCAMERAS,                  "no cameras found"},
    {CAMERR_OPENDEVICE,                 "unable to open device"},
    {CAMERR_OPENSESSION,                "unable to open session"},
    {CAMERR_NOTSUPPORTED,               "not supported"},
    {CAMERR_BADPARM,                    "bad parameter"},
    {CAMERR_NODATA,                     "no data received"},
    {CAMERR_NORESPONSE,                 "no response"},
    {CAMERR_USBFAILURE,                 "USB failure"},
    {PTP_RC_Undefined,                  "Undefined Error"},
    {PTP_RC_OK,                         "OK"},
    {PTP_RC_GeneralError,               "General Error"},
    {PTP_RC_SessionNotOpen,             "Session Not Open"},
    {PTP_RC_InvalidTransactionID,       "Invalid Transaction ID"},
    {PTP_RC_OperationNotSupported,      "Operation Not Supported"},
    {PTP_RC_ParameterNotSupported,      "Parameter Not Supported"},
    {PTP_RC_IncompleteTransfer,         "Incomplete Transfer"},
    {PTP_RC_InvalidStorageId,           "Invalid Storage ID"},
    {PTP_RC_InvalidObjectHandle,        "Invalid Object Handle"},
    {PTP_RC_DevicePropNotSupported,     "Device Prop Not Supported"},
    {PTP_RC_InvalidObjectFormatCode,    "Invalid Object Format Code"},
    {PTP_RC_StoreFull,                  "Store Full"},
    {PTP_RC_ObjectWriteProtected,       "Object Write Protected"},
    {PTP_RC_StoreReadOnly,              "Store Read Only"},
    {PTP_RC_AccessDenied,               "Access Denied"},
    {PTP_RC_NoThumbnailPresent,         "No Thumbnail Present"},
    {PTP_RC_SelfTestFailed,             "Self Test Failed"},
    {PTP_RC_PartialDeletion,            "Partial Deletion"},
    {PTP_RC_StoreNotAvailable,          "Store Not Available"},
    {PTP_RC_SpecificationByFormatUnsupported,
                        "Specification By Format Unsupported"},
    {PTP_RC_NoValidObjectInfo,          "No Valid Object Info"},
    {PTP_RC_InvalidCodeFormat,          "Invalid Code Format"},
    {PTP_RC_UnknownVendorCode,          "Unknown Vendor Code"},
    {PTP_RC_CaptureAlreadyTerminated,   "Capture Already Terminated"},
    {PTP_RC_DeviceBusy,                 "Device Bus"},
    {PTP_RC_InvalidParentObject,        "Invalid Parent Object"},
    {PTP_RC_InvalidDevicePropFormat,    "Invalid Device Prop Format"},
    {PTP_RC_InvalidDevicePropValue,     "Invalid Device Prop Value"},
    {PTP_RC_InvalidParameter,           "Invalid Parameter"},
    {PTP_RC_SessionAlreadyOpened,       "Session Already Opened"},
    {PTP_RC_TransactionCanceled,        "Transaction Canceled"},
    {PTP_RC_SpecificationOfDestinationUnsupported,
                        "Specification Of Destination Unsupported"},
    {0, NULL}
};

CAMCodeName ptp_errors_EK[] = {
    {PTP_RC_EK_FilenameRequired,        "EK: Filename Required"},
    {PTP_RC_EK_FilenameConflicts,       "EK: Filename Conflicts"},
    {PTP_RC_EK_FilenameInvalid,         "EK: Filename Invalid"},
    {0, NULL}
};

/***************************************************************************/
// operations
/***************************************************************************/

CAMCodeName ptp_operations[] = {
    {PTP_OC_Undefined,                  "UndefinedOperation"},
    {PTP_OC_GetDeviceInfo,              "GetDeviceInfo"},
    {PTP_OC_OpenSession,                "OpenSession"},
    {PTP_OC_CloseSession,               "CloseSession"},
    {PTP_OC_GetStorageIDs,              "GetStorageIDs"},
    {PTP_OC_GetStorageInfo,             "GetStorageInfo"},
    {PTP_OC_GetNumObjects,              "GetNumObjects"},
    {PTP_OC_GetObjectHandles,           "GetObjectHandles"},
    {PTP_OC_GetObjectInfo,              "GetObjectInfo"},
    {PTP_OC_GetObject,                  "GetObject"},
    {PTP_OC_GetThumb,                   "GetThumb"},
    {PTP_OC_DeleteObject,               "DeleteObject"},
    {PTP_OC_SendObjectInfo,             "SendObjectInfo"},
    {PTP_OC_SendObject,                 "SendObject"},
    {PTP_OC_InitiateCapture,            "InitiateCapture"},
    {PTP_OC_FormatStore,                "FormatStore"},
    {PTP_OC_ResetDevice,                "ResetDevice"},
    {PTP_OC_SelfTest,                   "SelfTest"},
    {PTP_OC_SetObjectProtection,        "SetObjectProtection"},
    {PTP_OC_PowerDown,                  "PowerDown"},
    {PTP_OC_GetDevicePropDesc,          "GetDevicePropDesc"},
    {PTP_OC_GetDevicePropValue,         "GetDevicePropValue"},
    {PTP_OC_SetDevicePropValue,         "SetDevicePropValue"},
    {PTP_OC_ResetDevicePropValue,       "ResetDevicePropValue"},
    {PTP_OC_TerminateOpenCapture,       "TerminateOpenCapture"},
    {PTP_OC_MoveObject,                 "MoveObject"},
    {PTP_OC_CopyObject,                 "CopyObject"},
    {PTP_OC_GetPartialObject,           "GetPartialObject"},
    {PTP_OC_InitiateOpenCapture,        "InitiateOpenCapture"},
    {0, NULL}
};

CAMCodeName ptp_operations_EK[] = {
    {PTP_OC_EK_SendFileObjectInfo,      "EK SendFileObjectInfo"},
    {PTP_OC_EK_SendFileObject,          "EK SendFileObject"},
    {0, NULL}
};

CAMCodeName ptp_operations_CANON[] = {
    {PTP_OC_CANON_GetObjectSize,        "CANON GetObjectSize"},
    {PTP_OC_CANON_StartShootingMode,    "CANON StartShootingMode"},
    {PTP_OC_CANON_EndShootingMode,      "CANON EndShootingMode"},
    {PTP_OC_CANON_ViewfinderOn,         "CANON ViewfinderOn"},
    {PTP_OC_CANON_ViewfinderOff,        "CANON ViewfinderOff"},
    {PTP_OC_CANON_ReflectChanges,       "CANON ReflectChanges"},
    {PTP_OC_CANON_CheckEvent,           "CANON CheckEvent"},
    {PTP_OC_CANON_FocusLock,            "CANON FocusLock"},
    {PTP_OC_CANON_FocusUnlock,          "CANON FocusUnlock"},
    {PTP_OC_CANON_InitiateCaptureInMemory,
                        "CANON InitiateCaptureInMemory"},
    {PTP_OC_CANON_GetPartialObject,     "CANON GetPartialObject"},
    {PTP_OC_CANON_GetViewfinderImage,   "CANON GetViewfinderImage"},
    {PTP_OC_CANON_GetChanges,           "CANON GetChanges"},
    {PTP_OC_CANON_GetFolderEntries,     "CANON GetFolderEntries"},
    {0, NULL}
};

/***************************************************************************/
// properties
/***************************************************************************/

CAMCodeName ptp_device_properties[] = {
    {PTP_DPC_Undefined,                 "Undefined"},
    {PTP_DPC_BatteryLevel,              "Battery Level"},
    {PTP_DPC_FunctionalMode,            "Functional Mode"},
    {PTP_DPC_ImageSize,                 "Image Size"},
    {PTP_DPC_CompressionSetting,        "Compression Setting"},
    {PTP_DPC_WhiteBalance,              "White Balance"},
    {PTP_DPC_RGBGain,                   "RGB Gain"},
    {PTP_DPC_FNumber,                   "F-Number"},
    {PTP_DPC_FocalLength,               "Focal Length"},
    {PTP_DPC_FocusDistance,             "Focus Distance"},
    {PTP_DPC_FocusMode,                 "Focus Mode"},
    {PTP_DPC_ExposureMeteringMode,      "Exposure Metering Mode"},
    {PTP_DPC_FlashMode,                 "Flash Mode"},
    {PTP_DPC_ExposureTime,              "Exposure Time"},
    {PTP_DPC_ExposureProgramMode,       "Exposure Program Mode"},
    {PTP_DPC_ExposureIndex,             "Exposure Index (film speed ISO)"},
    {PTP_DPC_ExposureBiasCompensation,  "Exposure Bias Compensation"},
    {PTP_DPC_DateTime,                  "Date Time"},
    {PTP_DPC_CaptureDelay,              "Pre-Capture Delay"},
    {PTP_DPC_StillCaptureMode,          "Still Capture Mode"},
    {PTP_DPC_Contrast,                  "Contrast"},
    {PTP_DPC_Sharpness,                 "Sharpness"},
    {PTP_DPC_DigitalZoom,               "Digital Zoom"},
    {PTP_DPC_EffectMode,                "Effect Mode"},
    {PTP_DPC_BurstNumber,               "Burst Number"},
    {PTP_DPC_BurstInterval,             "Burst Interval"},
    {PTP_DPC_TimelapseNumber,           "Timelapse Number"},
    {PTP_DPC_TimelapseInterval,         "Timelapse Interval"},
    {PTP_DPC_FocusMeteringMode,         "Focus Metering Mode"},
    {PTP_DPC_UploadURL,                 "Upload URL"},
    {PTP_DPC_Artist,                    "Artist"},
    {PTP_DPC_CopyrightInfo,             "Copyright Info"},
    {0, NULL}
};

CAMCodeName ptp_device_properties_EK[] = {
    {PTP_DPC_EK_ColorTemperature,       "EK Color Temperature"},
    {PTP_DPC_EK_DateTimeStampFormat,    "EK Date Time Stamp Format"},
    {PTP_DPC_EK_BeepMode,               "EK Beep Mode"},
    {PTP_DPC_EK_VideoOut,               "EK Video Out"},
    {PTP_DPC_EK_PowerSaving,            "EK Power Saving"},
    {PTP_DPC_EK_UI_Language,            "EK UI Language"},
    {0, NULL}
};

CAMCodeName ptp_device_properties_CANON[] = {
    {PTP_DPC_CANON_BeepMode,            "CANON Beep Mode"},
    {PTP_DPC_CANON_ViewfinderMode,      "CANON Viewfinder Mode"},
    {PTP_DPC_CANON_ImageQuality,        "CANON Image Quality"},
    {PTP_DPC_CANON_ImageSize,           "CANON Image Size"},
    {PTP_DPC_CANON_FlashMode,           "CANON Flash Mode"},
    {PTP_DPC_CANON_TvAvSetting,         "CANON TvAv Setting"},
    {PTP_DPC_CANON_MeteringMode,        "CANON Metering Mode"},
    {PTP_DPC_CANON_MacroMode,           "CANON Macro Mode"},
    {PTP_DPC_CANON_FocusingPoint,       "CANON Focusing Point"},
    {PTP_DPC_CANON_WhiteBalance,        "CANON White Balance"},
    {PTP_DPC_CANON_ISOSpeed,            "CANON ISO Speed"},
    {PTP_DPC_CANON_Aperture,            "CANON Aperture"},
    {PTP_DPC_CANON_ShutterSpeed,        "CANON Shutter Speed"},
    {PTP_DPC_CANON_ExpCompensation,     "CANON Exp Compensation"},
    {PTP_DPC_CANON_Zoom,                "CANON Zoom"},
    {PTP_DPC_CANON_SizeQualityMode,     "CANON Size Quality Mode"},
    {PTP_DPC_CANON_FlashMemory,         "CANON Flash Card Capacity"},
    {PTP_DPC_CANON_CameraModel,         "CANON Camera Model"},
    {PTP_DPC_CANON_CameraOwner,         "CANON Camera Owner"},
    {PTP_DPC_CANON_UnixTime,            "CANON Unix Time"},
    {PTP_DPC_CANON_RealImageWidth,      "CANON Real Image Width"},
    {PTP_DPC_CANON_PhotoEffect,         "CANON Photo Effect"},
    {PTP_DPC_CANON_AssistLight,         "CANON Assist Light"},
    {0, NULL}
};

// Nikon Codes added by Corey Manders and Mehreen Chaudary
CAMCodeName ptp_device_properties_NIKON[] = {
    {PTP_DPC_NIKON_ShootingBank,        "NIKON Shooting Bank"},
    {PTP_DPC_NIKON_ShootingBankNameA,   "NIKON Shooting Bank Name A"},
    {PTP_DPC_NIKON_ShootingBankNameB,   "NIKON Shooting Bank Name B"},
    {PTP_DPC_NIKON_ShootingBankNameC,   "NIKON Shooting Bank Name C"},
    {PTP_DPC_NIKON_ShootingBankNameD,   "NIKON Shooting Bank Name D"},
    {PTP_DPC_NIKON_RawCompression,      "NIKON Raw Compression"},
    {PTP_DPC_NIKON_WhiteBalanceAutoBias,
                                "NIKON White Balance Auto Bias"},
    {PTP_DPC_NIKON_WhiteBalanceTungstenBias,
                                "NIKON White Balance Tungsten Bias"},
    {PTP_DPC_NIKON_WhiteBalanceFlourescentBias,
                                "NIKON White Balance Flourescent Bias"},
    {PTP_DPC_NIKON_WhiteBalanceDaylightBias,
                                "NIKON White Balance Daylight Bias"},
    {PTP_DPC_NIKON_WhiteBalanceFlashBias,
                                "NIKON White Balance Flash Bias"},
    {PTP_DPC_NIKON_WhiteBalanceCloudyBias,
                                "NIKON White Balance Cloudy Bias"},
    {PTP_DPC_NIKON_WhiteBalanceShadeBias,
                                "NIKON White Balance Shade Bias"},
    {PTP_DPC_NIKON_WhiteBalanceColourTemperature,
                                "NIKON White Balance Colour Temperature"},
    {PTP_DPC_NIKON_ImageSharpening,     "NIKON Image Sharpening"},
    {PTP_DPC_NIKON_ToneCompensation,    "NIKON Tone Compensation"},
    {PTP_DPC_NIKON_ColourMode,          "NIKON Colour Mode"},
    {PTP_DPC_NIKON_HueAdjustment,       "NIKON Hue Adjustment"},
    {PTP_DPC_NIKON_NonCPULensDataFocalLength,
                                "NIKON Non CPU Lens Data Focal Length"},
    {PTP_DPC_NIKON_NonCPULensDataMaximumAperture,
                                "NIKON Non CPU Lens Data Maximum Aperture"},
    {PTP_DPC_NIKON_CSMMenuBankSelect,   "NIKON CSM Menu Bank Select"},
    {PTP_DPC_NIKON_MenuBankNameA,       "NIKON Menu Bank Name A"},
    {PTP_DPC_NIKON_MenuBankNameB,       "NIKON Menu Bank Name B"},
    {PTP_DPC_NIKON_MenuBankNameC,       "NIKON Menu Bank Name C"},
    {PTP_DPC_NIKON_MenuBankNameD,       "NIKON Menu Bank Name D"},
    {PTP_DPC_NIKON_A1AFCModePriority,   "NIKON (A1) AFC Mode Priority"},
    {PTP_DPC_NIKON_A2AFSModePriority,   "NIKON (A2) AFS Mode Priority"},
    {PTP_DPC_NIKON_A3GroupDynamicAF,    "NIKON (A3) Group Dynamic AF"},
    {PTP_DPC_NIKON_A4AFActivation,      "NIKON (A4) AF Activation"},    
    {PTP_DPC_NIKON_A5FocusAreaIllumManualFocus,
                                "NIKON (A5) Focus Area Illum Manual Focus"},
    {PTP_DPC_NIKON_FocusAreaIllumContinuous,
                                "NIKON Focus Area Illum Continuous"},
    {PTP_DPC_NIKON_FocusAreaIllumWhenSelected,
                                "NIKON Focus Area Illum When Selected"},
    {PTP_DPC_NIKON_A6FocusArea,         "NIKON (A6) Focus Area"},
    {PTP_DPC_NIKON_A7VerticalAFON,      "NIKON (A7) Vertical AF ON"},
    {PTP_DPC_NIKON_B1ISOAuto,           "NIKON (B1) ISO Auto"},
    {PTP_DPC_NIKON_B2ISOStep,           "NIKON (B2)    ISO Step"},
    {PTP_DPC_NIKON_B3EVStep,            "NIKON (B3) EV Step"},
    {PTP_DPC_NIKON_B4ExposureCompEv,    "NIKON (B4) Exposure Comp Ev"},
    {PTP_DPC_NIKON_B5ExposureComp,      "NIKON (B5) Exposure Comp"},
    {PTP_DPC_NIKON_B6CenterWeightArea,  "NIKON (B6) Center Weight Area"},
    {PTP_DPC_NIKON_C1AELock,            "NIKON (C1) AE Lock"},
    {PTP_DPC_NIKON_C2AELAFL,            "NIKON (C2) AE_L/AF_L"},
    {PTP_DPC_NIKON_C3AutoMeterOff,      "NIKON (C3) Auto Meter Off"},
    {PTP_DPC_NIKON_C4SelfTimer,         "NIKON (C4) Self Timer"},    
    {PTP_DPC_NIKON_C5MonitorOff,        "NIKON (C5) Monitor Off"},
    {PTP_DPC_NIKON_D1ShootingSpeed,     "NIKON (D1) Shooting Speed"},
    {PTP_DPC_NIKON_D2MaximumShots,      "NIKON (D2) Maximum Shots"},
    {PTP_DPC_NIKON_D3ExpDelayMode,      "NIKON (D3) ExpDelayMode"},    
    {PTP_DPC_NIKON_D4LongExposureNoiseReduction,
                                "NIKON (D4) Long Exposure Noise Reduction"},
    {PTP_DPC_NIKON_D5FileNumberSequence,
                                "NIKON (D5) File Number Sequence"},
    {PTP_DPC_NIKON_D6ControlPanelFinderRearControl,
                                "NIKON (D6) Control Panel Finder Rear Control"},
    {PTP_DPC_NIKON_ControlPanelFinderViewfinder,
                                "NIKON Control Panel Finder Viewfinder"},
    {PTP_DPC_NIKON_D7Illumination,      "NIKON (D7) Illumination"},
    {PTP_DPC_NIKON_E1FlashSyncSpeed,    "NIKON (E1) Flash Sync Speed"},
    {PTP_DPC_NIKON_E2FlashShutterSpeed, "NIKON (E2) Flash Shutter Speed"},
    {PTP_DPC_NIKON_E3AAFlashMode,       "NIKON (E3) AA Flash Mode"},
    {PTP_DPC_NIKON_E4ModelingFlash,     "NIKON (E4) Modeling Flash"},
    {PTP_DPC_NIKON_E5AutoBracketSet,    "NIKON (E5) Auto Bracket Set"},
    {PTP_DPC_NIKON_E6ManualModeBracketing,
                                "NIKON (E6) Manual Mode Bracketing"},
    {PTP_DPC_NIKON_E7AutoBracketOrder,  "NIKON (E7) Auto Bracket Order"},
    {PTP_DPC_NIKON_E8AutoBracketSelection,
                                "NIKON (E8) Auto Bracket Selection"},
    {PTP_DPC_NIKON_F1CenterButtonShootingMode,
                                "NIKON (F1) Center Button Shooting Mode"},
    {PTP_DPC_NIKON_CenterButtonPlaybackMode,
                                "NIKON Center Button Playback Mode"},
    {PTP_DPC_NIKON_F2Multiselector,     "NIKON (F2) Multiselector"},
    {PTP_DPC_NIKON_F3PhotoInfoPlayback, "NIKON (F3) PhotoInfoPlayback"},    
    {PTP_DPC_NIKON_F4AssignFuncButton,  "NIKON (F4) Assign Function Button"},
    {PTP_DPC_NIKON_F5CustomizeCommDials,"NIKON (F5) Customize Comm Dials"},
    {PTP_DPC_NIKON_ChangeMainSub,       "NIKON Change Main Sub"},
    {PTP_DPC_NIKON_ApertureSetting,     "NIKON Aperture Setting"},
    {PTP_DPC_NIKON_MenusAndPlayback,    "NIKON Menus and Playback"},
    {PTP_DPC_NIKON_F6ButtonsAndDials,   "NIKON (F6) Buttons and Dials"},
    {PTP_DPC_NIKON_F7NoCFCard,          "NIKON (F7) No CF Card"},
    {PTP_DPC_NIKON_AutoImageRotation,   "NIKON Auto Image Rotation"},
    {PTP_DPC_NIKON_ExposureBracketingOnOff,
                                "NIKON Exposure Bracketing On Off"},
    {PTP_DPC_NIKON_ExposureBracketingIntervalDist,
                                "NIKON Exposure Bracketing Interval Distance"},
    {PTP_DPC_NIKON_ExposureBracketingNumBracketPlace,
                                "NIKON Exposure Bracketing Number Bracket Place"},
    {PTP_DPC_NIKON_AutofocusLCDTopMode2,
                                "NIKON Autofocus LCD Top Mode 2"},
    {PTP_DPC_NIKON_AutofocusLCDTopMode3AndMode4,
                                "NIKON Autofocus LCD Top Mode 3 and Mode 4"},
    {PTP_DPC_NIKON_LightMeter,          "NIKON Light Meter"},
    {PTP_DPC_NIKON_ExposureApertureLock,"NIKON Exposure Aperture Lock"},
    {PTP_DPC_NIKON_MaximumShots,        "NIKON Maximum Shots"},
    {PTP_DPC_NIKON_Beep,                "NIKON AF Beep Mode"},
    {PTP_DPC_NIKON_AFC,                 "NIKON ??? AF Related"},
    {PTP_DPC_NIKON_AFLampOff,           "NIKON AF Lamp"},
    {PTP_DPC_NIKON_PADVPMode,           "NIKON Auto ISO P/A/DVP Setting"},
    {PTP_DPC_NIKON_ReviewOff,           "NIKON Image Review"},
    {PTP_DPC_NIKON_GridDisplay,         "NIKON Viewfinder Grid Display"},
    {PTP_DPC_NIKON_AFAreaIllumination,  "NIKON AF Area Illumination"},
    {PTP_DPC_NIKON_FlashMode,           "NIKON Flash Mode"},
    {PTP_DPC_NIKON_FlashPower,          "NIKON Flash Power"},
    {PTP_DPC_NIKON_FlashSignOff,        "NIKON Flash Sign"},
    {PTP_DPC_NIKON_FlashExposureCompensation,
                                "NIKON Flash Exposure Compensation"},
    {PTP_DPC_NIKON_RemoteTimeout,       "NIKON Remote Timeout"},
    {PTP_DPC_NIKON_ImageCommentString,  "NIKON Image Comment String"},
    {PTP_DPC_NIKON_FlashOpen,           "NIKON Flash Open"},
    {PTP_DPC_NIKON_FlashCharged,        "NIKON Flash Charged"},
    {PTP_DPC_NIKON_LensID,              "NIKON Lens ID"},
    {PTP_DPC_NIKON_FocalLengthMin,      "NIKON Min. Focal Length"},
    {PTP_DPC_NIKON_FocalLengthMax,      "NIKON Max. Focal Length"},
    {PTP_DPC_NIKON_MaxApAtMinFocalLength,
                                "NIKON Max. Aperture at Min. Focal Length"},
    {PTP_DPC_NIKON_MaxApAtMaxFocalLength,
                                "NIKON Max. Aperture at Max. Focal Length"},
    {PTP_DPC_NIKON_LowLight,            "NIKON Low Light"},
    {PTP_DPC_NIKON_ExtendedCSMMenu,     "NIKON Extended CSM Menu"},
    {PTP_DPC_NIKON_OptimiseImage,       "NIKON Optimise Image"},
    {0, NULL}
};

/***************************************************************************/
// miscellanea
/***************************************************************************/

// PTP Association Types
CAMCodeName ptp_association[] = {
    {PTP_AT_Undefined,                  "none"},
    {PTP_AT_GenericFolder,              "Generic Folder"},
    {PTP_AT_Album,                      "Album"},
    {PTP_AT_TimeSequence,               "Time Sequence"},
    {PTP_AT_HorizontalPanoramic,        "Horizontal Panoramic"},
    {PTP_AT_VerticalPanoramic,          "Vertical Panoramic"},
    {PTP_AT_2DPanoramic,                "2D Panoramic"},
    {PTP_AT_AncillaryData,              "Ancillary Data"},
    {0, NULL}
};
                                            
// PTP Protection Status
CAMCodeName ptp_protection[] = {
    {PTP_PS_NoProtection,               "No Protection"},
    {PTP_PS_ReadOnly,                   "Read Only"},
    {0, NULL}
};

// PTP Storage Types
CAMCodeName ptp_storagetype[] = {
    {PTP_ST_Undefined,                  "Undefined"},
    {PTP_ST_FixedROM,                   "Fixed ROM"},
    {PTP_ST_RemovableROM,               "Removable ROM"},
    {PTP_ST_FixedRAM,                   "Fixed RAM"},
    {PTP_ST_RemovableRAM,               "Removable RAM"},
    {0, NULL}
};

// PTP StorageInfo AccessCapability Values
CAMCodeName ptp_storageaccess[] = {
    {PTP_AC_ReadWrite,                  "Read Write"},
    {PTP_AC_ReadOnly,                   "Read Only"},
    {PTP_AC_ReadOnly_with_Object_Deletion, "Read Only with Object Deletion"},
    {0, NULL}
};

// PTP FilesystemType Values
CAMCodeName ptp_filesystem[] = {
    {PTP_FST_Undefined,                 "Undefined"},
    {PTP_FST_GenericFlat,               "Generic Flat"},
    {PTP_FST_GenericHierarchical,       "Generic Hierarchical"},
    {PTP_FST_DCF,                       "DCF"},
    {0, NULL}
};

// PTP Object Format Codes
CAMCodeName ptp_objectformat[] = {
    {PTP_OFC_EXIF_JPEG,                 "EXIF JPEG"},
    {PTP_OFC_TIFF_EP,                   "TIFF EP"},
    {PTP_OFC_FlashPix,                  "FlashPix"},
    {PTP_OFC_BMP,                       "BMP"},
    {PTP_OFC_CIFF,                      "CIFF"},
    {PTP_OFC_Undefined_0x3806,          "Undefined"},
    {PTP_OFC_GIF,                       "GIF"},
    {PTP_OFC_JFIF,                      "JFIF"},
    {PTP_OFC_PCD,                       "PCD"},
    {PTP_OFC_PICT,                      "PICT"},
    {PTP_OFC_PNG,                       "PNG"},
    {PTP_OFC_Undefined_0x380C,          "Undefined"},
    {PTP_OFC_TIFF,                      "TIFF"},
    {PTP_OFC_TIFF_IT,                   "TIFF IT"},
    {PTP_OFC_JP2,                       "JP2"},
    {PTP_OFC_JPX,                       "JPX"},
    {PTP_OFC_Undefined,                 "Undefined"},
    {PTP_OFC_Association,               "Association"},
    {PTP_OFC_Script,                    "Script"},
    {PTP_OFC_Executable,                "Executable"},
    {PTP_OFC_Text,                      "Text"},
    {PTP_OFC_HTML,                      "HTML"},
    {PTP_OFC_DPOF,                      "DPOF"},
    {PTP_OFC_AIFF,                      "AIFF"},
    {PTP_OFC_WAV,                       "WAV"},
    {PTP_OFC_MP3,                       "MP3"},
    {PTP_OFC_AVI,                       "AVI"},
    {PTP_OFC_MPEG,                      "MPEG"},
    {PTP_OFC_ASF,                       "ASF"},
    {PTP_OFC_QT,                        "QT"},  // guessing
    {PTP_OFC_EK_M3U,                    "Kodak M3U"},
    {0, NULL}
};

// PTP Data Type Codes
CAMCodeName ptp_datatype[] = {
    {CAM_TYPE_UNDEF,                    "undefined"},
    {CAM_TYPE_INT8,                     "int8"}, 
    {CAM_TYPE_UINT8,                    "uint8"},
    {CAM_TYPE_INT16,                    "int16"},
    {CAM_TYPE_UINT16,                   "uint16"},
    {CAM_TYPE_INT32,                    "int32"},
    {CAM_TYPE_UINT32,                   "uint32"},
    {CAM_TYPE_INT64,                    "int64"},
    {CAM_TYPE_UINT64,                   "uint64"},
    {CAM_TYPE_INT128,                   "int128"},
    {CAM_TYPE_UINT128,                  "uint128"},
    {CAM_TYPE_AINT8,                    "int8 array"},
    {CAM_TYPE_AUINT8,                   "uint8 array"},
    {CAM_TYPE_AINT16,                   "int16 array"},
    {CAM_TYPE_AUINT16,                  "uint16 array"},
    {CAM_TYPE_AINT32,                   "int32 array"},
    {CAM_TYPE_AUINT32,                  "uint32 array"},
    {CAM_TYPE_AINT64,                   "int64 array"},
    {CAM_TYPE_AUINT64,                  "uint64 array"},
    {CAM_TYPE_AINT128,                  "int128 array"},
    {CAM_TYPE_AUINT128,                 "uint128 array"},
    {CAM_TYPE_STR,                      "string"},
    {0, NULL}
};

/***************************************************************************/
// get miscellaneous names

const char* GetCodeName( uint32_t code, int type)

{
    CAMCodeName *c2n = 0;

    switch (type) {
        case CAM_OBJECT_FORMAT:
            c2n = ptp_objectformat;
            break;
        case CAM_ASSOCIATION:
            c2n = ptp_association;
            break;
        case CAM_PROTECTION:
            c2n = ptp_protection;
            break;
        case CAM_FILESYSTEM:
            c2n = ptp_filesystem;
            break;
        case CAM_STORAGE:
            c2n = ptp_storagetype;
            break;
        case CAM_STORAGE_ACCESS:
            c2n = ptp_storageaccess;
            break;
        case CAM_DATATYPE:
            c2n = ptp_datatype;
            break;
    }

    if (c2n) {
        for (; c2n->name; c2n++)
            if (c2n->code == code)
                return (c2n->name);
    }

    return "[Unknown]";
}

/***************************************************************************/
// return ptp error name

const char* GetErrorName( CAMDeviceInfoPtr pDevInfo, uint32_t code)

{
    CAMCodeName *c2n;

    for (c2n = ptp_errors; c2n->name; c2n++)
        if (c2n->code == code)
            return (c2n->name);

    if (pDevInfo && pDevInfo->VendorExtensionID == PTP_VENDOR_EASTMAN_KODAK)
        for (c2n = ptp_errors_EK; c2n->name; c2n++)
            if (c2n->code == code)
                return (c2n->name);

    return "[Unknown]";
}

/***************************************************************************/
// return ptp operation name

const char* GetOperationName( CAMDeviceInfoPtr pDevInfo, uint32_t code)

{
    CAMCodeName *c2n;

    for (c2n = ptp_operations; c2n->name; c2n++)
        if (c2n->code == code)
            return (c2n->name);

    if (pDevInfo)
        switch (pDevInfo->VendorExtensionID) {
            case PTP_VENDOR_EASTMAN_KODAK:
                for (c2n = ptp_operations_EK; c2n->name; c2n++)
                    if (c2n->code == code)
                        return (c2n->name);
                break;

            case PTP_VENDOR_CANON:
                for (c2n = ptp_operations_CANON; c2n->name; c2n++)
                    if (c2n->code == code)
                        return (c2n->name);
                break;
        }

    return "[Unknown]";
}

/***************************************************************************/
// return ptp property name

const char* GetPropertyName( CAMDeviceInfoPtr pDevInfo, uint32_t code)

{
    CAMCodeName *c2n;

    for (c2n = ptp_device_properties; c2n->name; c2n++)
        if (c2n->code == code)
            return (c2n->name);

    if (pDevInfo)
        switch (pDevInfo->VendorExtensionID) {
            case PTP_VENDOR_EASTMAN_KODAK:
                for (c2n = ptp_device_properties_EK; c2n->name; c2n++)
                    if (c2n->code == code)
                        return (c2n->name);
                break;
            case PTP_VENDOR_CANON:
                for (c2n = ptp_device_properties_CANON; c2n->name; c2n++)
                    if (c2n->code == code)
                        return (c2n->name);
                break;
            case PTP_VENDOR_NIKON:
                for (c2n = ptp_device_properties_NIKON; c2n->name; c2n++)
                    if (c2n->code == code)
                        return (c2n->name);
                break;
        }

    return "[Unknown]";
}
/***************************************************************************/

#pragma pack()

/***************************************************************************/

