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

#ifndef OSS_HTTPSSLCONTEXTOBJECT_H_INCLUDED
#define OSS_HTTPSSLCONTEXTOBJECT_H_INCLUDED


#include <Poco/Net/NetException.h>
#include <Poco/Net/SSLException.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/PrivateKeyPassphraseHandler.h>
#include <Poco/Net/SSLManager.h>
#include "OSS/JS/JSPlugin.h"


class HttpSSLContextObject: public OSS::JS::JSObjectWrap
{
public:
  typedef Poco::Net::Context Context;
  typedef Poco::Net::InvalidCertificateHandler InvalidCertificateHandler;
  typedef Poco::Net::PrivateKeyPassphraseHandler PrivateKeyPassphraseHandler;
  typedef Poco::SharedPtr<PrivateKeyPassphraseHandler> PrivateKeyPassphraseHandlerPtr;
  typedef Poco::SharedPtr<InvalidCertificateHandler> InvalidCertificateHandlerPtr;
  typedef Poco::Net::VerificationErrorArgs VerificationErrorArgs;
  typedef Poco::Net::SSLManager SSLManager;
  
  JS_CONSTRUCTOR_DECLARE();
  JS_METHOD_DECLARE(setUsage);
  JS_METHOD_DECLARE(getUsage);
  JS_METHOD_DECLARE(setPrivateKeyFile);
  JS_METHOD_DECLARE(getPrivateKeyFile);
  JS_METHOD_DECLARE(setCertificateKeyFile);
  JS_METHOD_DECLARE(getCertificateKeyFile);
  JS_METHOD_DECLARE(setCaLocation);
  JS_METHOD_DECLARE(getCaLocation);
  JS_METHOD_DECLARE(setVerificationMode);
  JS_METHOD_DECLARE(getVerificationMode);
  JS_METHOD_DECLARE(setVerificationDepth);
  JS_METHOD_DECLARE(getVerificationDepth);
  JS_METHOD_DECLARE(setLoadDefaultCa);
  JS_METHOD_DECLARE(getLoadDefaultCa);
  JS_METHOD_DECLARE(setCipherList);
  JS_METHOD_DECLARE(getCipherList);
  JS_METHOD_DECLARE(setPassphraseHandlerId);
  JS_METHOD_DECLARE(setInvalidCertificateHandlerId);
  
  JS_METHOD_DECLARE(registerContext);

protected:
  HttpSSLContextObject();
  ~HttpSSLContextObject();
  
  Context::Ptr _pContext;
  PrivateKeyPassphraseHandlerPtr _pPassPhraseHandler;
  InvalidCertificateHandlerPtr _pInvalidCertHandler;
  std::string _passPhraseHandlerId;
  std::string _invalidCertHandlerId;
  
  std::string _privateKeyFile;
  std::string _certificateFile;
  std::string _caLocation;
  Context::VerificationMode _verificationMode;
  int _verificationDepth;
  bool _loadDefaultCAs;
  std::string _cipherList;
  Context::Usage _usage;

  
  friend class CertificateValidator;
  friend class PassPhraseValidator;
};

#endif // OSS_HTTPSSLCONTEXTOBJECT_H_INCLUDED

