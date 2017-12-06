#include <OSS/JSON/elements.h>

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include "OSS/UTL/Logger.h"
#include "OSS/JSON/Json.h"
#include "OSS/JS/modules/Async.h"
#include "OSS/JS/modules/HttpSSLContextObject.h"

using OSS::JS::ObjectWrap;

JS_CLASS_INTERFACE(HttpSSLContextObject, "HttpSSLContext") 
{
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setUsage", setUsage);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getUsage", getUsage);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setPrivateKeyFile", setPrivateKeyFile);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getPrivateKeyFile", getPrivateKeyFile);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setCertificateKeyFile", setCertificateKeyFile);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getCertificateKeyFile", getCertificateKeyFile);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setCaLocation", setCaLocation);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getCaLocation", getCaLocation);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setVerificationMode", setVerificationMode);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getVerificationMode", getVerificationMode);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setVerificationDepth", setVerificationDepth);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getVerificationDepth", getVerificationDepth);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setLoadDefaultCa", setLoadDefaultCa);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getLoadDefaultCa", getLoadDefaultCa);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setCipherList", setCipherList);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "getCipherList", getCipherList);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setPassphraseHandlerId", setPassphraseHandlerId);
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "setInvalidCertificateHandlerId", setInvalidCertificateHandlerId);
  
  JS_CLASS_METHOD_DEFINE(HttpSSLContextObject, "registerContext", registerContext);
  
  JS_CLASS_INTERFACE_END(HttpSSLContextObject);
}

class CertificateValidator : public HttpSSLContextObject::InvalidCertificateHandler
{
public:
  HttpSSLContextObject* _pContext;
  
  CertificateValidator(HttpSSLContextObject* pContext) :
    HttpSSLContextObject::InvalidCertificateHandler(pContext->_usage == HttpSSLContextObject::Context::SERVER_USE ? true : false)
  {
    _pContext = pContext;
  }
  
  ~CertificateValidator()
  {
  }
  
  void onInvalidCertificate(const void* pSender, HttpSSLContextObject::VerificationErrorArgs& errorCert)
  {
    OSS::JSON::Object request, certificate, result;
    
    request["method"] = OSS::JSON::String(_pContext->_invalidCertHandlerId);
    const Poco::Net::X509Certificate& cert = errorCert.certificate();
    
    certificate["errorMessage"] = OSS::JSON::String(errorCert.errorMessage());
    certificate["commonName"] = OSS::JSON::String(cert.commonName());
    certificate["issuerName"] = OSS::JSON::String(cert.issuerName());
    certificate["subjectName"] = OSS::JSON::String(cert.subjectName());
    
    std::set<std::string> domainNames;
    std::string commonaName;
    cert.extractNames(commonaName, domainNames);
    if (!domainNames.empty())
    {
      OSS::JSON::Array aliases;
      std::size_t index = 0;
      for (std::set<std::string>::iterator iter = domainNames.begin(); iter != domainNames.end(); iter++)
      {
        aliases[index] = OSS::JSON::String(*iter);
        index++;
        certificate["domains"] = aliases;
      }
    }
    
    request["arguments"] = certificate;
    if (Async::json_execute_promise(request, result))
    {
      OSS::JSON::Object::iterator iter = result.Find("result");
      if (iter != result.End())
      {
        OSS::JSON::Object& resultValue = iter->element;
        OSS::JSON::Object::iterator allowIter = resultValue.Find("allow");
        if (allowIter != resultValue.End())
        {
          OSS::JSON::Boolean& allow = allowIter->element;
          errorCert.setIgnoreError(allow.Value());
        }
      }
    }
  }
};

class PassPhraseValidator : public HttpSSLContextObject::PrivateKeyPassphraseHandler
{
public:
  HttpSSLContextObject* _pContext;
  
  PassPhraseValidator(HttpSSLContextObject* pContext) :
    HttpSSLContextObject::PrivateKeyPassphraseHandler(pContext->_usage == HttpSSLContextObject::Context::SERVER_USE ? true : false)
  {
    _pContext = pContext;
  }
  
  ~PassPhraseValidator()
  {
  }
  
  void onPrivateKeyRequested(const void* pSender, std::string& privateKey)
  {
    OSS::JSON::Object request, result;
    request["method"] = OSS::JSON::String(_pContext->_passPhraseHandlerId);
    if (Async::json_execute_promise(request, result))
    {
   
      OSS::JSON::Object::iterator iter = result.Find("result");
      if (iter != result.End())
      {
        OSS::JSON::Object& resultValue = iter->element;
        OSS::JSON::Object::iterator passwordIter = resultValue.Find("password");
        if (passwordIter != resultValue.End())
        {
          OSS::JSON::String& password = passwordIter->element;
          privateKey = password.Value();
        }
      }
    }
  }
};

HttpSSLContextObject::HttpSSLContextObject()
{
  _usage = Context::CLIENT_USE;
  _cipherList = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
  _verificationMode = Context::VERIFY_RELAXED;
  _verificationDepth = 9;
  _loadDefaultCAs = false;
}

HttpSSLContextObject::~HttpSSLContextObject()
{
}

JS_CONSTRUCTOR_IMPL(HttpSSLContextObject)
{
  js_enter_scope();
  HttpSSLContextObject* pObject = new HttpSSLContextObject();
  pObject->Wrap(js_method_arg_self());
  return js_method_arg_self();
}

JS_METHOD_IMPL(HttpSSLContextObject::setUsage)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_int32(usage, 0);
  self->_usage = (Context::Usage)usage;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::getUsage)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSInt32(self->_usage);
}

JS_METHOD_IMPL(HttpSSLContextObject::setPrivateKeyFile)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_string(privateKeyFile, 0);
  self->_privateKeyFile = privateKeyFile;
  return JSUndefined();
}
JS_METHOD_IMPL(HttpSSLContextObject::getPrivateKeyFile)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSString(self->_privateKeyFile);
}
JS_METHOD_IMPL(HttpSSLContextObject::setCertificateKeyFile)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_string(certificateFile, 0);
  self->_certificateFile = certificateFile;
  return JSUndefined();
}
JS_METHOD_IMPL(HttpSSLContextObject::getCertificateKeyFile)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSString(self->_certificateFile);
}

JS_METHOD_IMPL(HttpSSLContextObject::setCaLocation)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_string(caLocation, 0);
  self->_caLocation = caLocation;
  return JSUndefined();
}
JS_METHOD_IMPL(HttpSSLContextObject::getCaLocation)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSString(self->_caLocation);
}

JS_METHOD_IMPL(HttpSSLContextObject::setCipherList)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_string(cipherList, 0);
  self->_cipherList = cipherList;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::getCipherList)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSString(self->_cipherList);
}

JS_METHOD_IMPL(HttpSSLContextObject::setVerificationMode)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_int32(verificationMode, 0);
  self->_verificationMode = (Context::VerificationMode)verificationMode;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::getVerificationMode)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSInt32(self->_verificationMode);
}

JS_METHOD_IMPL(HttpSSLContextObject::setVerificationDepth)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_int32(verificationDepth, 0);
  self->_verificationDepth = verificationDepth;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::getVerificationDepth)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSInt32(self->_verificationDepth);
}

JS_METHOD_IMPL(HttpSSLContextObject::setLoadDefaultCa)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_bool(loadDefaultCa, 0);
  self->_loadDefaultCAs = loadDefaultCa;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::getLoadDefaultCa)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  return JSBoolean(self->_loadDefaultCAs);
}

JS_METHOD_IMPL(HttpSSLContextObject::setInvalidCertificateHandlerId)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_string(invalidCertHandlerId, 0);
  self->_invalidCertHandlerId = invalidCertHandlerId;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::setPassphraseHandlerId)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  js_method_arg_declare_string(passPhraseHandlerId, 0);
  self->_passPhraseHandlerId = passPhraseHandlerId;
  return JSUndefined();
}

JS_METHOD_IMPL(HttpSSLContextObject::registerContext)
{
  js_enter_scope();
  HttpSSLContextObject* self = js_method_arg_unwrap_self(HttpSSLContextObject);
  
  if (self->_pContext)
  {
    js_throw("Context Already Registered");
  }
  
  try
  {
    self->_pPassPhraseHandler = new PassPhraseValidator(self);
    self->_pInvalidCertHandler = new CertificateValidator(self);
    self->_pContext = new Context(
      self->_usage, 
      self->_privateKeyFile, 
      self->_certificateFile, 
      self->_caLocation, 
      self->_verificationMode, 
      self->_verificationDepth, 
      self->_loadDefaultCAs, 
      self->_cipherList);

    if (self->_usage == HttpSSLContextObject::Context::SERVER_USE)
    {
      SSLManager::instance().initializeServer(self->_pPassPhraseHandler, self->_pInvalidCertHandler, self->_pContext);
    }
    else
    {
      SSLManager::instance().initializeClient(self->_pPassPhraseHandler, self->_pInvalidCertHandler, self->_pContext);
    }
  }
  catch(const Poco::Exception& e)
  {
    js_throw(e.message().c_str());
  }
  return JSUndefined();
}

JS_EXPORTS_INIT()
{
  js_enter_scope();
  js_export_class(HttpSSLContextObject);
  
  js_export_int32("CLIENT_USE", HttpSSLContextObject::Context::CLIENT_USE);
  js_export_int32("SERVER_USE", HttpSSLContextObject::Context::SERVER_USE);
  js_export_int32("VERIFY_NONE", HttpSSLContextObject::Context::VERIFY_NONE);
  js_export_int32("VERIFY_RELAXED", HttpSSLContextObject::Context::VERIFY_RELAXED);
  js_export_int32("VERIFY_STRICT", HttpSSLContextObject::Context::VERIFY_STRICT);
  js_export_int32("VERIFY_ONCE", HttpSSLContextObject::Context::VERIFY_ONCE);
  
  js_export_finalize(); 
}

JS_REGISTER_MODULE(JSHttpSSLContext);


