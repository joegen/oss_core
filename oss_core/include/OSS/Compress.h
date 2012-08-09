// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
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


#ifndef OSS_COMPRESS_H_INCLUDED
#define OSS_COMPRESS_H_INCLUDED


#include <vector>

#include "OSS/OSS.h"


namespace OSS {


void OSS_API compress_zlib_inflate(const std::vector<char>& in, std::vector<char>& out);
  /// Inflate data using zlib compression algorithm

void OSS_API compress_zlib_deflate(const std::vector<char>& in, std::vector<char>& out);
  /// Deflate data using zlib compression algorithm

void OSS_API compress_bzip2_inflate(const std::vector<char>& in, std::vector<char>& out);
  /// Inflate data using bzip2 compression algorithm

void OSS_API compress_bzip_deflate(const std::vector<char>& in, std::vector<char>& out);
  /// Deflate data using bzip2 compression algorithm

} // OSS

#endif // OSS_COMPRESS_H_INCLUDED


