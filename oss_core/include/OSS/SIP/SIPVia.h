// Library: OSS Software Solutions Application Programmer Interface
// Package: OSSSIP
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Copyright (c) OSS Software Solutions
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


#ifndef SIP_SIPVia_INCLUDED
#define SIP_SIPVia_INCLUDED


#include "OSS/Core.h"
#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"


namespace OSS {
namespace SIP {

class SIPMessage;

class OSS_API SIPVia: public SIPParser
  /// Via header lazy parser.
{
public:
  SIPVia();
    /// Create a new SIPVia vector

  SIPVia(const std::string& via);
    /// Create a via from a string.
    ///
    /// This constructor expects that the via header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the via variable parameter
    /// is the body of the via header (not including Via:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("via");
    ///   SIPVia via(hdr);

  SIPVia(const SIPVia& via);
    /// Create a new SIPVia from another SIPVia object

  ~SIPVia();
    /// Destroy the SIPVia object

  SIPVia& operator = (const std::string& via);
    /// Copy the content from another a via string.

  SIPVia& operator = (const SIPVia& via);
    /// Copy the content from another SIPVia

  void swap(SIPVia& via);
    /// Exchanges the data between two SIPVia

  std::string getSentBy() const;
    /// Returns the value of the via sent-by (host [ COLON port ])
    /// if present.

  static bool getSentBy(const std::string& via, std::string& sentBy);
    /// Returns the value of the via sent-by (host [ COLON port ])
    /// if present.

  std::string getTransport() const;
    /// Returns the value of the via transport ("UDP" / "TCP" / "TLS" / "SCTP")
    /// if present.

  static bool getTransport(const std::string& via, std::string& transport);
    /// Returns the value of the via transport ("UDP" / "TCP" / "TLS" / "SCTP")
    /// if present.

  void setTransport(const std::string& transport);
    /// change the transport

  static bool setTransport(std::string& via, const std::string& transport);
    //change the transport

  std::string getBranch() const;
    /// Returns the value of the via branch parameter
    /// if present.

  static bool getBranch(const std::string& via, std::string& paramValue);
    /// Returns the value of the via branch parameter
    /// if present.

  std::string getRPort() const;
    /// Returns the value of the via rport parameter
    /// if present.

  static bool getRPort(const std::string& via, std::string& paramValue);
    /// Returns the value of the via rport parameter
    /// if present.

  std::string getReceived() const;
    /// Returns the value of the via received parameter
    /// if present.

  static bool getReceived(const std::string& via, std::string& paramValue);
    /// Returns the value of the via received parameter
    /// if present.

  std::string getParam(const char* paramName) const;
    /// Returns the value of any via parameter
    /// if present.

  static bool getParam(const std::string& via, const char* paramName, std::string& paramValue);
    /// Returns the value of any via parameter
    /// if present.

  bool hasParam(const char* paramName) const;
    /// Returns true if the via parameter
    /// if present.

  static bool hasParam(const std::string& via, const char* paramName);
    /// Returns true if the via parameter
    /// if present.

  bool setSentBy(const char* hostPort);
    /// Set the sent-by of the via in the form host ":" port

  static bool setSentBy(const std::string& via, const char* hostPort);
    /// Set the sent-by of the via in the form host ":" port


  bool setParam(const char* paramName, const char* paramValue);
    /// Set the value of a via parameter.
    ///
    /// If the via parameter already exists in the parameter list,
    /// its value will be repalced with the new paramValue.
    /// If the parameter does not exist, it will be created.
    ///
    /// This function will throw SIPABNFSyntaxException() if
    /// paramName or paramValue is not a valid token based on 
    /// RFC 3261 ABNF rules for pname and pvalue respectively. 

  static bool setParam(std::string& via, const char* paramName, const char* paramValue);
    /// Set the value of a via parameter.
    ///
    /// If the via parameter already exists in the parameter list,
    /// its value will be repalced with the new paramValue.
    /// If the parameter does not exist, it will be created.
    ///
    /// This function will throw SIPABNFSyntaxException() if
    /// paramName or paramValue is not a valid token based on 
    /// RFC 3261 ABNF rules for pname and pvalue respectively. 

  static int countElements(const std::string& via);
    /// This function will return the number of via elements
    /// contained in a single via header.  

  static int splitElements(const std::string& via, std::vector<std::string>& elements);
    /// A single via header may contain multiple elements.
    /// This function is used to split each element into a vector of
    /// strings.  This function returns the number of elements found.

  static bool getTopVia(const std::string& hVia, std::string& topVia);
    /// Get the top most via from a raw via header

  static bool getBottomVia(const std::string& hVia, std::string& bottomVia);
    /// Get the bottom most via from a raw via header

  static bool msgGetTopVia(SIPMessage* pMsg, std::string& topVia);
    /// Get the top most via of of a SIPMessage

  static std::string popTopVia(const std::string& hVia, std::string& topVia);
    /// Get the top most via and return the remaining via elements (if any) in a c_str.
    /// If there are no more via to be parsed, this function will return an empty string.
    /// Take note that there can be multiple via headers in a SIPMessage.  This
    /// function only pops a via header from a list of elements belonging to a single
    /// header.  (also see msgPopTopVia() function)

  static bool msgPopTopVia(SIPMessage* pMsg, std::string& topVia);
    /// Pop the top most via of a SIPMessage.

  static bool msgAddVia(SIPMessage* pMsg, const std::string& via);
    /// Add a new via to the SIPMessage

  static bool msgClearVias(SIPMessage* pMsg);
    /// Remove all the via headers present in a SIPMessage

  static bool msgGetTopViaSentBy(SIPMessage* pMsg, std::string& sentBy);
    /// Get the top most via of of a SIPMessage

  static bool msgGetTopViaTransport(SIPMessage* pMsg, std::string& transport);
    /// Get the top most via of of a SIPMessage

  static bool msgGetBottomVia(SIPMessage* pMsg, std::string& via);
    /// Get the bottom most via

  static bool msgGetBottomViaSentBy(SIPMessage* pMsg, std::string& sentBy);
    /// Get the sent-by of bottom most via of of a SIPMessage

  static bool msgGetBottomViaReceived(SIPMessage* pMsg, std::string& received);
    /// Get the received parameter of bottom most via of of a SIPMessage

  static bool msgGetBottomViaRPort(SIPMessage* pMsg, std::string& rport);
    /// Get the rport parameter of bottom most via of of a SIPMessage

};

//
// Inlines
//


} } // OSS::SIP
#endif // SIP_SIPVia_INCLUDED


