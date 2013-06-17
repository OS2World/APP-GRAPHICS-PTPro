/***************************************************************************/
//  ptproos2.c
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

#define INCL_NOPMAPI
#define INCL_DOSFILEMGR
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>
#include <time.h>
#include <stdio.h>
#include "ptprousb.h"

/***************************************************************************/

#ifndef CAMCNR_DATASIZE
#define CAMCNR_DATASIZE         500
#endif

/***************************************************************************/

uint32_t    os2_get_file( CAMHandle hCam, CAMObjectInfoPtr pObjInfo,
                          uint32_t handle, char* filename, int replace,
                          int thumb)
{
    APIRET      rc = 0;
    uint32_t    rtn = 0;
    HFILE       hFile = 0;
    char *      pBuf = 0;
    char *      pMem = 0;
    char *      pMsg = 0;
    ULONG       ulSize;
    ULONG       ul;
    FILESTATUS3 fs3;
    struct tm * ptm;

do {
    if (!filename) {
        filename = pObjInfo->Filename;
        printf (" Saving %3d  (%s)... ", handle, filename);
    }
    else
        printf (" Saving %3d  (%s) as \"%s\"... ", handle, pObjInfo->Filename, filename);

    ulSize = (thumb ? pObjInfo->ThumbCompressedSize : pObjInfo->ObjectCompressedSize);
    if (ulSize == 0) {
        pMsg = "skipped - file size is zero\n";
        break;
    }

    rc = DosOpen( filename, &hFile, &ul, ulSize,
                  FILE_NORMAL, OPEN_ACTION_CREATE_IF_NEW | (replace ?
                  OPEN_ACTION_REPLACE_IF_EXISTS : OPEN_ACTION_FAIL_IF_EXISTS),
                  (OPEN_FLAGS_FAIL_ON_ERROR | OPEN_FLAGS_SEQUENTIAL |
                  OPEN_SHARE_DENYREADWRITE | OPEN_ACCESS_READWRITE), 0);
    if (rc) {
        if (rc == ERROR_OPEN_FAILED)
            pMsg = "skipped - file exists\n";
        break;
    }

    rc = DosAllocMem( (PVOID)&pMem, ulSize + 0x1000 - CAMCNR_DATASIZE,
                      PAG_COMMIT | OBJ_TILE | PAG_READ | PAG_WRITE);
    if (rc)
        break;

    // this trick eliminates 400 memcpy()s in UsbBulkRead() for a 2mb file;
    // after GetData() reads the first 500 bytes, the buffer it passes
    // to UsbBulkRead() will be page-aligned & can be used as-is
    pBuf = pMem + 0x1000 - CAMCNR_DATASIZE;

    if (thumb)
        rtn = GetThumb( hCam, handle, &pBuf);
    else
        rtn = GetObject( hCam, handle, &pBuf);
    if (rtn) {
        rc = (APIRET)rtn;
        break;
    }

    rc = DosWrite( hFile, pBuf, ulSize, &ul);
    if (rc)
        break;

    if (pObjInfo->CaptureDate == 0) {
        pMsg = "done\n";
        break;
    }

    pMsg = "done - unable to set timestamp\n";

    rc = DosQueryFileInfo( hFile, FIL_STANDARD, &fs3, sizeof(FILESTATUS3));
    if (rc)
        break;

    ptm = localtime( &pObjInfo->CaptureDate);
    if (!ptm)
        break;

    fs3.fdateCreation.year = ptm->tm_year - 80;
    fs3.fdateCreation.month = ptm->tm_mon + 1;
    fs3.fdateCreation.day = ptm->tm_mday;
    fs3.ftimeCreation.hours = ptm->tm_hour;
    fs3.ftimeCreation.minutes = ptm->tm_min;
    fs3.ftimeCreation.twosecs = ptm->tm_sec / 2;
    fs3.fdateLastWrite = fs3.fdateCreation;
    fs3.ftimeLastWrite = fs3.ftimeCreation;

    rc = DosSetFileInfo( hFile, FIL_STANDARD, &fs3, sizeof(FILESTATUS3));
    if (rc)
        break;

    pMsg = "done\n";

} while (0);

    if (pMem)
        DosFreeMem( pMem);

    if (hFile)
        DosClose( hFile);

    if (pMsg)
        printf( pMsg);
    else
        printf( "error - rc= %d\n", (int)rc);

    return rtn;
}

/***************************************************************************/

