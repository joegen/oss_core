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


#include <list>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include "OSS/Core.h"
#include "OSS/SIP/SIPMessage.h"
#include "OSS/ABNF/ABNFParser.h"
#include "OSS/ABNF/ABNFSIPRules.h"
#include "OSS/ABNF/ABNFSIPToken.h"
#include "OSS/ABNF/ABNFSIPHostPort.h"
#include "OSS/ABNF/ABNFSIPRequestLine.h"
#include "OSS/ABNF/ABNFSIPStatusLine.h"
#include "OSS/SIP/SIPVia.h"
#include "OSS/SIP/SIPCSeq.h"
#include "OSS/SIP/SIPFrom.h"
#include "OSS/Logger.h"

namespace OSS {
namespace SIP {


using namespace OSS::ABNF;
std::string SIPMessage::_headerEmptyRet = "";
static ABNFEvaluate<ABNFSIPRequestLine> requestLineVerify;
static ABNFEvaluate<ABNFSIPStatusLine> statusLineVerify;


SIPMessage::SIPMessage() :
  _consumeState(IDLE),
  _finalized(true),
  _headerOffSet(0),
  _expectedBodyLen(0),
  _isResponse(boost::indeterminate),
  _isRequest(boost::indeterminate)
{
  _idleBuffer.reserve(4);
}

SIPMessage::~SIPMessage()
{
}

SIPMessage::SIPMessage(const std::string& packet) :
  _finalized(false),
  _headerOffSet(0),
  _expectedBodyLen(0),
  _isResponse(boost::indeterminate),
  _isRequest(boost::indeterminate)
{
  _data = packet;
  parse();
}

SIPMessage::SIPMessage(const SIPMessage& packet)
{
  ReadLock lock(packet._rwlock); 

  _finalized = packet._finalized;
  _data = packet._data;
  _startLine = packet._startLine;
  _body = packet._body;
  _badHeaders = packet._badHeaders;
  _headers = packet._headers;
  _headerOffSet = packet._headerOffSet;
  _expectedBodyLen = packet._expectedBodyLen;
  _isResponse = packet._isResponse;
  _isRequest = packet._isRequest;
}

void SIPMessage::swap(SIPMessage& packet)
{
  ReadLock lock(packet._rwlock); 

  std::swap(_finalized, packet._finalized);
  std::swap(_data, packet._data);
  std::swap(_startLine, packet._startLine);
  std::swap(_body, packet._body);
  std::swap(_badHeaders, packet._badHeaders);
  std::swap(_headers, packet._headers);
  std::swap(_headerOffSet, packet._headerOffSet);
  std::swap(_expectedBodyLen, packet._expectedBodyLen);
  std::swap(_isResponse, packet._isResponse);
  std::swap(_isRequest, packet._isRequest);
}

SIPMessage & SIPMessage::operator=(const SIPMessage & copy)
{ 
  SIPMessage msg(copy);

  WriteLock lock(_rwlock);
  swap(msg);
  return *this;
}

SIPMessage& SIPMessage::operator = (const std::string& data)
{
  _finalized = false;
  _data = data;
  _startLine = "";
  _body = "";
  _badHeaders.clear();
  _headers.clear();
  _headerOffSet = 0;
  _expectedBodyLen = 0;
  _isResponse = boost::indeterminate;
  _isRequest = boost::indeterminate;
  parse();
  return *this;
}



#if 0

static std::string headerGetCompactForm(const std::string & header)
{
  std::string h = header;
  boost::to_lower(h);

  if( h == "accept-contact" )
    return "a";
  else if( h == "referred-by" )
    return "b";
  else if( h == "content-type" )
    return "c";
  else if( h == "content-encoding" )
    return "e";
  else if( h == "from" )
    return "f";
  else if( h == "call-id" )
    return "i";
  else if( h == "supported" )
    return "k";
  else if( h == "content-length" )
    return "l";
  else if( h == "contact" )
    return "m";
  else if( h == "event" )
    return "o";
  else if( h == "refer-to" )
    return "r";
  else if( h == "subject" )
    return "s";
  else if( h == "to" )
    return "t";
  else if( h == "allow-events" )
    return "u";
  else if( h == "via" )
    return "v";

  return "";
}
#endif

static std::string hdrGetExpandedForm(const std::string & header)
{
  std::string h = header;
  boost::to_lower(h);

  if( h == "a" ) //a
    return "Accept-Contact";
  else if( h == "b" )//b
    return "Referred-By";
  else if( h == "c" )//c
    return "Content-Type";
  else if( h == "e" )//e
    return "Content-Encoding";
  else if( h == "f" )//f
    return "From";
  else if( h == "i" )//i
    return "Call-ID";
  else if( h == "k" )//k
    return "Supported";
  else if( h == "l" )//l
    return "Content-Length";
  else if( h == "m" )//m
    return "Contact";
  else if( h == "o" )//o
    return "Event";
  else if( h == "r" )//r
    return "Refer-To";
  else if( h == "s" )//s
    return "Subject";
  else if( h == "t" )//t
    return "to";
  else if( h == "u" )//u
    return "Allow-Events";
  else if( h == "v" )//v
    return "Via";

  return header;
}

void SIPMessage::parse()
{
  WriteLock lock(_rwlock);

  if (_finalized)
    return;

  SIPHeaderTokens headers;
  if (_data.empty())
    return;

  for (std::string::iterator iter = _data.begin(); iter != _data.end();)
  {
    if (!isChar(*iter) || *iter == '\r' || *iter == '\n')
      iter = _data.erase(iter);
    else
      break;
  }

  if ( messageSplit(_data, headers, _body) )
  {
    if (headers.empty())
      return;
    _badHeaders.clear();
    SIPHeaderTokens::iterator iter;
    iter = headers.begin();
    _startLine = *iter;
    iter++;
    for (;iter != headers.end(); iter++)
    {
      std::string& header = *iter;
      std::string headerName;
      std::string headerValue;
      if (!headerSplit(header, headerName, headerValue))
      {
        _badHeaders.push_back(header);
        continue;
      }
      std::string rawHeaderName = headerName;
      headerName = hdrGetExpandedForm(headerName);
      boost::to_lower(headerName);
      if (_headers.find(headerName) == _headers.end())
      {
        SIPHeaderTokens tokens;
        tokens.rawHeaderName() = rawHeaderName;
        tokens.headerOffSet() = _headerOffSet++;
        tokens.push_back(headerValue);
        _headers[headerName] = tokens;
      }
      else
      {
        _headers[headerName].push_back(headerValue);
      }
    }
  }
  _finalized = true;
}

bool SIPMessage::headerTokenize(
  SIPHeaderTokens & lines,
  const std::string & theString,
  const char * lineBreakToken)
{
  try
  {
    if (theString.empty())
    {
      return false;
    }

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(lineBreakToken);
    tokenizer tokens(theString, sep);
    std::string previousLine;
    for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
    {
      std::string currentLine = *tok_iter;
      if (currentLine.length() > 0 && ::isspace(currentLine[0]))
      {
        boost::trim(previousLine);
        boost::trim(currentLine);
        currentLine =  previousLine + " " + currentLine;
        lines.pop_back();///erase the previous line so that the slot gets reused by the wrapped header
      }
      lines.push_back( currentLine );
      previousLine = currentLine;
    }
  }
  catch(const std::exception& e)
  {
    OSS_LOG_ERROR("Unknown parser error - " << e.what());
    return false;
  }
  return lines.begin() != lines.end();
}

bool SIPMessage::headerSplit(
    const std::string & header,
    std::string & name,
    std::string & value)
{
  size_t nameBound = header.find_first_of(':');
  if (nameBound == std::string::npos)
  {
    return false;
  }
  name = header.substr(0,nameBound);
  value = header.substr(nameBound+1);
  boost::trim(name);
  boost::trim(value);
  return true;
}

bool SIPMessage::messageSplit(
  std::string & message,
  std::string & headers,
  std::string & body)
{
  if (message.empty())
  {
    return false;
  }

  for (std::string::iterator iter = message.begin(); iter != message.end();)
  {
    if (!isChar(*iter) || *iter == '\r' || *iter == '\n')
      iter = message.erase(iter);
    else
      break;
  }

  int boundaryLen = 4;
  size_t offSet = std::string::npos;
  offSet = message.find(CRLFCRLF);
  if (offSet == std::string::npos)
  {
    offSet = message.find( CRCR );
    boundaryLen = 2;
  }
  if (offSet == std::string::npos)
  {
    offSet = message.find( LFLF );
    boundaryLen = 2;
  }
  if (offSet == std::string::npos)
  {
    headers = message; /// this is a header only SIP Message
    return true;
  }
  headers = message.substr(0,offSet);

  if (message.size() > headers.size() + boundaryLen )
  {
    ///ok we have a body
    body = message.substr( offSet + boundaryLen );
  }
 
  return true;
}

bool SIPMessage::messageSplit(
    std::string & message,
    SIPHeaderTokens& headers,
    std::string & body)
{
  std::string rawHeaders;
  if (!messageSplit(message, rawHeaders, body))
    return false;
  if (!headerTokenize(headers,rawHeaders))
    return false;
  return true;
}



size_t SIPMessage::hdrPresent(const char * headerName) const
{
  ReadLock lock(_rwlock);

  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");

  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
    return 0;
  return const_cast<SIPMessage*>(this)->_headers[key].size();
}

const std::string& SIPMessage::hdrGet(const char * headerName, size_t index) const
{
  ReadLock lock(_rwlock);

  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");

  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
    return _headerEmptyRet;
  if (const_cast<SIPMessage*>(this)->_headers[key].size() == 0)
    return _headerEmptyRet;
  if (index >= const_cast<SIPMessage*>(this)->_headers[key].size())
    throw OSS::SIP::SIPParserException("Invalid Index Exception");
  return const_cast<SIPMessage*>(this)->_headers[key][index];
}

bool SIPMessage::hdrSet(const char * headerName, const std::string& headerValue)
{
  WriteLock lock(_rwlock);

  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");

  if (headerValue.empty())
    throw OSS::SIP::SIPParserException("Header Value is empty while calling SIPMessage::hdrSet()");

  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end() || _headers[key].size() == 0)
  {
    SIPHeaderTokens tokens;
    tokens.push_back(headerValue);
    tokens.rawHeaderName() = headerName;
    tokens.headerOffSet() = _headerOffSet++;
    _headers[key] = tokens;
  }
  else
  {
    _headers[key][0]=headerValue;
  }
  return true;
}

bool SIPMessage::hdrSet(const char* headerName, const std::string& headerValue, size_t index)
{
  WriteLock lock(_rwlock);


  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");

  if (headerValue.empty())
    throw OSS::SIP::SIPParserException("Header Value is empty while calling SIPMessage::hdrSet()");

  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
  {
    SIPHeaderTokens tokens;
    tokens.push_back(headerValue);
    tokens.rawHeaderName() = headerName;
    tokens.headerOffSet() = _headerOffSet++;
    _headers[key] = tokens;
    return true;
  }

  if (_headers[key].size() == 0 || index >= _headers[key].size())
    throw OSS::SIP::SIPParserException("Invalid Index Exception");

  _headers[key][index]=headerValue;
  return true;
}

bool SIPMessage::hdrRemove(const char* headerName)
{
  WriteLock lock(_rwlock);
  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");
  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
    return false;
  if (_headers[key].size() > 1)
    throw OSS::SIP::SIPParserException("hdrRemove - Attempt to remove a header with more than one element!");
  _headers.erase(key);
  return true;
}


bool SIPMessage::hdrListAppend(const char* name, const std::string & value)
{
  WriteLock lock(_rwlock);

  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");

  if (value.empty())
    throw OSS::SIP::SIPParserException("Header Value is empty while calling SIPMessage::hdrListAppend()");

  std::string key = name;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
  {
    SIPHeaderTokens tokens;
    tokens.push_back(value);
    tokens.rawHeaderName() = name;
    tokens.headerOffSet() = _headerOffSet++;
    _headers[key] = tokens;
  }
  else
  {
    _headers[key].push_back(value);
  }
  return true;
}

bool SIPMessage::hdrListPrepend(const char* name, const std::string& value)
{
  WriteLock lock(_rwlock);

  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");

  if (value.empty())
    throw OSS::SIP::SIPParserException("Header Value is empty while calling SIPMessage::hdrListAppend()");

  std::string key = name;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
  {
    SIPHeaderTokens tokens;
    tokens.push_back(value);
    tokens.rawHeaderName() = name;
    tokens.headerOffSet() = _headerOffSet++;
    _headers[key] = tokens;
  }
  else
  {
    _headers[key].push_front(value);
  }
  return true;
}

std::string SIPMessage::hdrListPopFront(const char* headerName)
{
  WriteLock lock(_rwlock);
  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");
  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
    return "";
  SIPHeaderTokens& tokens = _headers[key];
  std::string front;
  if (tokens.empty())
  {
    //
    // This should never happen but handle it just in case
    //
    _headers.erase(key);
    return "";
  }
  else if (tokens.size() == 1)
  {
    SIPHeaderTokens::iterator iter = tokens.begin();
    front = *iter;
    _headers.erase(key);
  }
  else
  {
    SIPHeaderTokens::iterator iter = tokens.begin();
    front = *iter;
    tokens.erase(iter);
  }
  return front;
}

bool SIPMessage::hdrListRemove(const char* headerName)
{
  WriteLock lock(_rwlock);
  if (!_finalized)
    throw OSS::SIP::SIPParserException("Invalid Parser State Exception");
  std::string key = headerName;
  boost::to_lower(key);
  if (_headers.find(key)==_headers.end())
    return false;
  _headers.erase(key);
  return true;
}

bool SIPMessage::commitData()
{
  WriteLock lock(_rwlock);

  std::ostringstream strm;
  strm << _startLine << CRLF;
  SIPHeaderList::iterator iter;
  typedef std::map<size_t,SIPHeaderTokens*> sorted;
  sorted sortedHeaders; 
  for (iter = _headers.begin(); iter != _headers.end(); iter++)
  {
    SIPHeaderTokens & tokens = iter->second;
    sortedHeaders[tokens.headerOffSet()] = &tokens;
  }

  sorted::iterator siter;
  for (siter = sortedHeaders.begin(); siter != sortedHeaders.end(); siter++)
  {
    SIPHeaderTokens * tokens = siter->second;
    SIPHeaderTokens::iterator headerIter;
    for (headerIter = tokens->begin(); headerIter != tokens->end(); headerIter++)
    {
      if (!headerIter->empty()) 
        strm << siter->second->rawHeaderName() << ": " << *headerIter << CRLF;
    }
  }
  strm << CRLF;
  if (!_body.empty())
    strm << _body;
  _data = strm.str();
  return true;
}

bool SIPMessage::getTransactionId(std::string& transactionId, const char* method_) const
{
  
  ReadLock lock(_rwlock);
  std::string viaStr = hdrGet("via");
  std::string callIdStr = hdrGet("call-id");
  std::string cseqStr = hdrGet("cseq");

  if (viaStr.empty() || callIdStr.empty() || cseqStr.empty())
    return false;

  SIPCSeq cseq(cseqStr);
  std::string method;
  if (method_ == 0)
    method = cseq.getMethod();
  else
    method = method_;

  std::string number = cseq.getNumber();

  if (method.empty() || number.empty())
    return false;

  SIPVia via(viaStr);
  std::string id = via.getBranch();
  if( id.empty() && callIdStr.empty())
    return false;
  else if (id.empty())
    id = callIdStr;

  boost::to_lower(method);

  if (method == "ack")
    method = "invite";

  transactionId = method;
  transactionId += number;
  transactionId += id;

  return true;
}

boost::tribool SIPMessage::isRequest(const char* method) const
{
  if (method)
    return OSS::string_caseless_starts_with(_startLine, method);

  if (_isRequest != boost::indeterminate)
    return _isRequest;

  if (_startLine.empty())
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");

  if (requestLineVerify(_startLine.c_str()))
  {
    _isRequest = true;
    return true;
  }

  if (!statusLineVerify(_startLine.c_str()))
    return boost::indeterminate;

  _isRequest = false;
  return false;
}

boost::tribool SIPMessage::isResponseTo(const char* meth) const
{
  if (_startLine.empty())
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");

  if (!_isResponse)
    _isResponse = statusLineVerify(_startLine.c_str());

  if (_isResponse)
  {
    std::string cseq = hdrGet("cseq");
    if (cseq.empty())
      return boost::indeterminate;
    OSS::string_to_lower(cseq);
    std::string method = meth;
    OSS::string_to_lower(method);
    return OSS::string_ends_with(cseq, method.c_str());
  }

  if (!requestLineVerify(_startLine.c_str()))
    return boost::indeterminate;
  _isResponse = false;
  return false;
}

boost::tribool SIPMessage::isResponse() const
{
  if (_isResponse != boost::indeterminate)
    return _isResponse;

  if (_startLine.empty())
    throw OSS::SIP::SIPParserException("ABNF Syntax Exception");

  if (statusLineVerify(_startLine.c_str()))
  {
    _isResponse = true;
    return true;
  }

  if (!requestLineVerify(_startLine.c_str()))
    return boost::indeterminate;

  _isResponse = false;
  return false;
}

boost::tribool SIPMessage::isResponseFamily(int responseCode) const
{
  boost::tribool checkReponse = isResponse();
  if (!checkReponse)
    return checkReponse;

  if (responseCode >= 100 && responseCode <= 199 && _startLine[8] == '1')
    return true;
  else if (responseCode >= 200 && responseCode <= 299 && _startLine[8] == '2')
    return true;
  else if (responseCode >= 300 && responseCode <= 399 && _startLine[8] == '3')
    return true;
  else if (responseCode >= 400 && responseCode <= 499 && _startLine[8] == '4')
    return true;
  else if (responseCode >= 500 && responseCode <= 599 && _startLine[8] == '5')
    return true;
  else if (responseCode >= 600 && responseCode <= 699 && _startLine[8] == '6')
    return true;

  return false;
}

boost::tribool SIPMessage::isErrorResponse() const
{
  boost::tribool checkReponse = isResponse();
  if (!checkReponse)
    return checkReponse;

  if (_startLine[8] == '3')
    return true;
  else if (_startLine[8] == '4')
    return true;
  else if (_startLine[8] == '5')
    return true;
  else if (_startLine[8] == '6')
    return true;
  return false;
}

std::string SIPMessage::getMethod() const
{
  SIPCSeq cseq;
  cseq = hdrGet("cseq");
  return cseq.getMethod();
}

boost::tribool SIPMessage::isMidDialog() const
{
  SIPFrom from;
  SIPTo to;
  from = hdrGet("from");
  to = hdrGet("to");

  std::string fromTag;
  std::string toTag;
  fromTag = from.getHeaderParam("tag");
  toTag = to.getHeaderParam("tag");

  if (!fromTag.empty() && !toTag.empty())
    return true;
  else if (!fromTag.empty() && toTag.empty())
    return false;
  
  return boost::indeterminate;
}

std::string SIPMessage::getDialogId(bool asSender) const
{
  SIPFrom from;
  SIPTo to;
  from = hdrGet("from");
  to = hdrGet("to");

  std::string fromTag;
  std::string toTag;
  fromTag = from.getHeaderParam("tag");
  toTag = to.getHeaderParam("tag");

  if (fromTag.empty() || toTag.empty())
    return std::string();

  std::stringstream strm;

  if (asSender)
  {
    if (isResponse())
      strm << toTag << fromTag;
    else
      strm << fromTag << toTag;
  }
  else
  {
    if (isRequest())
      strm << toTag << fromTag;
    else
      strm << fromTag << toTag;
  }

  return strm.str();
}

bool SIPMessage::read(std::istream& strm, std::size_t& totalRead)
{
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

  totalRead = 0;
  while(true)
  {
    boost::tuple<boost::tribool, const char*> result;
    char buff[1024];

    //
    // Remember the position of our original read location
    // so we have a means to rewind back
    //
    std::streampos originalPosition = strm.tellg();

    std::size_t readBytes = strm.readsome(buff, 1024);
    totalRead += readBytes;

    if (readBytes == 0)
    {
      //
      // SIP Message is still incomplete.  Returning indeterminate here would
      // tell the upper layer that further bytes are needed to complete the
      // read operation
      //
      return false;
    }

    result = consume(buff, buff + readBytes);
    if (result.get<0>())
    {
      //
      // We have the complete message.
      // Check if there are extra bytes
      // that was processed by the buffer
      // and return the read pointer to that
      // position
      //
      if (result.get<1>() < buff + readBytes)
      {
        //
        // Do some pointer arithmetics and reposition the read pointer
        //
        std::streampos offset = result.get<1>() - buff;
        totalRead -= offset;
        offset += originalPosition;
        strm.seekg(offset);
      }
      return true;
    }
    else if (!result.get<0>())
    {
      totalRead = 0;  /// this will indicate an error happend in parsing
      break;
    }
  }

  return false;
}

boost::tuple<boost::tribool, const char*> SIPMessage::consume(const char* begin, const char* end)
{
  _finalized = false;
  while (begin != end)
  {
    boost::tribool result = consumeOne(*begin++);
    if (result || !result)
      return boost::make_tuple(result, begin);
  }
  boost::tribool result = boost::indeterminate;
  return boost::make_tuple(result, begin);
}

boost::tribool SIPMessage::consumeOne(char input)
{
  if (_consumeState == IDLE)
  {
    //
    // ignore new lines or non-chars
    //
    if (!isChar(input) || input == '\r' || input == '\n')
    {
      _idleBuffer.push_back(input);
      return boost::indeterminate;
    }

    _data = "";
    _consumeState = START_LINE_PARSE;
  }
  
  switch (_consumeState)
  {
  case START_LINE_PARSE:
    
    if (input == '\r')
    {
      _consumeState = EXPECTING_NEW_LINE_1;
    }
    else if (!isChar(input) || isCtl(input))
    {
      return false;
    }
    _data.push_back(input);
    return boost::indeterminate;
  case EXPECTING_NEW_LINE_1:
    if (input == '\n')
    {
      _consumeState = HEADER_LINE_START;
      _data.push_back(input);
      return boost::indeterminate;
    }
    else
    {
      return false;
    }
  case HEADER_LINE_START:
    if (input == '\r')
    {
      _consumeState  = EXPECTING_NEW_LINE_3;
    }
    else if (input == ' ' || input == '\t')
    {
      _consumeState = HEADER_LWS;
    }
    else if (!isChar(input) || isCtl(input))
    {
      return false;
    }
    else
    {
      _consumeState = HEADER_NAME;
    }
    _data.push_back(input);
    return boost::indeterminate;
  case HEADER_LWS:
    if (input == '\r')
    {
      _consumeState = EXPECTING_NEW_LINE_2;
    }
    else if (input == ' ' || input == '\t')
    {
      _consumeState = HEADER_LWS;
    }
    else if (isCtl(input))
    {
      return false;
    }
    else
    {
      _consumeState = HEADER_VALUE;
    }
    _data.push_back(input);
    return boost::indeterminate;
  case HEADER_NAME:
    if (input == ':')
    {
      _consumeState = HEADER_VALUE;
    }
    else if (!isChar(input) || isCtl(input))
    {
      return false;
    }
    _data.push_back(input);
    return boost::indeterminate;
  case HEADER_VALUE:
    if (input == '\r')
    {
      _consumeState = EXPECTING_NEW_LINE_2;
    }
    else if (!isChar(input) || isCtl(input))
    {
      return false;
    }
    _data.push_back(input);
    return boost::indeterminate;
  case EXPECTING_NEW_LINE_2:
    if (input == '\n')
    {
      _consumeState = HEADER_LINE_START;
    }
    else
    {
      return false;
    }
    _data.push_back(input);
    return boost::indeterminate;
  case EXPECTING_NEW_LINE_3:
    _data.push_back(input);
    if (input == '\n')
    {
      parse();
      std::string clen = this->hdrGet("content-length");
      if (!clen.empty())
      {
        _expectedBodyLen = ::atoi(clen.c_str());
        if (_expectedBodyLen > 0)
        {
          _consumeState = EXPECTING_BODY;
          _finalized = false;
          return boost::indeterminate;
        }
        return true;
      }
      else
      {
        return true;
      }
    }
    else
    {
      return false;
    }
  case EXPECTING_BODY:
    _body.push_back(input);
    if (_body.size() < _expectedBodyLen)
    {
      return boost::indeterminate;
    }
    else
    {
      _finalized = commitData();
      return _finalized;
    }
  default:
    return false;
  }
}

SIPMessage::Ptr SIPMessage::reformatResponse(const SIPMessage::Ptr& pResponse)
{
  ReadLock lock(_rwlock);

  if (!isRequest())
    throw OSS::SIP::SIPParserException("Calling createResponse() for a response is illegal!!");

  SIPMessage::Ptr pFormatedResponse = SIPMessage::Ptr(new SIPMessage(*(pResponse.get())));

  std::string to = hdrGet("to");
  if (to.empty())
    throw OSS::SIP::SIPParserException("Invalid To header.");

  std::string rTo = pFormatedResponse->hdrGet("to");
  SIPTo hRTo(rTo);
  std::string toTag = hRTo.getHeaderParam("tag");

  SIPTo hTo(to);
  std::string tag = hTo.getHeaderParam("tag");

  //
  // Construct the response
  //

  //
  // Set the headers
  //

  //
  // From
  //
  std::string from = hdrGet("from");
  if (from.empty())
    throw OSS::SIP::SIPParserException("Invalid From header");
  pFormatedResponse->hdrSet("From", from);

  //
  // To
  //
  if (!toTag.empty())
    hTo.setHeaderParam("tag", toTag.c_str());
  pFormatedResponse->hdrSet("To", hTo.data());

  //
  // Call-ID
  //
  std::string callId = hdrGet("call-id");
  if (callId.empty())
    throw OSS::SIP::SIPParserException("Invalid CALL-ID header");
  pFormatedResponse->hdrSet("Call-ID", callId);

  //
  // CSeq
  //
  std::string cseq = hdrGet("cseq");
  if (cseq.empty())
    throw OSS::SIP::SIPParserException("Invalid CSeq header");
  pFormatedResponse->hdrSet("CSeq", cseq);

    
  //
  // Via
  //
  pFormatedResponse->_headers.erase("via");
  pFormatedResponse->_headers.erase("Via");

  size_t viaCount = hdrGetSize("via");
  if (!viaCount)
    throw OSS::SIP::SIPParserException("Invalid Via header");
  for (size_t i = 0; i < viaCount; i++)
  {
    std::string via = hdrGet("via", i);
    if (via.empty())
      throw OSS::SIP::SIPParserException("Invalid via header");
    pFormatedResponse->hdrListAppend("Via", via);
  }

  //
  // Record-Route
  //
  pFormatedResponse->_headers.erase("record-route");
  pFormatedResponse->_headers.erase("Record-Route");

  size_t routeCount = hdrGetSize("record-route");
  for (size_t i = 0; i < routeCount; i++)
  {
    std::string route = hdrGet("record-route", i);
    if (route.empty())
      throw OSS::SIP::SIPParserException("Invalid Record-Route header");
    pFormatedResponse->hdrListAppend("Record-Route", route);
  }

  pFormatedResponse->commitData();

  return pFormatedResponse;
}

SIPMessage::Ptr SIPMessage::createResponse(
  int statusCode,
  const std::string& reasonPhrase, 
  const std::string& toTag,
  const std::string& contact)
{
  ReadLock lock(_rwlock);

  if (!isRequest())
    throw OSS::SIP::SIPParserException("Calling createResponse() for a response is illegal!!");

  SIPMessage::Ptr response = SIPMessage::Ptr(new SIPMessage());
  std::string to = hdrGet("to");
  if (to.empty())
    throw OSS::SIP::SIPParserException("Invalid To header.");

  SIPTo hTo(to);
  std::string tag = hTo.getHeaderParam("tag");
  if (!tag.empty() && !toTag.empty())
  {
    if (tag != toTag)
      throw OSS::SIP::SIPParserException("To tag is already present in current request.");
  }

  //
  // Construct the response
  //

  //
  // Status Line
  //
  std::stringstream startLine;
  std::string phrase = reasonPhrase;
  if (phrase.empty())
    SIPParser::getReasonPhrase(statusCode, phrase);
  startLine << "SIP/2.0 " << statusCode << " " << phrase;
  response->_startLine = startLine.str();
 
  //
  // Set the headers
  //

  //
  // From
  //
  std::string from = hdrGet("from");
  if (from.empty())
    throw OSS::SIP::SIPParserException("Invalid From header");
  response->hdrSet("From", from);

  //
  // To
  //
  if (!toTag.empty())
    hTo.setHeaderParam("tag", toTag.c_str());
  response->hdrSet("To", hTo.data());

  //
  // Call-ID
  //
  std::string callId = hdrGet("call-id");
  if (callId.empty())
    throw OSS::SIP::SIPParserException("Invalid CALL-ID header");
  response->hdrSet("Call-ID", callId);

  //
  // CSeq
  //
  std::string cseq = hdrGet("cseq");
  if (cseq.empty())
    throw OSS::SIP::SIPParserException("Invalid CSeq header");
  response->hdrSet("CSeq", cseq);

  //
  // Contact
  //
  if (!contact.empty())
    response->hdrListAppend("Contact", contact);
    
  //
  // Via
  //
  size_t viaCount = hdrGetSize("via");
  if (!viaCount)
    throw OSS::SIP::SIPParserException("Invalid Via header");
  for (size_t i = 0; i < viaCount; i++)
  {
    std::string via = hdrGet("via", i);
    if (via.empty())
      throw OSS::SIP::SIPParserException("Invalid via header");
    response->hdrListAppend("Via", via);
  }

  //
  // Record-Route
  //
  size_t routeCount = hdrGetSize("record-route");
  for (size_t i = 0; i < routeCount; i++)
  {
    std::string route = hdrGet("record-route", i);
    if (route.empty())
      throw OSS::SIP::SIPParserException("Invalid Record-Route header");
    response->hdrListAppend("Record-Route", route);
  }

  //
  // Content-Length
  //
  response->hdrSet("Content-Length", "0");

  response->commitData();

  return response;
}

void SIPMessage::setData(const std::string& data)
{
  WriteLock lock(_rwlock);
  _finalized = false;
  _data = data;
}

const std::string& SIPMessage::getBody() const
{
  ReadLock lock(_rwlock);
  return _body;
}

void SIPMessage::setBody(const std::string& body)
{
  WriteLock lock(_rwlock);
  _body = body;
}

void SIPMessage::updateLength()
{
  std::string newLen = OSS::string_from_number<size_t>(_body.length());
  hdrSet("Content-Length", newLen);
}

const std::string& SIPMessage::getStartLine() const
{
  ReadLock lock(_rwlock);
  return _startLine;
}

void SIPMessage::setStartLine(const std::string& startLine)
{
  WriteLock lock(_rwlock);
  _startLine = startLine;
}

void SIPMessage::setProperty(const std::string& property, const std::string& value)
{
  WriteLock lock(_rwlock);
  _properties[property] = value;
}

void SIPMessage::clearProperties()
{
  WriteLock lock(_rwlock);
  _properties.clear();
}

bool SIPMessage::getProperty(const std::string&  property, std::string& value) const
{
  ReadLock lock(_rwlock);
  CustomProperties::const_iterator iter = _properties.find(property);
  if (iter != _properties.end())
  {
    value = iter->second;
    return true;
  }
  return false;
}

boost::tribool SIPMessage::is1xx(int code) const
{
  if (!code)
    return isResponseFamily(100);

  std::ostringstream scode;
  scode << "SIP/2.0 " << code;
  return (OSS::string_starts_with(_startLine, scode.str().c_str()));
}

boost::tribool SIPMessage::is2xx(int code) const
{
  if (!code)
    return isResponseFamily(200);

  std::ostringstream scode;
  scode << "SIP/2.0 " << code;
  return (OSS::string_starts_with(_startLine, scode.str().c_str()));
}

boost::tribool SIPMessage::is3xx(int code) const
{
  if (!code)
    return isResponseFamily(300);
  std::ostringstream scode;
  scode << "SIP/2.0 " << code;
  return (OSS::string_starts_with(_startLine, scode.str().c_str()));
}

boost::tribool SIPMessage::is4xx(int code) const
{
  if (!code)
    return isResponseFamily(400);
  std::ostringstream scode;
  scode << "SIP/2.0 " << code;
  return (OSS::string_starts_with(_startLine, scode.str().c_str()));
}

boost::tribool SIPMessage::is5xx(int code) const
{
  if (!code)
    return isResponseFamily(500);
  std::ostringstream scode;
  scode << "SIP/2.0 " << code;
  return (OSS::string_starts_with(_startLine, scode.str().c_str()));
}

boost::tribool SIPMessage::is6xx(int code) const
{
  if (!code)
    return isResponseFamily(600);
  std::ostringstream scode;
  scode << "SIP/2.0 " << code;
  return (OSS::string_starts_with(_startLine, scode.str().c_str()));
}

std::string SIPMessage::createContextId(SIPMessage* pMsg, bool formatTabAndSpaces)
{
  std::string id = pMsg->hdrGet("call-id");
  unsigned int hash = OSS::string_to_js_hash(id);
  std::ostringstream newId;
  if (formatTabAndSpaces)
    newId << "\t";
  newId << "[CID=" << std::setfill('0') << std::hex << std::setw(8)
       << hash
       << std::setfill(' ') << std::dec << "]";
  if (formatTabAndSpaces)
    newId << " ";
  return newId.str();
}

std::string SIPMessage::createLoggerData(SIPMessage* pMsg)
{
  std::string cid = "\t\t";
  cid += createContextId(pMsg);
  cid += " ";

  WriteLock lock(pMsg->_rwlock);
  std::ostringstream strm;
  strm << CRLF << "{" << CRLF << cid << pMsg->_startLine;
  SIPHeaderList::iterator iter;
  typedef std::map<size_t,SIPHeaderTokens*> sorted;
  sorted sortedHeaders;
  for (iter = pMsg->_headers.begin(); iter != pMsg->_headers.end(); iter++)
  {
    SIPHeaderTokens & tokens = iter->second;
    sortedHeaders[tokens.headerOffSet()] = &tokens;
  }

  sorted::iterator siter;
  for (siter = sortedHeaders.begin(); siter != sortedHeaders.end(); siter++)
  {
    SIPHeaderTokens * tokens = siter->second;
    SIPHeaderTokens::iterator headerIter;
    for (headerIter = tokens->begin(); headerIter != tokens->end(); headerIter++)
    {
      if (!headerIter->empty())
        strm  << CRLF << cid << siter->second->rawHeaderName() << ": " << *headerIter;
    }
  }
  
  if (!pMsg->_body.empty())
  {
    strm << CRLF << cid;
    std::vector<std::string> tokens = OSS::string_tokenize(pMsg->_body, CRLF);
    for (std::vector<std::string>::const_iterator iter = tokens.begin(); iter != tokens.end(); iter++)
      strm  << CRLF << cid << *iter;
  }
  strm << CRLF << "};";
  return strm.str();
}

std::string SIPMessage::getFromTag() const
{
  std::string tag;
  std::string from(hdrGet("from"));
  if (!from.empty())
  {
    tag = SIPFrom::getTag(from);
  }
  return tag;
}

std::string SIPMessage::getFromHost() const
{
  std::string from(hdrGet("from"));
  std::string host;
  SIPFrom::getHost(from, host);
  return host;
}

std::string SIPMessage::getFromHostPort() const
{
  std::string from(hdrGet("from"));
  std::string host;
  SIPFrom::getHostPort(from, host);
  return host;
}

std::string SIPMessage::getToHost() const
{
  std::string to(hdrGet("to"));
  std::string host;
  SIPFrom::getHost(to, host);
  return host;
}

std::string SIPMessage::getToHostPort() const
{
  std::string to(hdrGet("to"));
  std::string host;
  SIPFrom::getHostPort(to, host);
  return host;
}

std::string SIPMessage::getToTag() const
{
  std::string tag;
  std::string to(hdrGet("to"));
  if (!to.empty())
  {
    tag = SIPFrom::getTag(to);
  }
  return tag;
}

std::string SIPMessage::getTopViaBranch() const
{
  std::string branch;
  std::string via(hdrGet("via"));
  if (!via.empty())
  {
    SIPVia::getBranch(via, branch);
  }
  return branch;
}


}} //OSS::SIP

