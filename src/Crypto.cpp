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


#include "OSS/Crypto.h"
#include "OSS/Exception.h"
#include <string.h>

#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"
#include <fstream>
#include <iostream>
#include <sstream>

using Poco::DigestEngine;
using Poco::MD5Engine;
using Poco::DigestOutputStream;
using Poco::StreamCopier;


namespace OSS {




void crypto_md5_encode(std::stringstream& in, std::ostringstream& out)
{
	MD5Engine md5;
	DigestOutputStream dos(md5);
	StreamCopier::copyStream(in, dos);
	dos.close();
	out << DigestEngine::digestToHex(md5.digest());
}

//
// Base 64 Encoding
//
static const unsigned char Binary2Base64[65] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char Base642Binary[256] =
{
  96, 99, 99, 99, 99, 99, 99, 99, 99, 99, 98, 99, 99, 98, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 62, 99, 99, 99, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 99, 99, 99, 97, 99, 99,
  99,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 99, 99, 99, 99, 99,
  99, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
  99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
};

void crypto_base64_encode(const std::string& in, std::string& out)
{
  std::vector<char> inVec(in.begin(), in.end());
  crypto_base64_encode(inVec, out);
}

void crypto_base64_encode(std::vector<char>& in, std::string& out)
{
  out.clear();
  out.reserve(in.size() + in.size() / 4);
  bool useCRLFs = false;
  size_t encodeLength = 0;
  size_t nextLine = 0;
  size_t saveCount = 0;
  char saveTriple[3];
  size_t length = in.size();

  const char* data = &in[0];
  while (saveCount < 3)
  {
    saveTriple[saveCount++] = *data++;
    if (--length == 0)
      return;
  }

  out.push_back(Binary2Base64[saveTriple[0] >> 2]);
  out.push_back(Binary2Base64[((saveTriple[0]&3)<<4) | (saveTriple[1]>>4)]);
  out.push_back(Binary2Base64[((saveTriple[1]&15)<<2) | (saveTriple[2]>>6)]);
  out.push_back(Binary2Base64[saveTriple[2]&0x3f]);
  encodeLength += 4;

  if (++nextLine > 18)
  { // 76 columns
    if (useCRLFs)
    {
      out.push_back('\r');
      encodeLength++;
    }
    out.push_back('\n');
    encodeLength++;
    nextLine = 0;
  }

  size_t i = 0;
  for (i = 0; i+2 < length; i += 3)
  {
    const char * currentFrame = data + i;
    out.push_back(Binary2Base64[currentFrame[0] >> 2]);
    out.push_back(Binary2Base64[((currentFrame[0]&3)<<4) | (currentFrame[1]>>4)]);
    out.push_back(Binary2Base64[((currentFrame[1]&15)<<2) | (currentFrame[2]>>6)]);
    out.push_back(Binary2Base64[currentFrame[2]&0x3f]);
    encodeLength += 4;

    if (++nextLine > 18)
    { // 76 columns
      if (useCRLFs)
      {
        out.push_back('\r');
        encodeLength++;
      }
      out.push_back('\n');
      encodeLength++;
      nextLine = 0;
    }
  }

  saveCount = length - i;
  switch (saveCount)
  {
    case 1 :
      saveTriple[0] = data[i];
      out.push_back(Binary2Base64[saveTriple[0] >> 2]);
      out.push_back(Binary2Base64[(saveTriple[0]&3)<<4]);
      out.push_back('=');
      out.push_back('=');
      encodeLength += 4;
      break;
    case 2 :
      saveTriple[0] = data[i++];
      saveTriple[1] = data[i];
      out.push_back(Binary2Base64[saveTriple[0] >> 2]);
      out.push_back(Binary2Base64[((saveTriple[0]&3)<<4) | (saveTriple[1]>>4)]);
      out.push_back(Binary2Base64[((saveTriple[1]&15)<<2)]);
      out.push_back('=');
      encodeLength += 4;
      break;
  }

  OSS_VERIFY(encodeLength == out.size());
}

bool crypto_base64_decode(const std::string& in, std::vector<char>& out)
{
  bool perfectDecode = true;
  size_t quadPosition = 0;
  out.clear();
  out.reserve(in.size() - in.size()/4);
  size_t decodeSize = 0;

  const char* cstr = in.c_str();
  for (;;)
  {
    unsigned char value = Base642Binary[(unsigned char)(*cstr)];
    cstr++;
    switch (value)
    {
      case 96 : // end of string
        return false;

      case 97 : // '=' sign
        if (quadPosition == 3 || (quadPosition == 2 && *cstr == '='))
        {
          quadPosition = 0;  // Reset this to zero, as have a perfect decode
          return true; // Stop decoding now as must be at end of data
        }
        perfectDecode = false;  // Ignore '=' sign but flag decode as suspect
        break;

      case 98 : // CRLFs
        break;  // Ignore totally

      case 99 :  // Illegal characters
        perfectDecode = false;  // Ignore rubbish but flag decode as suspect
        break;

      default : // legal value from 0 to 63
        switch (quadPosition) {
          case 0 :
            out.push_back(0);
            out[decodeSize] = (unsigned char)(value << 2);
            break;
          case 1 :
            out.push_back(0);
            out.push_back(0);
            out[decodeSize++] |= (unsigned char)(value >> 4);
            out[decodeSize] = (unsigned char)((value&15) << 4);
            break;
          case 2 :
            out.push_back(0);
            out.push_back(0);
            out[decodeSize++] |= (unsigned char)(value >> 2);
            out[decodeSize] = (unsigned char)((value&3) << 6);
            break;
          case 3 :
            out.push_back(0);
            out[decodeSize++] |= (unsigned char)value;
            break;
        }
        quadPosition = (quadPosition+1)&3;
    }
  }
  return perfectDecode;
}

} // OSS



