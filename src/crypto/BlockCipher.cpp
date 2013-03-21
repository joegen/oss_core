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


#include "OSS/Crypto/BlockCipher.h"


namespace OSS {
namespace Crypto {


  BlockCipher::BlockCipher(Algorithm algorithm, Mode mode) :
    _algorithm(algorithm),
    _mode(mode),
    _cipher(0)
  {
    char name[100];
    ::memset(name, 0, 100);
    ::strcpy(name, getName());
    
    char modeName[100];
    ::memset(modeName, 0, 100);
    ::strcpy(modeName, getModeName());
    
    _cipher = mcrypt_module_open(name, 0, modeName, 0);
    if (_cipher==MCRYPT_FAILED)
    {
      std::cout << "failed to initialize cipher" << std::endl;
    }
  }

  BlockCipher::~BlockCipher()
  {
    mcrypt_module_close(_cipher);
    _cipher = 0;
  }

  void BlockCipher::process(const std::string& key, const std::string& ivKey, Buffer& buff, Operation operation)
  {
    _ivKey = ivKey;
    _key = key;

    char* iv = 0;
    size_t ivKeySize = mcrypt_enc_get_iv_size(_cipher);
    if (_ivKey.size() != ivKeySize)
    {
      std::ostringstream errorMsg;
      errorMsg << "Invalid IV Key size.  Expecting " << ivKeySize << " bytes "
        << " but we got " << _ivKey.size() << " bytes.";
      throw BlockCipherException(errorMsg.str());
    }


    iv = (char*)malloc(ivKeySize);
    ::memcpy(iv, _ivKey.c_str(), ivKeySize);


    size_t keySize = mcrypt_enc_get_key_size(_cipher);
    if (_key.size() > keySize)
    {
      std::ostringstream errorMsg;
      errorMsg << "Invalid Key size.  Expecting lesser than " << keySize << " bytes "
        << " but we got " << _key.size() << " bytes.";
      throw BlockCipherException(errorMsg.str());
    }
    char* cipherKey = (char*)malloc(_key.size());
    ::mempcpy(cipherKey, _key.c_str(), _key.size());
    

    size_t blockSize = mcrypt_enc_get_block_size(_cipher);
    if (buff.size() > blockSize)
    {
      std::ostringstream errorMsg;
      errorMsg << "Invalid Block size.  Expecting <= " << blockSize << " bytes "
        << " but we got " << buff.size() << " bytes.";
      throw BlockCipherException(errorMsg.str());
    }
    else if (buff.size() < blockSize)
    {
      buff.resize(blockSize, 0);
    }

    char* cipherBuff = (char*)malloc(blockSize);
    ::memcpy(cipherBuff, &buff[0], blockSize);
    
    int ret = mcrypt_generic_init(_cipher, cipherKey, _key.size(), iv);
    if (ret < 0)
    {
      std::ostringstream errorMsg;
      errorMsg << "Cipher initialization error " << ret << " encountered while calling "
        << " BlockCipher::encrypt()";
      throw BlockCipherException(errorMsg.str());
    }

    if (operation == Encryptor)
      mcrypt_generic (_cipher, cipherBuff, blockSize);
    else
      mdecrypt_generic (_cipher, cipherBuff, blockSize);

    buff = Buffer(cipherBuff, cipherBuff + blockSize);

    free(cipherBuff);
    cipherBuff = 0;

    mcrypt_generic_deinit(_cipher);

  }

  int BlockCipher::getBlockSize() const
  {
    return mcrypt_enc_get_block_size(_cipher);
  }

  int BlockCipher::getKeySize() const
  {
    return mcrypt_enc_get_key_size(_cipher);
  }

  int BlockCipher::getIVKeySize() const
  {
    return mcrypt_enc_get_iv_size(_cipher);
  }

  const char* BlockCipher::getName() const
  {
    switch(_algorithm)
    {
    case CRYPT_BLOWFISH:
      return "blowfish";
    case CRYPT_DES:
      return "des";
    case CRYPT_3DES:
      return "tripledes";
    case CRYPT_3WAY:
      return "threeway";
    case CRYPT_GOST:
      return "gost";
    case CRYPT_SAFER_SK64:
      return "safer-sk64";
    case CRYPT_SAFER_SK128:
      return "safer-sk128";
    case CRYPT_CAST_128:
      return "cast-128";
    case CRYPT_XTEA:
      return "xtea";
    case CRYPT_RC2:
      return "rc2";
    case CRYPT_TWOFISH:
      return "twofish";
    case CRYPT_CAST_256:
      return "cast-256";
    case CRYPT_SAFERPLUS:
      return "saferplus";
    case CRYPT_LOKI97:
      return "loki97";
    case CRYPT_SERPENT:
      return "serpent";
    case CRYPT_RIJNDAEL_128:
      return "rijndael-128";
    case CRYPT_RIJNDAEL_192:
      return "rijndael-192";
    case CRYPT_RIJNDAEL_256:
      return "rijndael-256";
    case CRYPT_ENIGMA:
      return "enigma";
    case CRYPT_ARCFOUR:
      return "arcfour";
    case CRYPT_WAKE:
      return "wake";
    }
    return "";
  }

  const char* BlockCipher::getModeName() const
  {
    switch(_mode)
    {
    case MODE_CBC:
      return "cbc";
    case MODE_ECB:
      return "ecb";
    case MODE_CFB:
      return "cfb";
    case MODE_OFB:
      return "ofb";
    case MODE_NOFB:
      return "nofb";
    case MODE_STREAM:
      return "stream";
    }

    return "";
  }



} } // OSS::Crypto








