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


#ifndef OSS_CRYPTO_BlockCipher_H_INCLUDED
#define OSS_CRYPTO_BlockCipher_H_INCLUDED

#include <vector>
#include <string>

#include <boost/noncopyable.hpp>

#include "OSS/OSS.h"
#include "OSS/Crypto/OSSCrypto.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Exception.h"

#if OSS_HAVE_MCRYPT

#include <mcrypt.h>

namespace OSS {
namespace Crypto {


OSS_CREATE_INLINE_EXCEPTION(BlockCipherException, OSS::IOException, "Block Cipher Exception");

class BlockCipher : boost::noncopyable
{
public:
  typedef std::vector<char> Buffer;

  enum Algorithm
  {
    CRYPT_BLOWFISH,		//"blowfish"
    CRYPT_DES, 		//"des"
    CRYPT_3DES, 		//"tripledes"
    CRYPT_3WAY, 		//"threeway"
    CRYPT_GOST, 		//"gost"
    CRYPT_SAFER_SK64, 	//"safer-sk64"
    CRYPT_SAFER_SK128, 	//"safer-sk128"
    CRYPT_CAST_128, 	//"cast-128"
    CRYPT_XTEA, 		//"xtea"
    CRYPT_RC2,	 	//"rc2"
    CRYPT_TWOFISH, 		//"twofish"
    CRYPT_CAST_256, 	//"cast-256"
    CRYPT_SAFERPLUS, 	//"saferplus"
    CRYPT_LOKI97, 		//"loki97"
    CRYPT_SERPENT, 		//"serpent"
    CRYPT_RIJNDAEL_128, 	//"rijndael-128"
    CRYPT_RIJNDAEL_192, 	//"rijndael-192"
    CRYPT_RIJNDAEL_256, 	//"rijndael-256"
    CRYPT_ENIGMA, 		//"enigma"
    CRYPT_ARCFOUR,		//"arcfour"
    CRYPT_WAKE		//"wake"
  };

  enum Mode
  {
    MODE_CBC, //		"cbc"
    MODE_ECB, //		"ecb"
    MODE_CFB, //		"cfb"
    MODE_OFB, //		"ofb"
    MODE_NOFB, //		"nofb"
    MODE_STREAM //	"stream"
  };

  enum Operation
  {
    Encryptor,
    Decryptor
  };

  BlockCipher(Algorithm algorithm, Mode mode);

  virtual ~BlockCipher();

  void process(const std::string& key, const std::string& iv, Buffer& buff, Operation operation);
    /// Encrypt a block.  This will throw a BlockCipherException if an errrr occured

  int getBlockSize() const;
    /// Get the block size required by the algorithm

  int getKeySize() const;
    /// Returns the key size required by the algorithm

  int getIVKeySize() const;
    /// Returns the key size required by the mode

  const char* getName() const;
    /// Returns the name of the algorithm

  const char* getModeName() const;
   /// Returns the name of the mode
protected:
  Algorithm _algorithm;
  Mode _mode;
  std::string _key;
  std::string _ivKey;
  MCRYPT _cipher;
};

} } // OSS::Crypto

#endif // OSS_HAVE_MCRYPT

#endif // OSS_CRYPTO_BlockCipher_H_INCLUDED





