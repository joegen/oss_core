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


#include "OSS/SDP/SDPHeaderList.h"


namespace OSS {
namespace SDP {

static bool is_char(int c)
{
  return c >= 0 && c <= 127;
}

SDPHeaderList::SDPHeaderList() :
  std::list<SDPHeader>(),
    _parserState(STATE_EXPECTING_NAME),
    _isValid(false),
    _exitName('\0')
{
}


SDPHeaderList::SDPHeaderList(const char* rawHeader, char exitName) :
   std::list<SDPHeader>(),
   _parserState(STATE_EXPECTING_NAME),
   _isValid(false),
   _exitName(exitName)
{
  parse(rawHeader);
}

SDPHeaderList::SDPHeaderList(const SDPHeaderList& header) :
   std::list<SDPHeader>(header),
   _parserState(header._parserState),
   _isValid(header._isValid),
   _exitName(header._exitName),
   _tail(header._tail)
{
}

SDPHeaderList::~SDPHeaderList()
{
}

void SDPHeaderList::swap(SDPHeaderList& header)
{
  std::swap(dynamic_cast<std::list<SDPHeader>&>(*this), dynamic_cast<std::list<SDPHeader>&>(header));
  std::swap(_parserState, header._parserState);
  std::swap(_exitName, header._exitName);
  std::swap(_tail, header._tail);
  std::swap(_isValid, header._isValid);
}

SDPHeaderList& SDPHeaderList::operator = (const SDPHeaderList& header)
{
  SDPHeaderList swapable(header);
  swap(swapable);
  return *this;
}

SDPHeaderList& SDPHeaderList::operator = (const char* rawHeader)
{
  SDPHeaderList swapable(rawHeader);
  swap(swapable);
  return *this;
}

void SDPHeaderList::parse(const char* rawHeader)
{

  _parserState = STATE_EXPECTING_NAME;
  _isValid = false;

  char* parserData = const_cast<char*>(rawHeader);
  char c = *parserData;
  char name = 0;
  std::string value;
  value.reserve(100);

  while (c != '\0')
  {
    switch (_parserState)
    {
    case STATE_EXPECTING_NAME:
      if (is_char(c))
        name = c;
      else
        return;
      _parserState = STATE_EXPECTING_EQUAL;
      break;
    case STATE_EXPECTING_EQUAL:
      if (c == '=')
        _parserState = STATE_EXPECTING_BODY;
      else
        return;
      break;
    case STATE_EXPECTING_BODY:
      if (c == '\r' || c == '\n')
        _parserState = STATE_EXPECTING_LINE_END_OR_NAME;
      else
        value.push_back(c);
      break;
    case STATE_EXPECTING_LINE_END_OR_NAME:
      if (c != '\r' && c != '\n')
      {
        //
        // We are at the start of the next header at this point
        // store the accumulated value and reset
        //
        if (is_char(c))
        {
          push_back(SDPHeader(name, value.c_str()));
          name = c;
          value = "";
          _parserState = STATE_EXPECTING_EQUAL;
          if (c == _exitName)
          {
            _isValid = true;
            _tail = std::string(parserData);
            return;
          }
        }
      }
      break;
    }
    c = *(++parserData);
  }

  //
  // Push back the last item
  //
  if (name != '\0' && !value.empty())
    push_back(SDPHeader(name, value.c_str()));

  _isValid = true;
}


} }// OSS::SDP
