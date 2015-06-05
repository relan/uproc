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

#include "platform.h"
#include <sys/utsname.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void get_ram(char *buffer, size_t size)
{
	long pages = sysconf(_SC_PHYS_PAGES);
	long page_size = sysconf(_SC_PAGESIZE);

	if (pages == -1 || page_size == -1)
		return;

	snprintf(buffer, size, "(%llu MB)",
		((unsigned long long) pages * page_size / 1024 + 512) / 1024);
}

void get_os(char *buffer, size_t size)
{
	struct utsname name;

	if (uname(&name) != -1)
		snprintf(buffer, size, "Solaris %s", name.version);
	else
		strncpy(buffer, "unknown Solaris", size);
}

void get_kernel(char *buffer, size_t size)
{
	/* FIXME */
	strncpy(buffer, "unknown UTS", size);
}
