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


#ifndef OSS_CRYPTO_FileEncryptor_H_INCLUDED
#define OSS_CRYPTO_FileEncryptor_H_INCLUDED

#include "OSS/OSS.h"

#include "OSS/Crypto/BlockCipher.h"

#if OSS_HAVE_MCRYPT

namespace OSS {
namespace Crypto {

class FileEncryptor : boost::noncopyable
{
public:
  FileEncryptor(BlockCipher::Algorithm algorithm, BlockCipher::Mode mode);
    /// Create a new File Encryptor

  ~FileEncryptor();
    /// Destroy the File Encryptor

  void encrypt(const boost::filesystem::path& src, BlockCipher::Buffer& target);
    /// Encrypt a file and output the result to a buffer.
    /// Will throw a BlockCipherException if an error occurs

  void encrypt(BlockCipher::Buffer& src, const boost::filesystem::path& target);
    /// Encrypt a file and output the result to a buffer.
    /// Will throw a BlockCipherException if an error occurs

  void encrypt(const boost::filesystem::path& src, const boost::filesystem::path& target);
    /// Encrypt a file and output the result to a file
    /// Will throw a BlockCipherException if an error occurs

  void decrypt(const boost::filesystem::path& src, BlockCipher::Buffer& target);
    /// Decrypt a file and output the result to a buffer
    /// Will throw a BlockCipherException if an error occurs

  void decrypt(const boost::filesystem::path& src, const boost::filesystem::path& target);
    /// Decrypt a file and output the result to a file
    /// Will throw a BlockCipherException if an error occurs

  const std::string& getIVKey() const;
    /// Return the IV key

  void setIVKey(const std::string& ivKey);
    /// Set the IV Key

  const std::string& getPassword() const;
    /// Return the password

  void setPassword(const std::string& password);
    /// Set the password

private:
  BlockCipher _cipher;
  std::string _ivKey;
  std::string _password;
};

//
// Inlines
//

inline const std::string& FileEncryptor::getIVKey() const
{
  return _ivKey;
}

inline void FileEncryptor::setIVKey(const std::string& ivKey)
{
  _ivKey = ivKey;
}

inline const std::string& FileEncryptor::getPassword() const
{
  return _password;
}

inline void FileEncryptor::setPassword(const std::string& password)
{
  _password = password;
}

} } // OSS::Crypto

#endif // OSS_HAVE_MCRYPT

#endif



