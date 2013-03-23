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
#include "platform.h"
#include <stdio.h>

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
