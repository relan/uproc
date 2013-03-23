/*
 * Copyright (C) 2013  Andrew Nayenko
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if WINVER < 0x501
typedef struct {
	DWORD     dwLength;
	DWORD     dwMemoryLoad;
	DWORDLONG ullTotalPhys;
	DWORDLONG ullAvailPhys;
	DWORDLONG ullTotalPageFile;
	DWORDLONG ullAvailPageFile;
	DWORDLONG ullTotalVirtual;
	DWORDLONG ullAvailVirtual;
	DWORDLONG ullAvailExtendedVirtual;
} MEMORYSTATUSEX;
#endif
typedef BOOL (WINAPI *LPGMSE)(MEMORYSTATUSEX *lpBuffer);

void get_date(char *buffer, size_t size)
{
	TIME_ZONE_INFORMATION tzinfo;
	LONG tz;
	const time_t now = time(NULL);
	struct tm *tm = localtime(&now);

	if (GetTimeZoneInformation(&tzinfo) == TIME_ZONE_ID_INVALID)
		return;
	if (tzinfo.Bias < 0)
		tz = -tzinfo.Bias / 60 * 100 + -tzinfo.Bias % 60;
	else
		tz = -(tzinfo.Bias / 60 * 100 + tzinfo.Bias % 60);

	_snprintf(buffer, size, "%d-%02d-%02d %02d:%02d:%02d UTC%+05ld",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec,
		tz);
	buffer[size - 1] = '\0';
}

void get_ram(char *buffer, size_t size)
{
	MEMORYSTATUSEX meminfo = {sizeof(MEMORYSTATUSEX)};

	/* Windows XP or later is required */
	LPGMSE lpGlobalMemoryStatusEx = (LPGMSE) GetProcAddress(
		GetModuleHandle("kernel32.dll"), "GlobalMemoryStatusEx");

	if (lpGlobalMemoryStatusEx == NULL ||
			!lpGlobalMemoryStatusEx(&meminfo))
		return;

	_snprintf(buffer, size, "(%llu MB)",
		(meminfo.ullTotalPhys / 1024 + 512) / 1024);
	buffer[size - 1] = '\0';
}

void get_os(char *buffer, size_t size)
{
	HKEY hCurrentVersion = 0;

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		  "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
		  KEY_READ, &hCurrentVersion) == ERROR_SUCCESS
	    && hCurrentVersion) {
		DWORD dwKeyType = REG_NONE;
		DWORD dwBufferLength = (DWORD) size;

		if (RegQueryValueExA(hCurrentVersion, "ProductName", NULL,
			&dwKeyType, buffer, &dwBufferLength) == ERROR_SUCCESS
		    && dwKeyType == REG_SZ) {
			char *p;

			RegCloseKey(hCurrentVersion);

			/* remove TM mark to make the string more readable */
			if ((p = strstr(buffer, " (TM)")))
				strcpy(p, p + 5);
			return;
		}
		RegCloseKey(hCurrentVersion);
	}
	strncpy(buffer, "unknown Windows", size);
	buffer[size - 1] = '\0';
}

void get_kernel(char *buffer, size_t size)
{
	OSVERSIONINFO ver = {sizeof(OSVERSIONINFO)};

	if (GetVersionEx(&ver))
		_snprintf(buffer, size, "NT %lu.%lu.%lu",
			 ver.dwMajorVersion, ver.dwMinorVersion,
			 ver.dwBuildNumber);
	else
		strncpy(buffer, "unknown NT", size);
	buffer[size - 1] = '\0';
}
