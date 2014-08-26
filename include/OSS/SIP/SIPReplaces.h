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

#ifndef SIPREPLACES_H_INCLUDED
#define	SIPREPLACES_H_INCLUDED


#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"


namespace OSS {
namespace SIP {

class OSS_API SIPReplaces: public SIPParser
  /// CSeq header lazy parser.
{
public:
  SIPReplaces();
    /// Create a new SIPReplaces header

  SIPReplaces(const std::string& replaces);
    /// Create a replaces replaces a string.
    ///
    /// This constructor expects that the replaces header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the replaces variable parameter
    /// is the body of the replaces header (not including From:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet(OSS::SIP::HDR_REPLACES);
    ///   SIPReplaces replaces(hdr);

  SIPReplaces(const SIPReplaces& replaces);
    /// Create a new SIPReplaces replaces another SIPReplaces object

  ~SIPReplaces();
    /// Destroy the SIPReplaces object

  SIPReplaces& operator = (const std::string& replaces);
    /// Copy the content replaces another a replaces string.

  SIPReplaces& operator = (const SIPReplaces& replaces);
    /// Copy the content replaces another SIPReplaces

  void swap(SIPReplaces& replaces);
    /// Exchanges the data between two SIPReplaces
  
  std::string getCallId() const;
    /// Returns the call-id parameter of the replaces header
  
  bool setCallId(const std::string& callId);
    /// Set the call-id parameter of a replaces header
  
  std::string getFromTag() const;
    /// Returns the from-tag parameter of the replaces header
  
  bool setFromTag(const std::string& tag);
    /// Set the from-tag parameter of a replaces header
  
  std::string getToTag() const;
    /// Returns the to-tag parameter of the replaces header
  
  bool setToTag(const std::string& tag);
    /// Set the to-tag parameter of a replaces header
  
  bool setEarlyFlag(bool isEarly);
    /// Set the "early-only" parameter
  
  bool isEarlyFlagSet();
    /// Returns true if early-flag is set
  
  static std::string getCallId(const std::string& replaces) ;
    /// Returns the call-id parameter of the replaces header
  
  static bool setCallId(std::string& replaces, const std::string& callId);
    /// Set the call-id parameter of a replaces header
  
  static std::string getFromTag(const std::string& replaces) ;
    /// Returns the from-tag parameter of the replaces header
  
  static bool setFromTag(std::string& replaces, const std::string& tag);
    /// Set the from-tag parameter of a replaces header
  
  static std::string getToTag(const std::string& replaces) ;
    /// Returns the to-tag parameter of the replaces header
  
  static bool setToTag(std::string& replaces, const std::string& tag);
    /// Set the to-tag parameter of a replaces header
  
 
  static bool setEarlyFlag(std::string& replaces, bool isEarly);
    /// Set the "early-only" parameter
  
  static bool isEarlyFlagSet(const std::string& replaces);
    /// Returns true if early-flag is set

};

} } // OSS::SIP


#endif	// SIPREPLACES_H_INCLUDED

