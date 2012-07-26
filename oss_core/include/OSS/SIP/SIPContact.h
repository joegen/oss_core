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


#ifndef SIP_SIPContact_INCLUDED
#define SIP_SIPContact_INCLUDED

#include <vector>

#include "OSS/SIP/Parser.h"
#include "OSS/SIP/SIPParser.h"
#include "OSS/SIP/SIPFrom.h"


namespace OSS {
namespace SIP {

class SIPMessage;

typedef SIPFrom ContactURI;

class OSS_API SIPContact: public SIPParser
  /// CSeq header lazy parser.
{
public:
  SIPContact();
    /// Create a new SIPContact vector

  SIPContact(const std::string& contact);
    /// Create a contact from a string.
    ///
    /// This constructor expects that the contact header
    /// has already been split using SIPMessage::headerSplit()
    /// function and that the value of the contact variable parameter
    /// is the body of the contact header (not including Contact:).
    ///
    /// Usage:
    ///
    ///   std::string hdr = msg.hdrGet("contact");
    ///   SIPContact contact(hdr);

  SIPContact(const SIPContact& contact);
    /// Create a new SIPContact from another SIPContact object

  ~SIPContact();
    /// Destroy the SIPContact object

  SIPContact& operator = (const std::string& contact);
    /// Copy the content from another a contact string.

  SIPContact& operator = (const SIPContact& contact);
    /// Copy the content from another SIPContact

  void swap(SIPContact& contact);
    /// Exchanges the data between two SIPContact

  bool getAt(ContactURI& uri, size_t index) const;
    /// Gets the contact URI at the particular index.

  static bool getAt(const std::string& contact, ContactURI& uri, size_t index);
    /// Gets the contact URI at the particular index.

  bool setAt(const char* uri, size_t index);
    /// Sets the value of the contact URI at the particular index

  static bool setAt(std::string& contact, const char* uri, size_t index);
    /// Sets the value of the contact URI at the particular index

  size_t getSize() const;
    /// Returns the number of contact URI's present in the header

  static size_t getSize(const std::string& contact);
    /// Returns the number of contact URI's present in the header

  bool popTopURI(ContactURI& topURI);
    /// Pop the top most URI

  static bool popTopURI(std::string& contact, ContactURI& topURI);
    /// Pop the top most URI from a contact header string


  static int msgGetContacts(SIPMessage* pMsg, std::vector<std::string>& contacts);
    /// Get all the contacts as a vector
    /// Returns the number of contact retrieved
};


} } // OSS::SIP
#endif // SIP_SIPContact_INCLUDED


