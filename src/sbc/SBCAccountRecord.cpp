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


#include "OSS/SIP/SBC/SBCAccountRecord.h"
#include "OSS/SIP/SIPDigestAuth.h"


namespace OSS {
namespace SIP {
namespace SBC {
  
  
SBCAccountRecord::SBCAccountRecord()
{
}

SBCAccountRecord::SBCAccountRecord(const SBCAccountRecord& copy)
{
  _identity = copy._identity;
  _user = copy._user;
  _realm = copy._realm;
  _a1Hash = copy._a1Hash;
}

SBCAccountRecord::~SBCAccountRecord()
{
}

void SBCAccountRecord::swap(SBCAccountRecord& copy)
{
  std::swap(_identity, copy._identity);
  std::swap(_user, copy._user);
  std::swap(_realm, copy._realm);
  std::swap(_a1Hash, copy._a1Hash);
}

SBCAccountRecord& SBCAccountRecord::operator = (const SBCAccountRecord& copy)
{
  SBCAccountRecord swappable(copy);
  swap(swappable);
  return *this;
}
  
void SBCAccountRecord::toJson(json::Object& object)
{
  if (!_identity.empty())
  {
    object["identity"] = json::String(_identity);
  }
  
  if (!_user.empty())
  {
    object["user"] = json::String(_user);
  }
  
  if (!_realm.empty())
  {
    object["realm"] = json::String(_realm);
  }
  
  if (!_a1Hash.empty())
  {
    object["a1-hash"] = json::String(_a1Hash);
  }
}

bool SBCAccountRecord::readFromWorkSpace(const SBCWorkSpaceManager::WorkSpace& workspace, const std::string& key)
{
  json::Object response;
  if (!workspace->get(key, response))
    return false;
  
  json::Object::iterator identity = response.Find("identity");
  if (identity != response.End())
  {
    json::String element = identity->element;
    _identity = element.Value();
  }
  
  json::Object::iterator user = response.Find("user");
  if (user != response.End())
  {
    json::String element = user->element;
    _user = element.Value();
  }
  
  json::Object::iterator realm = response.Find("realm");
  if (realm != response.End())
  {
    json::String element = realm->element;
    _realm = element.Value();
  }
  
  json::Object::iterator a1Hash = response.Find("a1-hash");
  if (a1Hash != response.End())
  {
    json::String element = a1Hash->element;
    _a1Hash = element.Value();
  }
  else
  {
    json::Object::iterator pwdIter = response.Find("password");
    if (pwdIter != response.End())
    {
      json::String element = pwdIter->element;
      std::string password = element.Value();
      //
      // Compute A1
      //
      if (!_user.empty() && !_realm.empty() && !password.empty())
      {
        _a1Hash = SIPDigestAuth::digestCreateA1Hash(_user, password, _realm);
        //
        // Save this new record
        //
        if (!_a1Hash.empty())
        {
          json::Object newObject;
          toJson(newObject);
          workspace->set(key, newObject);
        }
      }
    }
  }
  
  return true;
}
  
bool SBCAccountRecord::writeToWorkSpace(const SBCWorkSpaceManager::WorkSpace& workspace, const std::string& key)
{
  json::Object params;
  toJson(params);
  return workspace->set(key, params);
}

bool SBCAccountRecord::computeA1Hash(const std::string& password, std::string& a1Hash) const
{
  if (_user.empty() || _realm.empty())
  {
    return false;
  }
  a1Hash = SIPDigestAuth::digestCreateA1Hash(_user, password, _realm);
  return true;
}

bool SBCAccountRecord::computeAuthenticateChallengeHeader(const std::string& callId, const std::string& forceNonce, SIPAuthorization& challengeResponse) const
{
  if (!isValid())
  {
    return false;
  }
  //
  // Proxy-Authenticate: Digest realm=\"gobitech.inc\", nonce=\"26406c04b3dae533c62787a1d23ede67560ccda9\", qop=\"auth\"\r"
  //
  std::string nonce;
  if (forceNonce.empty())
  {
    nonce = SIPDigestAuth::digestCreateNonce(callId);
  }
  else
  {
    nonce =forceNonce;
  }
    
  challengeResponse.setRealm(_realm.c_str());
  challengeResponse.setQop("auth");
  challengeResponse.setNonce(nonce.c_str());
  
  return true;
}

bool SBCAccountRecord::isValid() const
{
  return !_identity.empty() &&
    !_user.empty() &&
    !_realm.empty() &&
    !_a1Hash.empty();
}

} } } // OSS::SIP::SBC


  
