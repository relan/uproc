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

%{
#include <stdio.h>

int yylex(void);
int yyerror(const char *s);
%}

%union {
	const char *s;
	unsigned long long x;
}

%token <s> STRING
%token <s> ID
%token <x> HEX

%token PROFILE_START
%token PROFILE_END

%token DATE
%token COMPUTER
%token OS
%token KERNEL

%token EOL

%token CPUID
%token CPUID_HEADING
%token MSR
%token NA

%start profile

%%

profile : PROFILE_START EOL body PROFILE_END EOL
body : heading EOL blocks
heading : date computer os kernel
date : DATE STRING EOL
computer : COMPUTER STRING EOL
os : OS STRING EOL
kernel : KERNEL STRING EOL
blocks : block | block EOL blocks
block : cpuid | msr

cpuid : cpuid_start EOL CPUID_HEADING EOL cpuid_body
	{ puts(")"); }
cpuid_start : CPUID
	{ puts("("); }
cpuid_body : cpuid_leaf | cpuid_leaf cpuid_body
cpuid_leaf : cpuid_leaf4 | cpuid_leaf5
cpuid_leaf4 : HEX HEX HEX HEX HEX EOL
	{ printf("  (#x%llx #(#x%llx #x%llx #x%llx #x%llx))\n", $1, $2, $3, $4, $5); }
cpuid_leaf5 : HEX HEX HEX HEX HEX HEX EOL

msr : msr_start EOL msr_body
	{ puts(")"); }
msr_start : MSR
	{ puts("("); }
msr_body : msr_line | msr_line msr_body
msr_line : msr_available | msr_absent
msr_available : ID HEX EOL
	{ printf("  (\"%s\" #x%llx)\n", $1, $2); }
msr_absent : ID NA EOL

%%

int main()
{
	return yyparse();
}
