/***************************************************************************/
//  ptprocodes.h
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

#ifndef _PTPROCODES_H_
#define _PTPROCODES_H_

#define CAMERR_OK                           0x0000
#define CAMERR_MALLOC                       0x0001
#define CAMERR_USBINIT                      0x0002
#define CAMERR_USBENUM                      0x0003
#define CAMERR_NOCAMERAS                    0x0004
#define CAMERR_OPENDEVICE                   0x0005
#define CAMERR_OPENSESSION                  0x0006
#define CAMERR_NOTSUPPORTED                 0x0007
#define CAMERR_BADPARM                      0x0008
#define CAMERR_NODATA                       0x0009
#define CAMERR_NORESPONSE                   0x000A
#define CAMERR_USBFAILURE                   0x000B
#define CAMERR_DEVICEBUSY                   0x000C
#define CAMERR_MISCELLANEOUS                0x000D

/***************************************************************************/
//  Response Codes
/***************************************************************************/

//  Response Codes
#define PTP_RC_Undefined                    0x2000
#define PTP_RC_OK                           0x2001
#define PTP_RC_GeneralError                 0x2002
#define PTP_RC_SessionNotOpen               0x2003
#define PTP_RC_InvalidTransactionID         0x2004
#define PTP_RC_OperationNotSupported        0x2005
#define PTP_RC_ParameterNotSupported        0x2006
#define PTP_RC_IncompleteTransfer           0x2007
#define PTP_RC_InvalidStorageId             0x2008
#define PTP_RC_InvalidObjectHandle          0x2009
#define PTP_RC_DevicePropNotSupported       0x200A
#define PTP_RC_InvalidObjectFormatCode      0x200B
#define PTP_RC_StoreFull                    0x200C
#define PTP_RC_ObjectWriteProtected         0x200D
#define PTP_RC_StoreReadOnly                0x200E
#define PTP_RC_AccessDenied                 0x200F
#define PTP_RC_NoThumbnailPresent           0x2010
#define PTP_RC_SelfTestFailed               0x2011
#define PTP_RC_PartialDeletion              0x2012
#define PTP_RC_StoreNotAvailable            0x2013
#define PTP_RC_SpecificationByFormatUnsupported 0x2014
#define PTP_RC_NoValidObjectInfo            0x2015
#define PTP_RC_InvalidCodeFormat            0x2016
#define PTP_RC_UnknownVendorCode            0x2017
#define PTP_RC_CaptureAlreadyTerminated     0x2018
#define PTP_RC_DeviceBusy                   0x2019
#define PTP_RC_InvalidParentObject          0x201A
#define PTP_RC_InvalidDevicePropFormat      0x201B
#define PTP_RC_InvalidDevicePropValue       0x201C
#define PTP_RC_InvalidParameter             0x201D
#define PTP_RC_SessionAlreadyOpened         0x201E
#define PTP_RC_TransactionCanceled          0x201F
#define PTP_RC_SpecificationOfDestinationUnsupported 0x2020

/* Eastman Kodak extension Response Codes */
#define PTP_RC_EK_FilenameRequired          0xA001
#define PTP_RC_EK_FilenameConflicts         0xA002
#define PTP_RC_EK_FilenameInvalid           0xA003

/* Proprietary vendor extension response code mask */
#define PTP_RC_EXTENSION_MASK               0xF000
#define PTP_RC_EXTENSION                    0xA000
                                            
/***************************************************************************/
//  Operation & Event Codes
/***************************************************************************/

/* Vendor IDs */
#define PTP_VENDOR_EASTMAN_KODAK        0x00000001
#define PTP_VENDOR_SEIKO_EPSON          0x00000002
#define PTP_VENDOR_AGILENT              0x00000003
#define PTP_VENDOR_POLAROID             0x00000004
#define PTP_VENDOR_AGFA_GEVAERT         0x00000005
#define PTP_VENDOR_MICROSOFT            0x00000006
#define PTP_VENDOR_EQUINOX              0x00000007
#define PTP_VENDOR_VIEWQUEST            0x00000008
#define PTP_VENDOR_STMICROELECTRONICS   0x00000009
#define PTP_VENDOR_NIKON                0x0000000A
#define PTP_VENDOR_CANON                0x0000000B

/* Operation Codes */
#define PTP_OC_Undefined                    0x1000
#define PTP_OC_GetDeviceInfo                0x1001
#define PTP_OC_OpenSession                  0x1002
#define PTP_OC_CloseSession                 0x1003
#define PTP_OC_GetStorageIDs                0x1004
#define PTP_OC_GetStorageInfo               0x1005
#define PTP_OC_GetNumObjects                0x1006
#define PTP_OC_GetObjectHandles             0x1007
#define PTP_OC_GetObjectInfo                0x1008
#define PTP_OC_GetObject                    0x1009
#define PTP_OC_GetThumb                     0x100A
#define PTP_OC_DeleteObject                 0x100B
#define PTP_OC_SendObjectInfo               0x100C
#define PTP_OC_SendObject                   0x100D
#define PTP_OC_InitiateCapture              0x100E
#define PTP_OC_FormatStore                  0x100F
#define PTP_OC_ResetDevice                  0x1010
#define PTP_OC_SelfTest                     0x1011
#define PTP_OC_SetObjectProtection          0x1012
#define PTP_OC_PowerDown                    0x1013
#define PTP_OC_GetDevicePropDesc            0x1014
#define PTP_OC_GetDevicePropValue           0x1015
#define PTP_OC_SetDevicePropValue           0x1016
#define PTP_OC_ResetDevicePropValue         0x1017
#define PTP_OC_TerminateOpenCapture         0x1018
#define PTP_OC_MoveObject                   0x1019
#define PTP_OC_CopyObject                   0x101A
#define PTP_OC_GetPartialObject             0x101B
#define PTP_OC_InitiateOpenCapture          0x101C

/* Eastman Kodak extension Operation Codes */
#define PTP_OC_EK_SendFileObjectInfo        0x9005
#define PTP_OC_EK_SendFileObject            0x9006

/* Canon extension Operation Codes */
#define PTP_OC_CANON_GetObjectSize          0x9001
#define PTP_OC_CANON_StartShootingMode      0x9008
#define PTP_OC_CANON_EndShootingMode        0x9009
#define PTP_OC_CANON_ViewfinderOn           0x900B
#define PTP_OC_CANON_ViewfinderOff          0x900C
#define PTP_OC_CANON_ReflectChanges         0x900D
#define PTP_OC_CANON_CheckEvent             0x9013
#define PTP_OC_CANON_FocusLock              0x9014
#define PTP_OC_CANON_FocusUnlock            0x9015
#define PTP_OC_CANON_InitiateCaptureInMemory 0x901A
#define PTP_OC_CANON_GetPartialObject       0x901B
#define PTP_OC_CANON_GetViewfinderImage     0x901d
#define PTP_OC_CANON_GetChanges             0x9020
#define PTP_OC_CANON_GetFolderEntries       0x9021

/* Proprietary vendor extension operations mask */
#define PTP_OC_EXTENSION_MASK               0xF000
#define PTP_OC_EXTENSION                    0x9000

/* PTP Event Codes */                       
#define PTP_EC_Undefined                    0x4000
#define PTP_EC_CancelTransaction            0x4001
#define PTP_EC_ObjectAdded                  0x4002
#define PTP_EC_ObjectRemoved                0x4003
#define PTP_EC_StoreAdded                   0x4004
#define PTP_EC_StoreRemoved                 0x4005
#define PTP_EC_DevicePropChanged            0x4006
#define PTP_EC_ObjectInfoChanged            0x4007
#define PTP_EC_DeviceInfoChanged            0x4008
#define PTP_EC_RequestObjectTransfer        0x4009
#define PTP_EC_StoreFull                    0x400A
#define PTP_EC_DeviceReset                  0x400B
#define PTP_EC_StorageInfoChanged           0x400C
#define PTP_EC_CaptureComplete              0x400D
#define PTP_EC_UnreportedStatus             0x400E

/* Canon extension Event Codes */
#define PTP_EC_CANON_DeviceInfoChanged      0xC008
#define PTP_EC_CANON_RequestObjectTransfer  0xC009
#define PTP_EC_CANON_CameraModeChanged      0xC00C

/***************************************************************************/
// Storage & Handles
/***************************************************************************/

/* PTP Storage Types */                     
#define PTP_ST_Undefined                    0x0000
#define PTP_ST_FixedROM                     0x0001
#define PTP_ST_RemovableROM                 0x0002
#define PTP_ST_FixedRAM                     0x0003
#define PTP_ST_RemovableRAM                 0x0004

/* PTP FilesystemType Values */             
#define PTP_FST_Undefined                   0x0000
#define PTP_FST_GenericFlat                 0x0001
#define PTP_FST_GenericHierarchical         0x0002
#define PTP_FST_DCF                         0x0003

/* PTP StorageInfo AccessCapability Values */
#define PTP_AC_ReadWrite                    0x0000
#define PTP_AC_ReadOnly                     0x0001
#define PTP_AC_ReadOnly_with_Object_Deletion 0x0002

#define PTP_HANDLER_SPECIAL             0xffffffff
#define PTP_HANDLER_ROOT                0x00000000

/***************************************************************************/
//  ObjectInfo & Format Codes
/***************************************************************************/

/* PTP Association Types */
#define PTP_AT_Undefined                    0x0000
#define PTP_AT_GenericFolder                0x0001
#define PTP_AT_Album                        0x0002
#define PTP_AT_TimeSequence                 0x0003
#define PTP_AT_HorizontalPanoramic          0x0004
#define PTP_AT_VerticalPanoramic            0x0005
#define PTP_AT_2DPanoramic                  0x0006
#define PTP_AT_AncillaryData                0x0007
                                            
/* PTP Protection Status */                 
#define PTP_PS_NoProtection                 0x0000
#define PTP_PS_ReadOnly                     0x0001

/* PTP Object Format Codes */
/* ancillary formats */
#define PTP_OFC_Undefined                   0x3000
#define PTP_OFC_Association                 0x3001
#define PTP_OFC_Script                      0x3002
#define PTP_OFC_Executable                  0x3003
#define PTP_OFC_Text                        0x3004
#define PTP_OFC_HTML                        0x3005
#define PTP_OFC_DPOF                        0x3006
#define PTP_OFC_AIFF                        0x3007
#define PTP_OFC_WAV                         0x3008
#define PTP_OFC_MP3                         0x3009
#define PTP_OFC_AVI                         0x300A
#define PTP_OFC_MPEG                        0x300B
#define PTP_OFC_ASF                         0x300C
#define PTP_OFC_QT                          0x300D /* guessing */

/* Eastman Kodak extension ancillary format */
#define PTP_OFC_EK_M3U                      0xb002

/* image formats */
#define PTP_OFC_EXIF_JPEG                   0x3801
#define PTP_OFC_TIFF_EP                     0x3802
#define PTP_OFC_FlashPix                    0x3803
#define PTP_OFC_BMP                         0x3804
#define PTP_OFC_CIFF                        0x3805
#define PTP_OFC_Undefined_0x3806            0x3806
#define PTP_OFC_GIF                         0x3807
#define PTP_OFC_JFIF                        0x3808
#define PTP_OFC_PCD                         0x3809
#define PTP_OFC_PICT                        0x380A
#define PTP_OFC_PNG                         0x380B
#define PTP_OFC_Undefined_0x380C            0x380C
#define PTP_OFC_TIFF                        0x380D
#define PTP_OFC_TIFF_IT                     0x380E
#define PTP_OFC_JP2                         0x380F
#define PTP_OFC_JPX                         0x3810

/***************************************************************************/
//  Device Property
/***************************************************************************/

/* Device Properties Codes */               
#define PTP_DPC_Undefined                   0x5000
#define PTP_DPC_BatteryLevel                0x5001
#define PTP_DPC_FunctionalMode              0x5002
#define PTP_DPC_ImageSize                   0x5003
#define PTP_DPC_CompressionSetting          0x5004
#define PTP_DPC_WhiteBalance                0x5005
#define PTP_DPC_RGBGain                     0x5006
#define PTP_DPC_FNumber                     0x5007
#define PTP_DPC_FocalLength                 0x5008
#define PTP_DPC_FocusDistance               0x5009
#define PTP_DPC_FocusMode                   0x500A
#define PTP_DPC_ExposureMeteringMode        0x500B
#define PTP_DPC_FlashMode                   0x500C
#define PTP_DPC_ExposureTime                0x500D
#define PTP_DPC_ExposureProgramMode         0x500E
#define PTP_DPC_ExposureIndex               0x500F
#define PTP_DPC_ExposureBiasCompensation    0x5010
#define PTP_DPC_DateTime                    0x5011
#define PTP_DPC_CaptureDelay                0x5012
#define PTP_DPC_StillCaptureMode            0x5013
#define PTP_DPC_Contrast                    0x5014
#define PTP_DPC_Sharpness                   0x5015
#define PTP_DPC_DigitalZoom                 0x5016
#define PTP_DPC_EffectMode                  0x5017
#define PTP_DPC_BurstNumber                 0x5018
#define PTP_DPC_BurstInterval               0x5019
#define PTP_DPC_TimelapseNumber             0x501A
#define PTP_DPC_TimelapseInterval           0x501B
#define PTP_DPC_FocusMeteringMode           0x501C
#define PTP_DPC_UploadURL                   0x501D
#define PTP_DPC_Artist                      0x501E
#define PTP_DPC_CopyrightInfo               0x501F

/* Proprietary vendor extension device property mask */
#define PTP_DPC_EXTENSION_MASK              0xF000
#define PTP_DPC_EXTENSION                   0xD000

/* Vendor Extensions device property codes */

/* Eastman Kodak extension device property codes */
#define PTP_DPC_EK_ColorTemperature         0xD001
#define PTP_DPC_EK_DateTimeStampFormat      0xD002
#define PTP_DPC_EK_BeepMode                 0xD003
#define PTP_DPC_EK_VideoOut                 0xD004
#define PTP_DPC_EK_PowerSaving              0xD005
#define PTP_DPC_EK_UI_Language              0xD006

/* Canon extension device property codes */
#define PTP_DPC_CANON_BeepMode              0xD001
#define PTP_DPC_CANON_ViewfinderMode        0xD003
#define PTP_DPC_CANON_ImageQuality          0xD006
#define PTP_DPC_CANON_D007                  0xD007
#define PTP_DPC_CANON_ImageSize             0xD008
#define PTP_DPC_CANON_FlashMode             0xD00A
#define PTP_DPC_CANON_TvAvSetting           0xD00C
#define PTP_DPC_CANON_MeteringMode          0xD010
#define PTP_DPC_CANON_MacroMode             0xD011
#define PTP_DPC_CANON_FocusingPoint         0xD012
#define PTP_DPC_CANON_WhiteBalance          0xD013
#define PTP_DPC_CANON_ISOSpeed              0xD01C
#define PTP_DPC_CANON_Aperture              0xD01D
#define PTP_DPC_CANON_ShutterSpeed          0xD01E
#define PTP_DPC_CANON_ExpCompensation       0xD01F
#define PTP_DPC_CANON_D029                  0xD029
#define PTP_DPC_CANON_Zoom                  0xD02A
#define PTP_DPC_CANON_SizeQualityMode       0xD02C
#define PTP_DPC_CANON_FlashMemory           0xD031
#define PTP_DPC_CANON_CameraModel           0xD032
#define PTP_DPC_CANON_CameraOwner           0xD033
#define PTP_DPC_CANON_UnixTime              0xD034
#define PTP_DPC_CANON_RealImageWidth        0xD039
#define PTP_DPC_CANON_PhotoEffect           0xD040
#define PTP_DPC_CANON_AssistLight           0xD041
#define PTP_DPC_CANON_D045                  0xD045

/* Nikon extension device property codes */
#define PTP_DPC_NIKON_ShootingBank                  0xD010
#define PTP_DPC_NIKON_ShootingBankNameA             0xD011
#define PTP_DPC_NIKON_ShootingBankNameB             0xD012
#define PTP_DPC_NIKON_ShootingBankNameC             0xD013
#define PTP_DPC_NIKON_ShootingBankNameD             0xD014
#define PTP_DPC_NIKON_RawCompression                0xD016
#define PTP_DPC_NIKON_WhiteBalanceAutoBias          0xD017
#define PTP_DPC_NIKON_WhiteBalanceTungstenBias      0xD018
#define PTP_DPC_NIKON_WhiteBalanceFlourescentBias   0xD019
#define PTP_DPC_NIKON_WhiteBalanceDaylightBias      0xD01a
#define PTP_DPC_NIKON_WhiteBalanceFlashBias         0xD01b
#define PTP_DPC_NIKON_WhiteBalanceCloudyBias        0xD01c
#define PTP_DPC_NIKON_WhiteBalanceShadeBias         0xD01d
#define PTP_DPC_NIKON_WhiteBalanceColourTemperature 0xD01e
#define PTP_DPC_NIKON_ImageSharpening               0xD02a
#define PTP_DPC_NIKON_ToneCompensation              0xD02b
#define PTP_DPC_NIKON_ColourMode                    0xD02c
#define PTP_DPC_NIKON_HueAdjustment                 0xD02d
#define PTP_DPC_NIKON_NonCPULensDataFocalLength     0xD02e
#define PTP_DPC_NIKON_NonCPULensDataMaximumAperture 0xD02f
#define PTP_DPC_NIKON_CSMMenuBankSelect             0xD040
#define PTP_DPC_NIKON_MenuBankNameA                 0xD041
#define PTP_DPC_NIKON_MenuBankNameB                 0xD042
#define PTP_DPC_NIKON_MenuBankNameC                 0xD043
#define PTP_DPC_NIKON_MenuBankNameD                 0xD044
#define PTP_DPC_NIKON_A1AFCModePriority             0xD048
#define PTP_DPC_NIKON_A2AFSModePriority             0xD049
#define PTP_DPC_NIKON_A3GroupDynamicAF              0xD04a
#define PTP_DPC_NIKON_A4AFActivation                0xD04b
#define PTP_DPC_NIKON_A5FocusAreaIllumManualFocus   0xD04c
#define PTP_DPC_NIKON_FocusAreaIllumContinuous      0xD04d
#define PTP_DPC_NIKON_FocusAreaIllumWhenSelected    0xD04e
#define PTP_DPC_NIKON_A6FocusArea                   0xD04f
#define PTP_DPC_NIKON_A7VerticalAFON                0xD050
#define PTP_DPC_NIKON_B1ISOAuto                     0xD054
#define PTP_DPC_NIKON_B2ISOStep                     0xD055
#define PTP_DPC_NIKON_B3EVStep                      0xD056
#define PTP_DPC_NIKON_B4ExposureCompEv              0xD057
#define PTP_DPC_NIKON_B5ExposureComp                0xD058
#define PTP_DPC_NIKON_B6CenterWeightArea            0xD059
#define PTP_DPC_NIKON_C1AELock                      0xD05e
#define PTP_DPC_NIKON_C2AELAFL                      0xD05f
#define PTP_DPC_NIKON_C3AutoMeterOff                0xD062
#define PTP_DPC_NIKON_C4SelfTimer                   0xD063
#define PTP_DPC_NIKON_C5MonitorOff                  0xD064
#define PTP_DPC_NIKON_D1ShootingSpeed               0xD068
#define PTP_DPC_NIKON_D2MaximumShots                0xD069
#define PTP_DPC_NIKON_D3ExpDelayMode                0xD06a
#define PTP_DPC_NIKON_D4LongExposureNoiseReduction  0xD06b
#define PTP_DPC_NIKON_D5FileNumberSequence          0xD06c
#define PTP_DPC_NIKON_D6ControlPanelFinderRearControl 0xD06d
#define PTP_DPC_NIKON_ControlPanelFinderViewfinder  0xD06e
#define PTP_DPC_NIKON_D7Illumination                0xD06f
#define PTP_DPC_NIKON_E1FlashSyncSpeed              0xD074
#define PTP_DPC_NIKON_E2FlashShutterSpeed           0xD075
#define PTP_DPC_NIKON_E3AAFlashMode                 0xD076
#define PTP_DPC_NIKON_E4ModelingFlash               0xD077
#define PTP_DPC_NIKON_E5AutoBracketSet              0xD078
#define PTP_DPC_NIKON_E6ManualModeBracketing        0xD079    
#define PTP_DPC_NIKON_E7AutoBracketOrder            0xD07a
#define PTP_DPC_NIKON_E8AutoBracketSelection        0xD07b
#define PTP_DPC_NIKON_F1CenterButtonShootingMode    0xD080
#define PTP_DPC_NIKON_CenterButtonPlaybackMode      0xD081
#define PTP_DPC_NIKON_F2Multiselector               0xD082
#define PTP_DPC_NIKON_F3PhotoInfoPlayback           0xD083
#define PTP_DPC_NIKON_F4AssignFuncButton            0xD084
#define PTP_DPC_NIKON_F5CustomizeCommDials          0xD085
#define PTP_DPC_NIKON_ChangeMainSub                 0xD086
#define PTP_DPC_NIKON_ApertureSetting               0xD087
#define PTP_DPC_NIKON_MenusAndPlayback              0xD088
#define PTP_DPC_NIKON_F6ButtonsAndDials             0xD089
#define PTP_DPC_NIKON_F7NoCFCard                    0xD08a
#define PTP_DPC_NIKON_ImageCommentString            0xD090
#define PTP_DPC_NIKON_ImageCommentAttached          0xD091
#define PTP_DPC_NIKON_AutoImageRotation             0xD092
#define PTP_DPC_NIKON_ExposureBracketingOnOff       0xD0c0
#define PTP_DPC_NIKON_ExposureBracketingIntervalDist    0xD0c1
#define PTP_DPC_NIKON_ExposureBracketingNumBracketPlace 0xD0c2
#define PTP_DPC_NIKON_AutofocusLCDTopMode2          0xD107
#define PTP_DPC_NIKON_AutofocusLCDTopMode3AndMode4  0xD108
#define PTP_DPC_NIKON_LightMeter                    0xD10a
#define PTP_DPC_NIKON_ExposureApertureLock          0xD111
#define PTP_DPC_NIKON_MaximumShots                  0xD103
#define PTP_DPC_NIKON_Beep                          0xD160
#define PTP_DPC_NIKON_AFC                           0xD161
#define PTP_DPC_NIKON_AFLampOff                     0xD163
#define PTP_DPC_NIKON_PADVPMode                     0xD164
#define PTP_DPC_NIKON_ReviewOff                     0xD165
#define PTP_DPC_NIKON_GridDisplay                   0xD16c
#define PTP_DPC_NIKON_AFAreaIllumination            0xD166
#define PTP_DPC_NIKON_FlashMode                     0xD167
#define PTP_DPC_NIKON_FlashPower                    0xD16d
#define PTP_DPC_NIKON_FlashSignOff                  0xD169
#define PTP_DPC_NIKON_RemoteTimeout                 0xD16B
#define PTP_DPC_NIKON_LowLight                      0xD1B0
#define PTP_DPC_NIKON_ImageCommentString            0xD090
#define PTP_DPC_NIKON_FlashOpen                     0xD1C0
#define PTP_DPC_NIKON_FlashCharged                  0xD1C1
#define PTP_DPC_NIKON_LensID                        0xD0E0
#define PTP_DPC_NIKON_FocalLengthMin                0xD0E3
#define PTP_DPC_NIKON_FocalLengthMax                0xD0E4
#define PTP_DPC_NIKON_MaxApAtMinFocalLength         0xD0E5
#define PTP_DPC_NIKON_MaxApAtMaxFocalLength         0xD0E6
#define PTP_DPC_NIKON_FlashExposureCompensation     0xD126
#define PTP_DPC_NIKON_ExtendedCSMMenu               0xD180
#define PTP_DPC_NIKON_OptimiseImage                 0xD140

#endif  //_PTPROCODES_H_

/***************************************************************************/

