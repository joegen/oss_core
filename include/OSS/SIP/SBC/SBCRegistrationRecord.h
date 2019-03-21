// OSS Software Solutions Application Programmer Interface
// Package: SBC
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Package: SBC
//
// Copyright (c) OSS Software Solutions
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "OSS Software Solutions OSS API General License Agreement".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef OSS_REGISTRATIONRECORD_H_INCLUDED
#define	OSS_REGISTRATIONRECORD_H_INCLUDED

#include <boost/filesystem.hpp>
#include "OSS/UTL/CoreUtils.h"
#include "OSS/SIP/SBC/SBCWorkSpace.h"


namespace OSS {
namespace SIP {
namespace SBC {

class SBCRegistrationRecord
{
public: 
  SBCRegistrationRecord();
  SBCRegistrationRecord(const SBCRegistrationRecord& record);
  ~SBCRegistrationRecord();
  void swap(SBCRegistrationRecord& record);
  SBCRegistrationRecord& operator=(const SBCRegistrationRecord& record);
  std::string& contact();
  std::string& packetSource();
  std::string& localInterface();
  std::string& transportId();
  std::string& targetTransport();
  std::string& aor();
  int &expires();
  bool &enc();
  std::string& callId();
  OSS::UInt64& timeStamp();
  
  const std::string& contact() const;
  const std::string& packetSource() const;
  const std::string& localInterface() const;
  const std::string& transportId() const;
  const std::string& targetTransport() const;
  const std::string& aor() const;
  const int& expires() const;
  const bool& enc() const;
  const std::string& callId() const;
  const OSS::UInt64& timeStamp() const;

  bool writeToWorkSpace(SBCWorkSpace& client, const std::string& key) const;
  bool writeToFile(const boost::filesystem::path& file) const;
  bool writeToFile(const std::string& file) const;
  bool readFromWorkSpace(SBCWorkSpace& client, const std::string& key);
  bool readFromFile(const boost::filesystem::path& file);
  bool readFromFile(const std::string& file);

  void eraseFile();
  static void eraseFile(const boost::filesystem::path& file);
  void eraseWorkSpaceRecord(SBCWorkSpace& client);
  static void eraseWorkSpaceRecord(SBCWorkSpace& client, const std::string& key);
  
  bool operator < (const SBCRegistrationRecord& registration) const;
protected:
  //Contact = "sip:2017@192.168.1.98";
  //Packet-Source = "112.211.196.68:5060";
  //Local-Interface = "174.142.82.79:5060";
  //transport-id = "0";
  //target-transport = "udp";
  //AOR = "sip:2017@ezuce.com";
  //Error-Cnt = 0;

  std::string _contact;
  std::string _packetSource;
  std::string _localInterface;
  std::string _transportId;
  std::string _targetTransport;
  std::string _targetAddress;
  std::string _targetPort;
  std::string _aor;
  int _expires;
  bool _enc;
  mutable std::string _key;
  std::string _callId;
  OSS::UInt64 _timeStamp;
};

//
// Inlines
//
inline std::string& SBCRegistrationRecord::contact()
{
  return _contact;
}

inline std::string& SBCRegistrationRecord::packetSource()
{
  return _packetSource;
}

inline std::string& SBCRegistrationRecord::localInterface()
{
  return _localInterface;
}

inline std::string& SBCRegistrationRecord::transportId()
{
  return _transportId;
}

inline std::string& SBCRegistrationRecord::targetTransport()
{
  return _targetTransport;
}

inline std::string& SBCRegistrationRecord::aor()
{
  return _aor;
}

inline int& SBCRegistrationRecord::expires()
{
  return _expires;
}

inline bool& SBCRegistrationRecord::enc()
{
  return _enc;
}

inline std::string& SBCRegistrationRecord::callId()
{
  return _callId;
}
inline OSS::UInt64& SBCRegistrationRecord::timeStamp()
{
  return _timeStamp;
}

inline const std::string& SBCRegistrationRecord::contact() const
{
  return _contact;
}

inline const std::string& SBCRegistrationRecord::packetSource() const
{
  return _packetSource;
}

inline const std::string& SBCRegistrationRecord::localInterface() const
{
  return _localInterface;
}

inline const std::string& SBCRegistrationRecord::transportId() const
{
  return _transportId;
}

inline const std::string& SBCRegistrationRecord::targetTransport() const
{
  return _targetTransport;
}

inline const std::string& SBCRegistrationRecord::aor() const
{
  return _aor;
}

inline const int& SBCRegistrationRecord::expires() const
{
  return _expires;
}

inline const bool& SBCRegistrationRecord::enc() const
{
  return _enc;
}

inline const std::string& SBCRegistrationRecord::callId() const
{
  return _callId;
}
inline const OSS::UInt64& SBCRegistrationRecord::timeStamp() const
{
  return _timeStamp;
}

inline bool SBCRegistrationRecord::writeToFile(const boost::filesystem::path& file) const
{
  return writeToFile(boost_path(file));
}

inline bool SBCRegistrationRecord::readFromFile(const boost::filesystem::path& file)
{
  return readFromFile(boost_path(file));
}

inline bool SBCRegistrationRecord::operator < (const SBCRegistrationRecord& registration) const
{
  //
  // We want descending order.   Records with higher timestamp goes first.
  // This guarantees that the ->begin() will always point to the latest
  // registration
  //
  return  _timeStamp > registration._timeStamp;
}



} } } // OSS::SIP::SBC
#endif	/* REGISTRATIONRECORD_H */

