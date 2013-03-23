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

#if defined(UP_I386) || defined(UP_X86_64)
#include "x86cpuid.h"
#include "x86msr.h"
#else
#error unsupported architecture
#endif

#if defined(__gnu_linux__)
#define _XOPEN_SOURCE 500 /* for snprintf() in stdio.h */
#include <sys/utsname.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#elif defined(_WIN32)
#include <windows.h>
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
#define snprintf _snprintf
#elif defined(__sun)
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

static void get_date(char *buffer, size_t size)
{
	const time_t now = time(NULL);
	struct tm *tm = localtime(&now);

#ifdef _WIN32
	TIME_ZONE_INFORMATION tzinfo;
	LONG tz;

	if (GetTimeZoneInformation(&tzinfo) == TIME_ZONE_ID_INVALID)
		return;
	if (tzinfo.Bias < 0)
		tz = -tzinfo.Bias / 60 * 100 + -tzinfo.Bias % 60;
	else
		tz = -(tzinfo.Bias / 60 * 100 + tzinfo.Bias % 60);

	snprintf(buffer, size, "%d-%02d-%02d %02d:%02d:%02d UTC%+05ld",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec,
		tz);
#else
	strftime(buffer, size, "%F %T UTC%z", tm);
#endif
}

static void get_computer_model(char *buffer, size_t size)
{
#if defined(__APPLE__)
	if (sysctlbyname("hw.model", buffer, &size, NULL, 0) != 0)
		strcpy(buffer, "UnknownMac");	/* sysctl failed */
#elif defined(__gnu_linux__) || defined(_WIN32) || defined(__sun)
	strncpy(buffer, "PC", size);
#else
#error unsupported platform
#endif
}

static void get_os(char *buffer, size_t size)
{
#if defined(__APPLE__)
	struct utsname name;

	if (uname(&name) != -1) {
		int major = 0, minor = 0;

		sscanf(name.release, "%d.%d.", &major, &minor);
		snprintf(buffer, size, "OS X 10.%d.%d", major - 4, minor);
	} else
		strncpy(buffer, "unknown OS X", size);
#elif defined(__gnu_linux__)
	FILE *release;
	const char **pp;
	const char *release_files[] = {
		"/etc/redhat-release",
		"/etc/fedora-release",
		"/etc/mandriva-release",
		"/etc/SuSE-release",
		NULL
	};

	for (pp = release_files; *pp; pp++) {
		char *eol;

		/* try to open file (it's a sort of guesture) */
		release = fopen(*pp, "r");
		if (release == NULL)
			continue;
		/* release string should be the first line in the file */
		if (fgets(buffer, size, release) == NULL)
			break;
		eol = strchr(buffer, '\n');
		if (eol)
			*eol = '\0';
		/* success */
		fclose(release);
		return;
	}

	strcpy(buffer, "unknown GNU/Linux");
#elif defined(_WIN32)
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
	strcpy(buffer, "unknown Windows");
#elif defined(__sun)
	struct utsname name;

	if (uname(&name) != -1)
		snprintf(buffer, size, "Solaris %s", name.version);
	else
		strncpy(buffer, "unknown Solaris", size);
#else
#error unsupported platform
#endif
}

static void get_kernel(char *buffer, size_t size)
{
#if defined(__APPLE__) || defined(__gnu_linux__)
	struct utsname name;

	if (uname(&name) != -1)
		snprintf(buffer, size, "%s %s", name.sysname, name.release);
	else
#ifdef __APPLE__
		strncpy(buffer, "unknown Darwin", size);
#else
		strncpy(buffer, "unknown Linux", size);
#endif
#elif defined(_WIN32)
	OSVERSIONINFO ver = {sizeof(OSVERSIONINFO)};
	if (GetVersionEx(&ver))
		snprintf(buffer, size, "NT %lu.%lu.%lu",
			 ver.dwMajorVersion, ver.dwMinorVersion,
			 ver.dwBuildNumber);
	else
		strncpy(buffer, "unknown NT", size);
#elif defined(__sun)
	/* FIXME */
	strncpy(buffer, "unknown UTS", size);
#else
#error unsupported platform
#endif
}

static void get_ram(char *buffer, size_t size)
{
#if defined(__APPLE__)
	unsigned long long ram = 0;
	size_t sz = sizeof(ram);

	if (sysctlbyname("hw.memsize", &ram, &sz, NULL, 0) != 0)
		return;		/* sysctl failed */
	ram /= 1024;		/* convert to kilobytes */
#elif defined(__gnu_linux__)
	unsigned long long ram = 0;
	FILE *meminfo = fopen("/proc/meminfo", "r");

	if (meminfo == NULL)
		return;		/* procfs not supported or not mounted? */
	if (fscanf(meminfo, "MemTotal: %llu kB", &ram) != 1) {
		fclose(meminfo);
		return;		/* no match */
	}
	fclose(meminfo);
	if (ram == 0)
		return;		/* bad format */
#elif defined(_WIN32)
	ULONG64 ram = 0;
	MEMORYSTATUSEX meminfo = {sizeof(MEMORYSTATUSEX)};
	/* Windows XP or later is required */
	LPGMSE lpGlobalMemoryStatusEx = (LPGMSE) GetProcAddress(
		GetModuleHandle("kernel32.dll"), "GlobalMemoryStatusEx");

	if (lpGlobalMemoryStatusEx == NULL || !lpGlobalMemoryStatusEx(&meminfo))
		return;
	/* convert bytes to kilobytes with rounding */
	ram = (meminfo.ullTotalPhys + 512) / 1024;
#elif defined(__sun)
	unsigned long long ram;
	long pages = sysconf(_SC_PHYS_PAGES);
	long page_size = sysconf(_SC_PAGESIZE);

	if (pages == -1 || page_size == -1)
		return;
	ram = (unsigned long long) pages * page_size / 1024;
#else
#error unsupported platform
#endif

	/* convert from kilobytes to megabytes with rounding */
	ram = (ram + 512) / 1024;
	snprintf(buffer, size, "(%llu MB)", ram);
}

int main(void)
{
	char date[50] = {'\0'};
	char computer[50] = {'\0'};
	char ram[50] = {'\0'};
	char os[50] = {'\0'};
	char kernel[50] = {'\0'};

	get_date(date, sizeof(date));
	get_computer_model(computer, sizeof(computer));
	get_ram(ram, sizeof(ram));
	get_os(os, sizeof(os));
	get_kernel(kernel, sizeof(kernel));

	puts("================== begin of profile dump ===================");
	printf("date:     %s\n", date);
	printf("computer: %s %s\n", computer, ram);
	printf("OS:       %s\n", os);
	printf("kernel:   %s\n", kernel);

#if defined(UP_I386) || defined(UP_X86_64)
	dump_x86_cpuid();
	dump_x86_msr();
#else
#error unsupported architecture
#endif

	puts("=================== end of profile dump ====================");

	return 0;
}
