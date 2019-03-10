#include "OSS/SIP/SBC/SBCRegistrationRecord.h"
#include "OSS/Persistent/ClassType.h"
#include "OSS/Persistent/DataType.h"
#include <boost/filesystem.hpp>
#include "OSS/UTL/Logger.h"


namespace OSS {
namespace SIP {
namespace SBC {


using OSS::Persistent::ClassType;
using OSS::Persistent::DataType;

SBCRegistrationRecord::SBCRegistrationRecord() :
  _expires(3600),
  _enc(false),
  _timeStamp(0)
{
  _timeStamp = OSS::getTime();
}

SBCRegistrationRecord::SBCRegistrationRecord(const SBCRegistrationRecord& record)
{
  _contact = record._contact;
  _packetSource = record._packetSource;
  _localInterface = record._localInterface;
  _transportId = record._transportId;
  _targetTransport = record._targetTransport;
  _aor = record._aor;
  _expires = record._expires;
  _enc = record._enc;
  _key = record._key;
  _callId = record._callId;
  _timeStamp = record._timeStamp;
}

SBCRegistrationRecord::~SBCRegistrationRecord()
{
}

void SBCRegistrationRecord::swap(SBCRegistrationRecord& record)
{
  std::swap(_contact, record._contact);
  std::swap(_packetSource, record._packetSource);
  std::swap(_localInterface, record._localInterface);
  std::swap(_transportId, record._transportId);
  std::swap(_targetTransport, record._targetTransport);
  std::swap(_aor, record._aor);
  std::swap(_expires, record._expires);
  std::swap(_enc, record._enc);
  std::swap(_key, record._key);
  std::swap(_callId, record._callId);
  std::swap(_timeStamp, record._timeStamp);
}

SBCRegistrationRecord& SBCRegistrationRecord::operator=(const SBCRegistrationRecord& record)
{
  SBCRegistrationRecord clonable(record);
  swap(clonable);
  return *this;
}

bool SBCRegistrationRecord::readFromRedis(Persistent::RedisBroadcastClient& client, const std::string& key)
{
  _key = key;
  json::Object response;
  if (!client.get(key, response))
    return false;

  json::String contact = response["contact"];
  _contact = contact.Value();
  json::String packetSource = response["packetSource"];
  _packetSource = packetSource.Value();
  json::String localInterface = response["localInterface"];
  _localInterface = localInterface.Value();
  json::String transportId = response["transportId"];
  _transportId = transportId.Value();
  json::String targetTransport = response["targetTransport"];
  _targetTransport = targetTransport.Value();
  json::String aor = response["aor"];
  _aor = aor.Value();
  json::String callId = response["call-id"];
  _callId = callId.Value();
  json::Number expires = response["expires"];
  _expires = expires.Value();
  json::Number timestamp = response["timestamp"];
  _timeStamp = timestamp.Value();
  json::Boolean enc = response["enc"];
  _enc = enc.Value();

  OSS_LOG_DEBUG("[REDIS] Read registration record from database - " <<
    " key: " << key <<
    " contact: " << _contact <<
    " packetSource: " << _packetSource <<
    " localInterface: " << _localInterface <<
    " transportId: " << _transportId <<
    " targetTransport: " << _targetTransport <<
    " aor: " << _aor <<
    " expires: " << _expires <<
    " enc: " << _enc);

  return true;
}

bool SBCRegistrationRecord::writeToRedis(Persistent::RedisBroadcastClient& client, const std::string& key) const
{
  _key = key;
 
  std::ostringstream json;
  json << "{ ";
    json << "\"" << "timestamp" << "\"" << " : " <<  _timeStamp;
      json << ", ";
    json << "\"" << "call-id" << "\"" << " : " <<  "\"" << _callId << "\"";
      json << ", ";
    json << "\"" << "aor" << "\"" << " : " <<  "\"" << _aor << "\"";
      json << ", ";
    json << "\"" << "contact" << "\"" << " : " <<  "\"" << _contact << "\"";
      json << ", ";
    json << "\"" << "expires" << "\"" << " : " <<  _expires;
      json << ", ";
    json << "\"" << "transportId" << "\"" << " : " <<  "\"" << _transportId << "\"";
      json << ", ";
    json << "\"" << "packetSource" << "\"" << " : " <<  "\"" << _packetSource << "\"";
      json << ", ";
    json << "\"" << "localInterface" << "\"" << " : " <<  "\"" << _localInterface << "\"";
      json << ", ";
    json << "\"" << "targetTransport" << "\"" << " : " <<  "\"" << _targetTransport << "\"";
      json << ", ";
    json << "\"" << "enc" << "\"" << " : " <<  _enc;
  json << " }";
  
  

  OSS_LOG_DEBUG("[REDIS] Persisting registration record to database - " <<
    " key: " << _key <<
    " contact: " << _contact <<
    " packetSource: " << _packetSource <<
    " localInterface: " << _localInterface <<
    " transportId: " << _transportId <<
    " targetTransport: " << _targetTransport <<
    " aor: " << _aor <<
    " expires: " << _expires <<
    " enc: " << _enc);

  return client.set(_key, json.str(), _expires);
}


void SBCRegistrationRecord::eraseRedisRecord(Persistent::RedisBroadcastClient& client)
{
  eraseRedisRecord(client, _key);
}

void SBCRegistrationRecord::eraseRedisRecord(Persistent::RedisBroadcastClient& client, const std::string& key)
{
  client.del(key);
}


} } } // OSS::SIP::SBC