/**
 * @file tftpfileserver.cpp
 *
 */
/* Copyright (C) 2019-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "tftpfileserver.h"
#include "ubootheader.h"
#include "remoteconfig.h"

#include "display.h"

// Temporarily class. Only needed for the migration to compressed firmware
#include "compressed.h"

#include "debug.h"

#if defined(ORANGE_PI)
 constexpr char sFileName[] = "orangepi_zero.uImage";
 #define FILE_NAME_LENGTH	(sizeof(sFileName) - 1)
#else
 constexpr char sFileName[] = "orangepi_one.uImage";
 #define FILE_NAME_LENGTH	(sizeof(sFileName) - 1)
#endif

TFTPFileServer::TFTPFileServer(uint8_t *pBuffer, uint32_t nSize):
		m_pBuffer(pBuffer),
		m_nSize(nSize),
		m_nFileSize(0),
		m_bDone(false)
{
	DEBUG_ENTRY

	assert(m_pBuffer != 0);

	m_bIsCompressedSupported = Compressed::IsSupported();
	DEBUG_PRINTF("m_bIsCompressedSupported=%d", static_cast<int>(m_bIsCompressedSupported));

	DEBUG_EXIT
}

TFTPFileServer::~TFTPFileServer(void) {
	DEBUG_ENTRY

	DEBUG_EXIT
}

void TFTPFileServer::Exit(void) {
	DEBUG_ENTRY

	RemoteConfig::Get()->TftpExit();

	DEBUG_EXIT
}


bool TFTPFileServer::FileOpen(const char* pFileName, TTFTPMode tMode) {
	DEBUG_ENTRY

	DEBUG_EXIT
	return (false);
}

bool TFTPFileServer::FileCreate(const char* pFileName, TTFTPMode tMode) {
	DEBUG_ENTRY

	assert(pFileName != 0);

	if (tMode != TFTP_MODE_BINARY) {
		DEBUG_EXIT
		return false;
	}

	if (strncmp(sFileName, pFileName, FILE_NAME_LENGTH) != 0) {
		DEBUG_EXIT
		return false;
	}

	printf("TFTP started ...\n");
	Display::Get()->TextStatus("TFTP Started", DISPLAY_7SEGMENT_MSG_INFO_TFTP_STARTED);

	m_nFileSize = 0;

	DEBUG_EXIT
	return (true);
}

bool TFTPFileServer::FileClose(void) {
	DEBUG_ENTRY

	m_bDone = true;
	Display::Get()->TextStatus("TFTP Ended", DISPLAY_7SEGMENT_MSG_INFO_TFTP_ENDED);

	DEBUG_EXIT
	return true;
}

int TFTPFileServer::FileRead(void* pBuffer, unsigned nCount, unsigned nBlockNumber) {
	DEBUG_ENTRY

	DEBUG_EXIT
	return -1;
}

int TFTPFileServer::FileWrite(const void *pBuffer, unsigned nCount, unsigned nBlockNumber) {
	DEBUG_PRINTF("pBuffer=%p, nCount=%d, nBlockNumber=%d (%d)", pBuffer, nCount, nBlockNumber, m_nSize / 512);

	if (nBlockNumber > (m_nSize / 512)) {
		m_nFileSize = 0;
		return -1;
	}

	assert(nBlockNumber != 0);

	if (nBlockNumber == 1) {
		UBootHeader uImage(reinterpret_cast<uint8_t *>(const_cast<void*>(pBuffer)));
		if (!uImage.IsValid()) {
			DEBUG_PUTS("uImage is not valid");
			return -1;
		}
		// Temporarily code BEGIN
		if (!m_bIsCompressedSupported && uImage.IsCompressed()) {
			printf("Compressed uImage is not supported -> upgrade UBoot SPI");
			return -1;
		}
		// Temporarily code END
	}

	uint32_t nOffset = (nBlockNumber - 1) * 512;

	assert((nOffset + nCount) <= m_nSize);

	memcpy(&m_pBuffer[nOffset], pBuffer, nCount);

	m_nFileSize += nCount; //FIXME BUG When in retry ?

	return nCount;
}
