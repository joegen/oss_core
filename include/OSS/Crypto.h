// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
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


#ifndef OSS_CRYPTO_H_INCLUDED
#define OSS_CRYPTO_H_INCLUDED

#include <string>
#include <vector>

#include "OSS/OSS.h"

namespace OSS {

void OSS_API crypto_md5_encode(std::stringstream& in, std::ostringstream& out);
  /// Encode a stream to MD5

void OSS_API crypto_base64_encode(const std::string& in, std::string& out);
  /// Encode a string to base 64

void OSS_API crypto_base64_encode(std::vector<char>& in, std::string& out);
  /// Encode a byte array to base 64 string

bool OSS_API crypto_base64_decode(const std::string& in, std::vector<char>& out);
  /// Decode a string from base 64


} // OSS

#endif


