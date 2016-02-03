/*
 * Copyright (C) 2013-2016  Andrew Nayenko
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
#include <stdlib.h>
#include "parser.h"

int yyerror(const char *s);
%}

%option noyywrap
%option nounput
%option noinput

%s string

hex		[0-9a-f]+
id		[_A-Za-z][_A-Za-z0-9]*
space		[ \t]+

%%

"==================== profile dump start ===================="	return PROFILE_START;
"===================== profile dump end ====================="	return PROFILE_END;
date:{space}							BEGIN(string); return DATE;
computer:{space}						BEGIN(string); return COMPUTER;
OS:{space}							BEGIN(string); return OS;
kernel:{space}							BEGIN(string); return KERNEL;

"CPUID dump:"							return CPUID;
"      leaf   subleaf       eax       ebx       ecx       edx"	return CPUID_HEADING;
"MSR dump:"							return MSR;
"n/a"								return NA;

{hex}		yylval.x = strtoull(yytext, NULL, 16);		return HEX;
{id}		yylval.s = strdup(yytext);			return ID;
{space}		/* nothing */
\n		yylineno++;					return EOL;
<string>[^\n]+	yylval.s = strdup(yytext);			BEGIN(INITIAL); return STRING;
.		yyerror("parse error"); exit(1);

%%

int yyerror(const char *s)
{
	fprintf(stderr, "error at line %d: %s\n", yylineno, s);
	return 0;
}
