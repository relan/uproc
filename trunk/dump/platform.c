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
#include <time.h>
#include <string.h>

#ifndef _WIN32
void get_date(char *buffer, size_t size)
{
	const time_t now = time(NULL);
	struct tm *tm = localtime(&now);

	strftime(buffer, size, "%F %T UTC%z", tm);
}
#endif

#ifndef __APPLE__
void get_computer_model(char *buffer, size_t size)
{
	strncpy(buffer, "PC", size);
}
#endif
