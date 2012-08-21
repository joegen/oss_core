// OSS Software Solutions Application Programmer Interface
//
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: Core
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "OSS/StackTrace.h"

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>



namespace OSS {

void StackTrace::print()
{
  print(std::cerr);
}



void StackTrace::print(std::ostream& strm)
{
  void *array[256];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 256);
  strings = backtrace_symbols (array, size);

  printf ("Obtained %zd stack frames.\n", size);

  
  for (i = 1; i < size; i++)
    strm << strings[i] << std::endl;

  free (strings);
}

void StackTrace::log()
{
  std::ostringstream strm;
  print(strm);
  OSS_LOG_CRITICAL(strm.str());
}


} // OSS





