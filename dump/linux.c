/*
 * Copyright (C) 2013-2015  Andrew Nayenko
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

#define _XOPEN_SOURCE 500 /* for snprintf() in stdio.h */

#include "platform.h"
#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void get_ram(char *buffer, size_t size)
{
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

	snprintf(buffer, size, "(%llu MB)", (ram + 512) / 1024);
}

void get_os(char *buffer, size_t size)
{
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
}

void get_kernel(char *buffer, size_t size)
{
	struct utsname name;

	if (uname(&name) != -1)
		snprintf(buffer, size, "%s %s", name.sysname, name.release);
	else
		strncpy(buffer, "unknown Linux", size);
}
