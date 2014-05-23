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


#include "OSS/Persistent/RESTKeyValueStore.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPBasicCredentials.h"


using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Net::HTTPBasicCredentials;
  

namespace OSS {
namespace Persistent {

    
RESTKeyValueStore::RESTKeyValueStore()
{
  setHandler(boost::bind(&RESTKeyValueStore::onHandleRequest, this, _1, _2));
}
  
RESTKeyValueStore::RESTKeyValueStore(int maxQueuedConnections, int maxThreads) :
  OSS::Net::HTTPServer(maxQueuedConnections, maxThreads)
{
  setHandler(boost::bind(&RESTKeyValueStore::onHandleRequest, this, _1, _2));
}

RESTKeyValueStore::~RESTKeyValueStore()
{
  
}

bool RESTKeyValueStore::isAuthorized(Request& request, Response& response)
{
  if (_user.empty())
    return true;
  
  if (request.hasCredentials())
  {
    response.requireAuthentication(request.getURI());
    response.send();
    return false;
  }
  
  HTTPBasicCredentials cred(request);
  
  return cred.getUsername() == _user && cred.getPassword() == _password;
}

void RESTKeyValueStore::onHandleRequest(Request& request, Response& response)
{ 
  if (!isAuthorized(request, response))
  {
    return;
  }
  
  std::string value;
  
  HTMLForm form(request, request.stream());
  if (!form.empty() && form.has("key"))
  {
    std::string key = form.get("key");
    
    if (request.getURI() == "/get")
    {
      _data.get(key, value);
    }
    else if (request.getURI() == "/set" && form.has("value"))
    {
      int expires = 0;
      value = form.get("value");
      
      if (form.has("expires"))
      {
        expires = OSS::string_to_number<int>(form.get("expires").c_str());
        if (expires > 0)
        {
          _data.put(key, value, expires);
        }
        else
        {
          _data.put(key, value);
        }
      }
      
    }
    else if (request.getURI() == "/delete")
    {
      _data.del(key);
    }
    else if (_customHandler)
    {
      _customHandler(request, response);
      return;
    }
  }
  
  response.setChunkedTransferEncoding(true);
	response.setContentType("text/plain");

	std::ostream& ostr = response.send();
  
  if (!value.empty())
  {
    ostr << value;
  }
}
  
} }




