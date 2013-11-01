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


#ifndef SIP_SIPMessage_INCLUDED
#define SIP_SIPMessage_INCLUDED

#include <map>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/SIP/SIPHeaderTokens.h"
#include "OSS/SIP/SIPDigestAuth.h"


namespace OSS {
namespace SIP {


class OSS_API SIPMessage : 
  public SIPParser,
  public SIPDigestAuth,
  public boost::enable_shared_from_this<SIPMessage>
  /// This class is an implementation of a standard SIP Message
  /// supporting 3261, 3262, 3265, 3515, 3892, 3325, 4028 headers.
  /// Other or future as well as proprietary extension headers
  /// are also supported using generic header handling functions.
{
public:
  typedef boost::shared_ptr<SIPMessage> Ptr; /// A shared smart pointer to a SIPMessage object
  typedef boost::shared_lock<boost::shared_mutex> ReadLock;
  typedef boost::lock_guard<boost::shared_mutex> WriteLock;
  typedef std::map<std::string, std::string> CustomProperties;
  static const int NULL_HDR = 0;

  enum StatusCodes
  {
    CODE_UNKNOWN = 0,
    CODE_100_Trying = 100,
    CODE_MIN_CODE = CODE_100_Trying,
    CODE_180_Ringing = 180,
    CODE_181_BeingForwarded = 181,
    CODE_182_Queued = 182,
    CODE_183_SessionProgress = 183,
    CODE_200_Ok = 200,
    CODE_202_Accepted = 202,
    CODE_300_MultipleChoices = 300,
    CODE_301_MovedPermanently = 301,
    CODE_302_MovedTemporarily = 302,
    CODE_305_UseProxy = 305,
    CODE_380_AlternativeService = 380,
    CODE_400_BadRequest = 400,
    CODE_401_Unauthorized = 401,
    CODE_402_PaymentRequired = 402,
    CODE_403_Forbidden = 403,
    CODE_404_NotFound = 404,
    CODE_405_MethodNotAllowed = 405,
    CODE_406_NotAcceptable = 406,
    CODE_407_ProxyAuthenticationRequired = 407,
    CODE_408_RequestTimeout = 408,
    CODE_409_Conflict = 409,
    CODE_410_Gone = 410,
    CODE_411_LengthRequired = 411,
    CODE_412_ConditionalRequestFailed = 412,
    CODE_413_RequestEntityTooLarge = 413,
    CODE_414_RequestURITooLarge = 414,
    CODE_415_UnsupportedMedia = 415,
    CODE_416_UnsupportedURIScheme = 416,
    CODE_420_BadExtension = 420,
    CODE_422_SessionIntervalTooSmall = 422,
    CODE_423_IntervalTooShort = 423,
    CODE_480_TemporarilyNotAvailable = 480,
    CODE_481_TransactionDoesNotExist = 481,
    CODE_482_LoopDetected = 482,
    CODE_483_TooManyHops = 483,
    CODE_484_AddressIncomplete = 484,
    CODE_485_Ambiguous = 485,
    CODE_486_BusyHere = 486,
    CODE_487_RequestCancelled = 487,
    CODE_488_NotAcceptableHere = 488,
    CODE_489_BadRequest = 489,
    CODE_489_BadEvent = CODE_489_BadRequest,
    CODE_491_RequestPending = 491,
    CODE_500_InternalServerError = 500,
    CODE_501_NotImplemented = 501,
    CODE_502_BadGateway = 502,
    CODE_503_ServiceUnavailable = 503,
    CODE_504_GatewayTimeout = 504,
    CODE_505_VersionNotSupported = 505,
    CODE_600_BusyEverywhere = 600,
    CODE_603_Decline = 603,
    CODE_604_DoesNotExistAnywhere = 604,
    CODE_606_NotAcceptable = 606,
    CODE_MAX_CODE = 699
  };

  SIPMessage();
    /// Creates a blank SIP Message

  explicit SIPMessage(const std::string& packet);
    /// Creates a SIP Message from a byte array represented by an std::string.
    ///
    /// Take note tha std::string may accept both none printable characters
    /// and multiple occurence of nil.  This will be handled properly by the
    /// parser specially for cases where the body of a multipart SIP Message
    /// contains binary data.  The SIP Message class delays parsing of the
    /// packet until parse() function is called.  
    ///
    /// Various helper classes are available to process individual SIP headers.
    /// These helper classes are lazy parsers.  It means that they parse the
    /// header every time you use them.  With this in mind, it is practical
    /// to save the parser data somewhere so that it could be reused and obtained
    /// without requiring parse the sip header once again.

  explicit SIPMessage(const SIPMessage& packet);
    /// Creates a SIP Message from another SIP Message Object.

  virtual ~SIPMessage();
    /// Destroys the SIP Message

  void parse();
    /// Parse the SIP message headers.
    ///
    /// This method should be called before actual call to any function
    /// after construction of the SIP Message object.  This allows implementors
    /// to delay parsing of the SIP Message until it is absolutely needed.
    /// SIP Message objects that are instantiated using the copy constructor,
    /// however, would retain the parsed state of the object being copied
    /// and is therefore not required to call this function again.  The parse()
    /// function may throw a SIPParserException if parsing of the message failed.
    /// Finalize would also put bad/unparseable headers
    /// into the _badHeaders vector.  Developers may check the size of the _badHeaders
    /// vector after the call to parse() and decide what to do with them.
    /// If the developer decides to be an RFC police, he can send an outright 400 Bad Request
    /// or one may choose to ignore bad headers if the state of the message allows
    /// the SIP transaction to proceed.

  bool read(std::istream& strm, std::size_t& totalRead);
    /// Parse a SIP Message from a stream
    ///
    /// This method will read and parse the SIP Message from a stream
    /// If successul, the SIPMessage will be fully constructed and ready for access.
    /// If a parser error occurs a SIPParserException will be thrown.
    /// If EOF is encountered and the SIP Message is not fully parsed, the read operation
    /// will return false and totalRead variable will contain the number of bytes that was processed.
    /// If totalRead is greater than zero, the the read operation can be retried against the same stream
    /// as soon as data in the read buffer is available again.  If the read buffer is zero, it
    /// means an unrecoverable error is encountered and further read on the stream
    /// is no longer possible to construct the SIP Message.

  boost::tuple<boost::tribool, const char*> consume(const char* begin, const char* end);
    /// Consumes a complete or incomplete SIP message segment.
    ///
    /// The tribool return value will return indertimate if more input is required.
    /// The char* return value indicates how much of the segment is consumed
    ///
    /// This function was inspired by the asio http request parser.

  bool commitData();
    /// This method updates the _data member variable from the current state of
    /// the header vectors.
    ///
    /// Take note that this method is not thread safe.  Extra
    /// care is needed to make sure that this method is never called simultaneously
    /// by two threads.

  static bool headerTokenize(
    SIPHeaderTokens & lines,
    const std::string & theString,
    const char * lineBreakToken = "\r\n");
    /// Splits the sip header string to a vector of std:string
    /// with each line stored in a separate vector element.
    ///
    /// Wrapped headers will be automatically joined as one element.
    /// This function will throw an exception if you feed it an
    /// empty string argument.

  static bool headerSplit(
    const std::string & header,
    std::string & name,
    std::string & value);
    /// Splits a header token into the name and value.
    ///
    /// Example:  From: "Alice" <sip:alice@atlanta.com> will be
    /// split to name="From" and value="\"Alice\" <sip:alice@atlanta.com>"

  static std::string headerGetCompactForm(
    const std::string & header);
    /// Returns the compact form of a header name.
    ///
    /// Returns the expanded form if compact form is not available for the header.

  static bool messageSplit(
    std::string & message,
    std::string & headers,
    std::string & body);
    /// Splits a SIP Message string into headers and body element.
    ///
    /// The body element is a byte array since SIP allows the body
    /// of a SIP message to contain binary data and my contain multiple
    /// nil characters.  Extra care must be done to not treat the body
    /// as nil terminated.

  static bool messageSplit(
    std::string & message,
    SIPHeaderTokens& headers,
    std::string & body);
    /// Splits a SIP Message string into headers and body element.
    ///
    /// Headers will be automatically tokenized into a vector of string.
    /// The body element is a byte array since SIP allows the body
    /// of a SIP message to contain binary data and my contain multiple
    /// nil characters.  Extra care must be done to not treat the body
    /// as nil terminated.  

  size_t hdrPresent(const char* headerName) const;
    /// This function checks for the existence of a certain header
    /// in the header list.  
    ///
    /// If the header does not exist, it will
    /// return 0.  If the header exists, this function will return
    /// the number of elements contained in the headers vector.
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function).

  size_t hdrGetSize(const char* headerName) const;
    /// This function simply calls hdrPresent to return the size of the header list.
    ///
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function).
    ///
    ///   size_t sz = msg.hdrGet("Via"); 
    ///

  const std::string& hdrGet(const char* headerName, size_t index = 0) const;
    /// Returns the value of the header at a particular index in the list.
    ///
    /// If the header is not present, this function will return a reference
    /// to an empty string _headerEmptyRet.
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function).
    ///
    ///   std::string via = msg.hdrGet("Via"); 
    ///
 
  bool hdrSet( const char * headerName, const std::string& headerValue);
    /// Sets the value of the header.  
    ///
    /// If the header is not present, it will be created.
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function).
    ///
    ///   msg.hdrSet("From", "\"alice\"<sip:alice@atlanta.com>tag=123"); 
    ///

  bool hdrSet(const char* headerName, const std::string& headerValue, size_t index);
    /// Sets the value of the header at a particular index in the list.
    ///
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function) or 
    /// a SIPInvalidIndexException if the index specified is beyond the bounds of the array size.
    ///
    ///   msg.hdrSet("Via", "SIP/2.0/UDP pc33.atlanta.com;branch=z9hG4bK776asdhds", 0); 
    ///

  bool hdrRemove(const char* headerName);
    /// Removes a particular header from the SIPMessage.
    /// If the header does not exist, this function will return false.
    /// If the header being removed has more than one element (multiple Record-Route headers for example),
    /// this function will throw SIPParserException.  If the intention is to merely remove the top-most
    /// element, use hdrListPopFront() function or hdrListErase() if the intention is to clear all the 
    /// elements of a list

  bool hdrListAppend(const char* name, const std::string& value);
    /// Appends the header into the the corresponding header token vector.
    ///
    /// Take note that this function will not hinder anyone from violating
    /// SIP Message rules for none list headers. 
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function)

  bool hdrListPrepend(const char* name, const std::string& value);
    /// Prepends the header into the the corresponding header token vector.
    ///
    /// Take note that this function will not hinder anyone from violating
    /// SIP Message rules for none list headers. 
    /// This function will throw a SIPInvalidStateException
    /// if the SIP Message is not yet parsed (see parse() function)

  std::string hdrListPopFront(const char* name);
    /// Pop the first element in a list header.
    /// If the list header has only one element,
    /// the header will be deleted from the list

  bool hdrListRemove(const char* name);
    /// Erase a list header including all its elements.  
    /// To simply remove the first element in a list header,
    /// use hdrListPopFront instead.

  SIPMessage::Ptr createResponse(
    int statusCode,
    const std::string& reasonPhrase = "", 
    const std::string& toTag = "",
    const std::string& contact = "");
    /// create a response based on this SIP Request
    ///
    /// For responses that would not require a to tag
    /// or contact (eg. 100 Trying) both parameters
    /// may be set to empty strings.
    ///
    /// If the request being responded to already has a to tag
    /// and the toTag parameter is not empty, this function
    /// will throw a SIPException.
    ///
    /// The reasonPhrase parameter may hold a string replacement
    /// for the default reason phrase recommended in RFC 3261.
    /// If left blank, this function will use the stock
    /// reason phrase.
    ///
    /// The Content-Length for the response will always
    /// be set to zero.  It is the responsibility of the
    /// application to set this to the correct value
    /// if a body for the response is needed.

  SIPMessage::Ptr reformatResponse(const SIPMessage::Ptr& pResponse);
    /// Reformat another response so that it uses
    /// the transaction state for this request.
    ///
    /// Take note that the Contact header is not set by this function

  void swap(SIPMessage& packet);
    /// Exchanges the content of two messages.

  SIPHeaderTokens& badHeaders();
    /// Returns the bad header vector

  SIPMessage& operator = (const SIPMessage & copy);
    /// Copy the content of another SIP message

  SIPMessage& operator = (const std::string& data);
    /// Copy the content of a SIP message from a raw string

  bool getTransactionId(std::string& transactionId, const char* method = 0) const;
    /// Create a transaction id.
    ///
    /// This will be used by the transaction layer to identify
    /// the owning transaction 
    ///
    /// Format:
    ///   transaction-id = method  cseq  (via-branch / callid)

  boost::tribool isRequest(const char* method = 0) const;
    /// Returns true if the SIP Message is a request.
    ///
    /// if method parameter is specified, this function will
    /// simply return true or false depending on whether the
    /// request method euqulas that of the request start-line.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.
    ///
    

  boost::tribool isResponse() const;
    /// Returns true if the SIP Message is a response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool isResponseTo(const char* method) const;
    /// Returns true if the SIP Message is a response to a particular request.
    ///
    /// This method will return inderterminate if the request is neither
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.
    ///

  boost::tribool is1xx(int code = 0) const;
    /// Returns true if the SIP Message is a 1xx response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.
   
  boost::tribool is2xx(int code = 0) const;
    /// Returns true if the SIP Message is a 2xx response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool is3xx(int code = 0) const;
    /// Returns true if the SIP Message is a 3xx response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool is4xx(int code = 0) const;
    /// Returns true if the SIP Message is a 4xx response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool is5xx(int code = 0) const;
    /// Returns true if the SIP Message is a 5xx response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool is6xx(int code = 0) const;
    /// Returns true if the SIP Message is a 1xx response.
    ///
    /// This method will return inderterminate if the request is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool isResponseFamily(int responseCode) const;
    /// Returns true if the SIP Message belongs to the response code group (100, 200, 300, 400, 500, 600)
    ///
    /// This method will return inderterminate if the message is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool isErrorResponse() const;
    /// Returns true if the SIP Message belongs to the response code group (300, 400, 500, 600)
    ///
    /// This method will return inderterminate if the message is neither 
    /// a request nor a response.  It will throw SIPABNFSyntaxException
    /// if the start line is empty.

  boost::tribool isMidDialog() const;
    /// Determines whether the message is sent within a dialog
    ///
    /// Returns true if both from and to tags are set.
    /// Returns false if only from tag is set and to tag is empty
    /// Returns indeterminate if both tags are not set

  std::string getDialogId(bool asSender) const;
    /// Returns the dialogId in the form local-tag + remote-tag.
    ///
    /// If the asSender is true - For responses, the local-tag will be extracted from the to-header
    /// and the remote-tag from the from header.  It's the reverse for requests.
    ///
    /// The inverse of the prior rule will be true if the asSender flag is false

  std::string& body();
    /// Returns the body of the SIP Message if present;
    /// Take note that this is not thread safe
    /// use getBody() and setBody() instead for
    /// thread safe operations

  const std::string& getBody() const;
    /// Returns the body of the SIP Message if present

  void setBody(const std::string& body);
    /// Set the body value of the SIP Message
    /// Take note that this will not change the value
    /// of content-length.  It is the responsibility
    /// of the developer to set content-length
    /// according to the new length of the body.

  void updateLength();
    /// Update the content-length header based on the current length of the
    /// message body

  std::string& startLine();
    /// Returns a reference to the Start Line
    /// Take note that this is not thread safe
    /// use getStartLine() and setStartLine() instead for
    /// thread safe operations

  const std::string& getStartLine() const;
    /// Return the start line value

  void setStartLine(const std::string& startLine);
    /// Set the start-line value

  void setData(const std::string& data);
    /// Set the input for parsing

  void setProperty(const std::string& property, const std::string& value);
    /// Set a custom property for this message.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the sip message
    /// is processed.  Custom propertis are never
    /// inherited when copying sip messages to another.

  std::string getMethod() const;
    /// Return the method portion of the CSeq.  This function would behae the same
    /// for both requests and responses.

  bool getProperty(const std::string&  property, std::string& value) const;
    /// Get a custom property of this message.
    /// Custom properties are meant to simply hold
    /// arbitrary data to aid in how the sip message
    /// is processed.  Custom propertis are never
    /// inherited when copying sip messages to another.

  void clearProperties();
    /// Remove all custom properties

  OSS_HANDLE& userData();
    /// Returns a reference to the user data

  std::string createContextId(bool formatTabAndSpaces = false) const;
    /// Creates a 32 bit context id.  This is normally used for
    /// tagging transactions belonging to the same call-id.
    /// A bitwise hash will be used against the call-id header.

  static std::string createContextId(SIPMessage* pMsg, bool formatTabAndSpaces = false);
    /// Creates a 32 bit context id.  This is normally used for
    /// tagging transactions belonging to the same call-id.
    /// A bitwise hash will be used against the call-id header.

  std::string createLoggerData() const;
    /// Returns a log-ready string representation of the SIP Message

  static std::string createLoggerData(SIPMessage* pMsg);
    /// Returns a log-ready string representation of the SIP Message

  std::string& idleBuffer();
    /// Returns the buffer containing bytes that are dropped during idle state.
    /// This is normally used to determine if the consume() method has dropped
    /// leading CRLF bytes

  std::string getFromTag() const;
    /// Return the tag parameter of the From header.
  
  std::string getFromHost() const;
    /// Return the from host (no port)
  
  std::string getFromHostPort() const;
    /// Return the from host (including port)
  
  std::string getToHost() const;
    /// Return the from host (no port)
  
  std::string getToHostPort() const;
    /// Return the from host (including port)

  std::string getToTag() const;
    /// Return the to tag parameter of the To header;

  std::string getTopViaBranch() const;
    /// Return the top via branhc parameter

protected:
  boost::tribool consumeOne(char input);
  enum ConsumeState
  {
    IDLE,
    START_LINE_PARSE,
    EXPECTING_NEW_LINE_1,
    HEADER_LINE_START,
    HEADER_LWS,
    HEADER_NAME,
    HEADER_VALUE,
    EXPECTING_NEW_LINE_2,
    EXPECTING_NEW_LINE_3,
    EXPECTING_BODY
  } _consumeState;

  mutable boost::shared_mutex _rwlock;

  bool _finalized;
  std::string _startLine;
  std::string _body;
  SIPHeaderTokens _badHeaders;
  SIPHeaderList _headers;
  static std::string _headerEmptyRet;
  size_t _headerOffSet;
  std::size_t _expectedBodyLen;
  mutable boost::tribool _isResponse;
  mutable boost::tribool _isRequest;
  CustomProperties _properties;
  OSS_HANDLE _userData;
  std::string _idleBuffer;
};

//
// Inlines
//

inline size_t SIPMessage::hdrGetSize(
  const char* headerName)const
{
  return hdrPresent(headerName);
}

inline SIPHeaderTokens & SIPMessage::badHeaders()
{
  return _badHeaders;
}

inline std::string& SIPMessage::body()
{
  return _body;
}

inline std::string& SIPMessage::startLine()
{
  return _startLine;
}

inline OSS_HANDLE& SIPMessage::userData()
{
  return _userData;
}

inline std::string SIPMessage::createContextId(bool formatTabAndSpaces) const
{
  return createContextId(const_cast<SIPMessage*>(this), formatTabAndSpaces);
}

inline std::string SIPMessage::createLoggerData() const
{
  return createLoggerData(const_cast<SIPMessage*>(this));
}

inline std::string& SIPMessage::idleBuffer()
{
  return _idleBuffer;
}


}} //OSS::SIP
#endif //SIP_SIPMessage_INCLUDED

