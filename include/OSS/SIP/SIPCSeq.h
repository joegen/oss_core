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


#ifndef SIP_SIPCSeq_INCLUDED
#define SIP_SIPCSeq_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"


namespace OSS {
namespace SIP {


class OSS_API SIPCSeq: public SIPParser
  /// CSeq header lazy parser.
{
public:
  SIPCSeq();
    /// Create a new SIPCSeq vector

  SIPCSeq(const std::string& cseq);
    /// Create a cseq from a string.
    ///
    /// This constructor expects that the cseq header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the cseq variable parameter
    /// is the body of the cseq header (not including Via:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("cseq");
    ///   SIPCSeq cseq(hdr);

  SIPCSeq(const SIPCSeq& cseq);
    /// Create a new SIPCSeq from another SIPCSeq object

  ~SIPCSeq();
    /// Destroy the SIPCSeq object

  SIPCSeq& operator = (const std::string& cseq);
    /// Copy the content from another a cseq string.

  SIPCSeq& operator = (const SIPCSeq& cseq);
    /// Copy the content from another SIPCSeq

  void swap(SIPCSeq& cseq);
    /// Exchanges the data between two SIPCSeq

  std::string getMethod() const;
    /// Returns the value of the CSeq Method

  static bool getMethod(const std::string& cseq, std::string& method);
    /// Returns the value of the CSeq Method

  bool setMethod(const char* method);
    /// Sets the value of the CSeq Method

  static bool setMethod(std::string& cseq, const char* method);
    /// Sets the value of the CSeq Method

  std::string getNumber() const;
    /// Returns the value of the CSeq Number

  static bool getNumber(const std::string& cseq, std::string& number);
    /// Returns the value of the CSeq Number

  bool setNumber(const char* number);
    /// Sets the value of the CSeq Number

  static bool setNumber(std::string& cseq, const char* number);
    /// Sets the value of the CSeq Number

};


} } // OSS::SIP
#endif // SIP_SIPCSeq_INCLUDED


