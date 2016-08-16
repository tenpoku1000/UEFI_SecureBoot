
// @@@

// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

/**

Copyright (c) Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "UDK.h"

EFI_GUID gEfiDebugAgentGuid = { 0x865a5a9b, 0xb85d, 0x474c, { 0x84, 0x55, 0x65, 0xd1, 0xbe, 0x84, 0x4b, 0xe2 } };
EFI_GUID gEfiCertPkcs7Guid = { 0x4aafd29d, 0x68df, 0x49ee,{ 0x8a, 0xa9, 0x34, 0x7d, 0x37, 0x56, 0x65, 0xa7 } };
EFI_GUID gEfiCertRsa2048Guid = { 0x3c5766e8, 0x269c, 0x4e34,{ 0xaa, 0x14, 0xed, 0x77, 0x6e, 0x85, 0xb3, 0xb6 } };
EFI_GUID gEfiCertX509Guid = { 0xa5c059a1, 0x94e4, 0x4aa7,{ 0x87, 0xb5, 0xab, 0x15, 0x5c, 0x2b, 0xf0, 0x72 } };
EFI_GUID gEfiFirmwareFileSystem2Guid = { 0x8c8ce578, 0x8a3d, 0x4f1c, { 0x99, 0x35, 0x89, 0x61, 0x85, 0xc3, 0x2d, 0xd3 } };
EFI_GUID gEfiGlobalVariableGuid = { 0x8BE4DF61, 0x93CA, 0x11D2,{ 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C } };
EFI_GUID gEfiImageSecurityDatabaseGuid = { 0xd719b2cb, 0x3d3a, 0x4596,{ 0xa3, 0xbc, 0xda, 0xd0,  0xe, 0x67, 0x65, 0x6f } };
EFI_GUID gEfiPeiFirmwareVolumeInfoPpiGuid = { 0x49edb1c1, 0xbf21, 0x4761, { 0xbb, 0x12, 0xeb, 0x0, 0x31, 0xaa, 0xbb, 0x39 } };
EFI_GUID gEfiPeiFirmwareVolumeInfo2PpiGuid = { 0xea7ca24b, 0xded5, 0x4dad, { 0xa3, 0x89, 0xbf, 0x82, 0x7e, 0x8f, 0x9b, 0x38 } };
EFI_GUID gEfiPeiMemoryDiscoveredPpiGuid = { 0xf894643d, 0xc449, 0x42d1, { 0x8e, 0xa8, 0x85, 0xbd, 0xd8, 0xc6, 0x5b, 0xde } };
EFI_GUID gEfiVectorHandoffInfoPpiGuid = { 0x3cd652b4, 0x6d33, 0x4dce, { 0x89, 0xdb, 0x83, 0xdf, 0x97, 0x66, 0xfc, 0xca } };

/* MdePkg\Library\BaseLib\LinkedList.c */

#define _PCD_VALUE_PcdMaximumLinkedListLength  1000000U
const UINT32 _gPcd_FixedAtBuild_PcdMaximumLinkedListLength = _PCD_VALUE_PcdMaximumLinkedListLength;

#define _PCD_VALUE_PcdVerifyNodeInList  ((BOOLEAN)0U)
const BOOLEAN _gPcd_FixedAtBuild_PcdVerifyNodeInList = _PCD_VALUE_PcdVerifyNodeInList;

/* MdePkg\Library\BaseLib\String.c */

#define _PCD_VALUE_PcdMaximumAsciiStringLength  1000000U
const UINT32 _gPcd_FixedAtBuild_PcdMaximumAsciiStringLength = _PCD_VALUE_PcdMaximumAsciiStringLength;

#define _PCD_VALUE_PcdMaximumUnicodeStringLength  1000000U
const UINT32 _gPcd_FixedAtBuild_PcdMaximumUnicodeStringLength = _PCD_VALUE_PcdMaximumUnicodeStringLength;

/* MdePkg\Library\BaseSynchronizationLib\SynchronizationMsc.c */

#define _PCD_VALUE_PcdSpinLockTimeout  10000000U
const UINT32 _gPcd_FixedAtBuild_PcdSpinLockTimeout = _PCD_VALUE_PcdSpinLockTimeout;

/* UefiCpuPkg\Library\SecPeiDxeTimerLibUefiCpu\X86TimerLib.c */

#define _PCD_VALUE_PcdFSBClock  200000000U
const UINT32 _gPcd_FixedAtBuild_PcdFSBClock = _PCD_VALUE_PcdFSBClock;

/* UefiCpuPkg\Library\BaseXApicLib\BaseXApicLib.c */

#define _PCD_VALUE_PcdCpuInitIpiDelayInMicroSeconds  10000U
GLOBAL_REMOVE_IF_UNREFERENCED const UINT32 _gPcd_FixedAtBuild_PcdCpuInitIpiDelayInMicroSeconds = _PCD_VALUE_PcdCpuInitIpiDelayInMicroSeconds;
