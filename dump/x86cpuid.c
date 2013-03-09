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

#include "x86cpuid.h"
#include <stdio.h>

enum {eax, ebx, ecx, edx};

static void do_cpuid(unsigned leaf, unsigned subleaf, unsigned rv[])
{
#ifdef _WIN32
	unsigned out_eax, out_ebx, out_ecx, out_edx;

	__asm {
		mov eax, leaf
		mov ecx, subleaf
		cpuid
		mov out_eax, eax
		mov out_ebx, ebx
		mov out_ecx, ecx
		mov out_edx, edx
	}

	rv[eax] = out_eax;
	rv[ebx] = out_ebx;
	rv[ecx] = out_ecx;
	rv[edx] = out_edx;
#else
	__asm(
#ifdef UP_I386
		"xchgl %%ebx, %%esi\n"
#else
		"xchgq %%rbx, %%rsi\n"
#endif
		"cpuid\n"
		"movl %%ebx, %1\n"
#ifdef UP_I386
		"xchgl %%ebx, %%esi\n"
#else
		"xchgq %%rbx, %%rsi\n"
#endif
		: "=a" (rv[eax]), "=r" (rv[ebx]),
		  "=c" (rv[ecx]), "=d" (rv[edx])
		: "0" (leaf), "2" (subleaf)
		: "si");
#endif
}

static void dump_line(unsigned leaf, unsigned subleaf, unsigned rv[])
{
	printf("  %8x  %8x  %8x  %8x  %8x  %8x\n", leaf, subleaf,
			rv[eax], rv[ebx], rv[ecx], rv[edx]);
}

static void dump_page(unsigned input)
{
	unsigned rv[4], max;
	unsigned i, c;

	do_cpuid(input, 0, rv);
	max = rv[eax];
	if (max < input)
		return;

	for (i = input; i <= max; i++)
		switch (i) {
		case 4:
			for (c = 0; ; c++) {
				do_cpuid(i, c, rv);
				if (rv[eax] == 0)
					break;
				dump_line(i, c, rv);
			}
			break;
		case 0xb:
			for (c = 0; c < 2; c++) {
				do_cpuid(i, c, rv);
				if (c == 0 && (rv[ebx] & 0xffff) == 0)
					break;
				dump_line(i, c, rv);
			}
			break;
		case 0xd:
			for (c = 0; ; c++) {
				do_cpuid(i, c, rv);
				if (c >= 2 && rv[eax] == 0)
					break;
				dump_line(i, c, rv);
			}
			break;
		default:
			do_cpuid(i, 0, rv);
			printf("  %8x  %8s  %8x  %8x  %8x  %8x\n", i, "",
					rv[eax], rv[ebx], rv[ecx], rv[edx]);
			break;
		}
}

void dump_x86_cpuid(void)
{
	puts("");
	puts("CPUID dump:");
	printf("  %8s  %8s  %8s  %8s  %8s  %8s\n", "leaf", "subleaf",
			"eax", "ebx", "ecx", "edx");
	dump_page(0);
	dump_page(0x40000000);
	dump_page(0x80000000);
	dump_page(0xc0000000);
}