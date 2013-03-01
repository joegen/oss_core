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

#include <fstream>
#include "OSS/Crypto/FileEncryptor.h"


namespace OSS {
namespace Crypto {


#if OSS_ARCH_LITTLE_ENDIAN
struct BF_UINT64
{
	unsigned int zero:8;
	unsigned int one:8;
	unsigned int two:8;
	unsigned int three:8;
  unsigned int four:8;
	unsigned int five:8;
	unsigned int six:8;
	unsigned int seven:8;
};
#else
struct BF_UINT64
{
  unsigned int seven:8;
  unsigned int six:8;
  unsigned int five:8;
  unsigned int four:8;
	unsigned int three:8;
	unsigned int two:8;
	unsigned int one:8;
	unsigned int zero:8;
};
#endif

union ENDIAN_SAFE_INT64
{
  OSS::UInt64 first;
  BF_UINT64 second;
};

FileEncryptor::FileEncryptor(BlockCipher::Algorithm algorithm, BlockCipher::Mode mode) :
  _cipher(algorithm, mode)
{
  
}

FileEncryptor::~FileEncryptor()
{

}

void FileEncryptor::encrypt(const boost::filesystem::path& src, BlockCipher::Buffer& target)
{
  std::ifstream input(OSS::boost_path(src).c_str());
  if (!input.is_open())
  {
    std::ostringstream errorMsg;
    errorMsg << "Fatal Exception: Unable to read from " << src;
    throw BlockCipherException(errorMsg.str());
  }

  BlockCipher::Buffer inputBuff;

  char c;
  while (input.get(c))
    inputBuff.push_back(c);

  target.reserve(inputBuff.size() + 8);

  //
  // Record the size of the original file in the first 8 bytes
  //
  ENDIAN_SAFE_INT64 inputLength;
  inputLength.first = inputBuff.size();
  const char* sLen = (const char*)&inputLength.second;
  for (int i = 0; i < 8; i++)
  {
    target.push_back(*sLen);
    sLen++;
  }

  if (inputLength.first < 8)
  {
    _cipher.process(_password, _ivKey, inputBuff, BlockCipher::Encryptor);
    for (BlockCipher::Buffer::iterator iter = inputBuff.begin(); iter != inputBuff.end(); iter++)
      target.push_back(*iter);
  }
  else
  {
    char* pInputBuff = &inputBuff[0];

    for (size_t idx = 0; idx < inputLength.first; idx+=8)
    {
      BlockCipher::Buffer block;
      //
      // Encrypt 8 byte blocks
      //
      if (idx + 8 > inputLength.first)
      {
        //
        // This is the last iteration and it's lacking some bytes
        //
        block = BlockCipher::Buffer(pInputBuff, pInputBuff + (inputLength.first - idx));
      }
      else
      {
        block = BlockCipher::Buffer(pInputBuff, pInputBuff + 8);
      }
      _cipher.process(_password, _ivKey, block, BlockCipher::Encryptor);

      for (BlockCipher::Buffer::iterator iter = block.begin(); iter != block.end(); iter++)
        target.push_back(*iter);

      pInputBuff += 8;
    }
  }
}

void FileEncryptor::encrypt(BlockCipher::Buffer& inputBuff, const boost::filesystem::path& out)
{
  BlockCipher::Buffer target;
  target.reserve(inputBuff.size() + 8);

  //
  // Record the size of the original file in the first 8 bytes
  //
  ENDIAN_SAFE_INT64 inputLength;
  inputLength.first = inputBuff.size();
  const char* sLen = (const char*)&inputLength.second;
  for (int i = 0; i < 8; i++)
  {
    target.push_back(*sLen);
    sLen++;
  }

  if (inputLength.first < 8)
  {
    _cipher.process(_password, _ivKey, inputBuff, BlockCipher::Encryptor);
    for (BlockCipher::Buffer::iterator iter = inputBuff.begin(); iter != inputBuff.end(); iter++)
      target.push_back(*iter);
  }
  else
  {
    char* pInputBuff = &inputBuff[0];

    for (size_t idx = 0; idx < inputLength.first; idx+=8)
    {
      BlockCipher::Buffer block;
      //
      // Encrypt 8 byte blocks
      //
      if (idx + 8 > inputLength.first)
      {
        //
        // This is the last iteration and it's lacking some bytes
        //
        block = BlockCipher::Buffer(pInputBuff, pInputBuff + (inputLength.first - idx));
      }
      else
      {
        block = BlockCipher::Buffer(pInputBuff, pInputBuff + 8);
      }
      _cipher.process(_password, _ivKey, block, BlockCipher::Encryptor);

      for (BlockCipher::Buffer::iterator iter = block.begin(); iter != block.end(); iter++)
        target.push_back(*iter);

      pInputBuff += 8;
    }
  }

  if (boost::filesystem::exists(out) && !boost::filesystem::remove(out))
  {
    std::ostringstream errorMsg;
    errorMsg << "Unable to write to " << out;
    throw BlockCipherException(errorMsg.str());
  }

  std::ofstream output(OSS::boost_path(out).c_str());
  if (!output.is_open())
  {
    std::ostringstream errorMsg;
    errorMsg << "Unable to write to " << out;
    throw BlockCipherException(errorMsg.str());
  }
  output.write(&target[0], target.size());
}

void FileEncryptor::encrypt(const boost::filesystem::path& src, const boost::filesystem::path& target)
{
  if (boost::filesystem::exists(target) && !boost::filesystem::remove(target))
  {
    std::ostringstream errorMsg;
    errorMsg << "Unable to write to " << target;
    throw BlockCipherException(errorMsg.str());
  }

  std::ofstream output(OSS::boost_path(target).c_str());
  if (!output.is_open())
  {
    std::ostringstream errorMsg;
    errorMsg << "Unable to write to " << target;
    throw BlockCipherException(errorMsg.str());
  }

  BlockCipher::Buffer buff;
  encrypt(src, buff);
  output.write(&buff[0], buff.size());
}

void FileEncryptor::decrypt(const boost::filesystem::path& src, BlockCipher::Buffer& outputBuffer)
{
  std::ifstream input(OSS::boost_path(src).c_str());
  if (!input.is_open())
  {
    std::ostringstream errorMsg;
    errorMsg << "Fatal Exception: Unable to read from " << src;
    throw BlockCipherException(errorMsg.str());
  }

  //
  // Record the size of the original file in the first 8 bytes
  //
  ENDIAN_SAFE_INT64 inputLength;
  inputLength.second.zero = input.get();
  inputLength.second.one = input.get();
  inputLength.second.two = input.get();
  inputLength.second.three = input.get();
  inputLength.second.four = input.get();
  inputLength.second.five = input.get();
  inputLength.second.six = input.get();
  inputLength.second.seven = input.get();

  BlockCipher::Buffer inputBuff;
  char c;
  while (input.get(c))
    inputBuff.push_back(c);

  if (inputBuff.size() % 8 != 0)
  {
    std::ostringstream errorMsg;
    errorMsg << "Invalid input block size (" << inputBuff.size() << ") - " << src;
    throw BlockCipherException(errorMsg.str());
  }

  char* pInputBuff = &inputBuff[0];

  outputBuffer.reserve(inputBuff.size());

  for (size_t idx = 0; idx < inputBuff.size(); idx+=8)
  {
    BlockCipher::Buffer block;
    //
    // Encrypt 8 byte blocks
    //
    block = BlockCipher::Buffer(pInputBuff, pInputBuff + 8);
    _cipher.process(_password, _ivKey, block, BlockCipher::Decryptor);
    //output.write(&block[0], block.size());
    for (BlockCipher::Buffer::iterator iter = block.begin(); iter != block.end(); iter++)
      outputBuffer.push_back(*iter);
    pInputBuff += 8;
  }

  if (outputBuffer.size() < inputLength.first)
  {
    std::ostringstream errorMsg;
    errorMsg << "Invalid input block size ("
      << outputBuffer.size() << ") while expecting " << inputLength.first;
    throw BlockCipherException(errorMsg.str());
  }

  outputBuffer.resize(inputLength.first);
}

void FileEncryptor::decrypt(const boost::filesystem::path& src, const boost::filesystem::path& target)
{
  if (boost::filesystem::exists(target) && !boost::filesystem::remove(target))
  {
    std::ostringstream errorMsg;
    errorMsg << "Unable to write to " << target;
    throw BlockCipherException(errorMsg.str());
  }

  std::ofstream output(OSS::boost_path(target).c_str());
  if (!output.is_open())
  {
    std::ostringstream errorMsg;
    errorMsg << "Unable to write to " << target;
    throw BlockCipherException(errorMsg.str());
  }

  BlockCipher::Buffer buff;
  decrypt(src, buff);
  output.write(&buff[0], buff.size());
}


} } // OSS::Crypto






