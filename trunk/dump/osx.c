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
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void get_computer_model(char *buffer, size_t size)
{
	if (sysctlbyname("hw.model", buffer, &size, NULL, 0) != 0)
		strcpy(buffer, "UnknownMac");
}

void get_ram(char *buffer, size_t size)
{
	unsigned long long ram = 0;
	size_t sz = sizeof(ram);

	if (sysctlbyname("hw.memsize", &ram, &sz, NULL, 0) != 0)
		return;

	snprintf(buffer, size, "(%llu MB)", (ram / 1024 + 512) / 1024);
}

void get_os(char *buffer, size_t size)
{
	struct utsname name;

	if (uname(&name) != -1) {
		int major = 0, minor = 0;

		sscanf(name.release, "%d.%d.", &major, &minor);
		snprintf(buffer, size, "OS X 10.%d.%d", major - 4, minor);
	} else
		strncpy(buffer, "unknown OS X", size);
}

void get_kernel(char *buffer, size_t size)
{
	struct utsname name;

	if (uname(&name) != -1)
		snprintf(buffer, size, "%s %s", name.sysname, name.release);
	else
		strncpy(buffer, "unknown Darwin", size);
}
