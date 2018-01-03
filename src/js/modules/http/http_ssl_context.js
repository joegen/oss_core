"use-strict";

const _http_ssl_context = require("./_http_ssl_context.jso");
const SSLContext = _http_ssl_context.HttpSSLContext;
const EventEmitter = async.EventEmitter;
const isolate = require("isolate");

///   * usage specifies whether the context is used by a client or server.
///   * privateKeyFile contains the path to the private key file used for encryption.
///     Can be empty if no private key file is used.
///   * certificateFile contains the path to the certificate file (in PEM format).
///     If the private key and the certificate are stored in the same file, this
///     can be empty if privateKeyFile is given.
///   * caLocation contains the path to the file or directory containing the
///     CA/root certificates. Can be empty if the OpenSSL builtin CA certificates
///     are used (see loadDefaultCAs).
///   * verificationMode specifies whether and how peer certificates are validated.
///   * verificationDepth sets the upper limit for verification chain sizes. Verification
///     will fail if a certificate chain larger than this is encountered.
///   * loadDefaultCAs specifies wheter the builtin CA certificates from OpenSSL are used.
///   * cipherList specifies the supported ciphers in OpenSSL notation.
var HttpSSLContext = function(
  usage,
  privateKeyFile,
  certificateFile,
  caLocation,
  verificationMode,
  verificationDepth,
  loadDefaultCAs,
  cipherList)
{
  if (verificationMode === undefined)
  {
    verificationMode = HttpSSLContext.VERIFY_RELAXED;
  }
  if (verificationDepth === undefined)
  {
    verificationDepth = 9;
  }
  if (loadDefaultCAs === undefined)
  {
    loadDefaultCAs = false;
  }
  if (cipherList === undefined)
  {
    cipherList = "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH";
  }
  
  var _context = new SSLContext();
  var _passphraseHandlerId = "HttpSSLContext_onPrivateKeyRequested_" + HttpSSLContext._INSTANCE_COUNTER;
  var _invalidCertificateHandlerId = "HttpSSLContext_onInvalidCertificate_" + HttpSSLContext._INSTANCE_COUNTER;
  ++HttpSSLContext._INSTANCE_COUNTER;
  
  var _passphraseHandlerCb = undefined;
  var passphraseHandlerCb = function()
  {
    var ret = _passphraseHandlerCb(certificate);
    if (ret)
    {
      return { result : { password : ret } }
    }
    else
    {
      return { result : {} }
    }
  }
  
  var _invalidCertificateHandlerCb = undefined;
  var invalidCertificateHandlerCb = function(certificate)
  {
    var ret = _invalidCertificateHandlerCb(certificate);
    return { result : { allow : !!ret } }
  }
  
  this.registerContext = function(passpharaseHandler, invalidCertificateHandler)
  {
    _context.setUsage(usage);
    _context.setPrivateKeyFile(privateKeyFile);
    _context.setCertificateKeyFile(certificateFile);
    _context.setCaLocation(caLocation);
    _context.setVerificationMode(verificationMode);
    _context.setVerificationDepth(verificationDepth);
    _context.setLoadDefaultCa(loadDefaultCAs);
    _context.setCipherList(cipherList);
    _context.setPassphraseHandlerId(_passphraseHandlerId);
    _context.setInvalidCertificateHandlerId(_invalidCertificateHandlerId);
    _context.registerContext();
    
    _passphraseHandlerCb = passpharaseHandler;
    _invalidCertificateHandlerCb = invalidCertificateHandler;
    
    isolate.on(_passphraseHandlerId, passphraseHandlerCb);
    isolate.on(_invalidCertificateHandlerId, invalidCertificateHandlerCb);
  }
  
  this.unregisterContext = function()
  {
    isolate.remove(_passphraseHandlerId);
    isolate.remove(_invalidCertificateHandlerId);
  }
}
HttpSSLContext._INSTANCE_COUNTER = 0;

exports.CLIENT_USE = HttpSSLContext.CLIENT_USE = _http_ssl_context.CLIENT_USE /// Context is used by a client.
exports.SERVER_USE = HttpSSLContext.SERVER_USE = _http_ssl_context.SERVER_USE  /// Context is used by a server.

exports.VERIFY_NONE = HttpSSLContext.VERIFY_NONE    = _http_ssl_context.VERIFY_NONE;
  /// Server: The server will not send a client certificate 
  /// request to the client, so the client will not send a certificate. 
  ///
  /// Client: If not using an anonymous cipher (by default disabled), 
  /// the server will send a certificate which will be checked, but
  /// the result of the check will be ignored.

exports.VERIFY_RELAXED = HttpSSLContext.VERIFY_RELAXED = _http_ssl_context.VERIFY_RELAXED;
  /// Server: The server sends a client certificate request to the 
  /// client. The certificate returned (if any) is checked. 
  /// If the verification process fails, the TLS/SSL handshake is 
  /// immediately terminated with an alert message containing the 
  /// reason for the verification failure. 
  ///
  /// Client: The server certificate is verified, if one is provided. 
  /// If the verification process fails, the TLS/SSL handshake is
  /// immediately terminated with an alert message containing the 
  /// reason for the verification failure. 

exports.VERIFY_STRICT = HttpSSLContext.VERIFY_STRICT  = _http_ssl_context.VERIFY_STRICT;
  /// Server: If the client did not return a certificate, the TLS/SSL 
  /// handshake is immediately terminated with a handshake failure
  /// alert. 
  ///
  /// Client: Same as VERIFY_RELAXED. 

exports.VERIFY_ONCE = HttpSSLContext.VERIFY_ONCE    = _http_ssl_context.VERIFY_ONCE;
  /// Server: Only request a client certificate on the initial 
  /// TLS/SSL handshake. Do not ask for a client certificate 
  /// again in case of a renegotiation.
  ///
  /// Client: Same as VERIFY_RELAXED.	

var _ssl_client_context = undefined;

exports.hasClientContext = function()
{
  return _ssl_client_context !== undefined;
}

exports.registerClientContext = function(
  privateKeyFile,
  passphraseFunction, 
  certificateFile,
  validateCertFunction,
  caLocation, 
  verificationMode, 
  verificationDepth, 
  loadDefaultCAs, 
  cipherList
)
{
  if (_ssl_client_context)
  {
    throw new Error("Client context already set");
  }
  _ssl_client_context = new HttpSSLContext(
    HttpSSLContext.CLIENT_USE,
    privateKeyFile,
    certificateFile,
    caLocation, 
    verificationMode, 
    verificationDepth, 
    loadDefaultCAs, 
    cipherList);
    _ssl_client_context.registerContext(passphraseFunction, validateCertFunction);
}

var _ssl_server_context = undefined;

exports.hasServerContext = function()
{
  return _ssl_server_context !== undefined;
}

exports.registerServerContext = function(
  privateKeyFile,
  passphraseFunction, 
  certificateFile,
  validateCertFunction,
  caLocation, 
  verificationMode, 
  verificationDepth, 
  loadDefaultCAs, 
  cipherList
)
{
  if (_ssl_server_context)
  {
    throw new Error("Server context already set");
  }
  
  _ssl_server_context = new HttpSSLContext(
    HttpSSLContext.SERVER_USE,
    privateKeyFile,
    certificateFile,
    caLocation, 
    verificationMode, 
    verificationDepth, 
    loadDefaultCAs, 
    cipherList);
    _ssl_client_context.registerContext(passphraseFunction, validateCertFunction);
}