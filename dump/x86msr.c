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

#include "x86msr.h"
#if defined(__gnu_linux__)
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static void dump_msr(int fd, const char *name, unsigned index)
{
	unsigned long long v;

	if (fd != -1 && pread(fd, &v, sizeof(v), index) == sizeof(v))
		printf("  %-40s  %16llx\n", name, v);
	else
		printf("  %-40s  %16s\n", name, "n/a");
}
#endif

void dump_x86_msr(void)
{
#if defined(__gnu_linux__)
	int fd = open("/dev/cpu/0/msr", O_RDONLY);

	puts("");
	puts("MSR dump:");

	dump_msr(fd, "IA32_EFER", 0xc0000080);
	dump_msr(fd, "IA32_VMX_PINBASED_CTLS", 0x481);
	dump_msr(fd, "IA32_VMX_PROCBASED_CTLS", 0x482);
	dump_msr(fd, "IA32_VMX_PROCBASED_CTLS2", 0x48b);

	if (fd != -1)
		close(fd);
#endif
}
