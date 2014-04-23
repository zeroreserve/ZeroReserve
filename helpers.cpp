/*
    This file is part of the Zero Reserve Plugin for Retroshare.

    Zero Reserve is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zero Reserve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Zero Reserve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "helpers.h"
#include <execinfo.h>
#include <stdlib.h>
#include <iostream>


void print_stacktrace()
{

    int j, nptrs;
#define SIZE 100
    void *buffer[ SIZE ];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        std::cerr << "Zero Reserve: " << __func__ << ": Cannot print symbols" << std::endl;
        return;
    }

    std::cerr << "Zero Reserve: Printing Stack Trace:" << std::endl;
    for (j = 1; j < nptrs; j++)
        std::cerr << "Zero Reserve: " << strings[j] << std::endl;

    free(strings);
}
