
// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
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

#ifndef NO_BZIP2
#define NO_BZIP2 1
#endif

#include <iostream>
#include <sstream>

#include <boost/assign/list_of.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#ifndef NO_BZIP2
#include <boost/iostreams/filter/bzip2.hpp>
#endif
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include "OSS/Compress.h"



namespace OSS {


void compress_zlib_inflate(const std::vector<char>& in, std::vector<char>& out)
{
  out.clear();
  boost::iostreams::filtering_streambuf<boost::iostreams::output> outputBuff;
  outputBuff.push(boost::iostreams::zlib_compressor());
  outputBuff.push(boost::iostreams::back_inserter(out));
  boost::iostreams::copy(boost::make_iterator_range(in), outputBuff);
}
void compress_bzip2_inflate(const std::vector<char>& in, std::vector<char>& out)
{
  #ifndef NO_BZIP2
  out.clear();
  boost::iostreams::filtering_streambuf<boost::iostreams::output> outputBuff;
  outputBuff.push(boost::iostreams::bzip2_compressor());
  outputBuff.push(boost::iostreams::back_inserter(out));
  boost::iostreams::copy(boost::make_iterator_range(in), outputBuff);
  #endif
}

void compress_zlib_deflate(const std::vector<char>& in, std::vector<char>& out)
{
  out.clear();
  boost::iostreams::filtering_streambuf<boost::iostreams::input> outputBuff;
  outputBuff.push(boost::iostreams::zlib_decompressor());
  outputBuff.push(boost::make_iterator_range(in));
  boost::iostreams::copy(outputBuff, boost::iostreams::back_inserter(out));
}

void compress_bzip_deflate(const std::vector<char>& in, std::vector<char>& out)
{
  #ifndef NO_BZIP2
  out.clear();
  boost::iostreams::filtering_streambuf<boost::iostreams::input> outputBuff;
  outputBuff.push(boost::iostreams::bzip2_decompressor());
  outputBuff.push(boost::make_iterator_range(in));
  boost::iostreams::copy(outputBuff, boost::iostreams::back_inserter(out));
  #endif
}

} // OSS



