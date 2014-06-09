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


#ifndef OSS_KEYVALUESTOREBASE_H_INCLUDED
#define OSS_KEYVALUESTOREBASE_H_INCLUDED

namespace OSS {
namespace Persistent {

struct KVRecord
{
  std::string key;
  std::string value;
};

typedef std::vector<std::string> KVKeys;
typedef std::vector<KVRecord> KVRecords;

template <typename KV>
class KeyValueStoreBase : boost::noncopyable
{
public:
  
  
  KeyValueStoreBase()
  {
  }
  
  ~KeyValueStoreBase()
  {
  }

  bool open(const std::string& path)
  {
    return _impl.open(path);
  }

  bool isOpen()
  {
    return _impl.isOpen();
  }

  bool close()
  {
    return _impl.close();
  }

  bool put(const std::string& key, const std::string& value)
  {
    return _impl.put(key, value);
  }

  bool put(const std::string& key, const std::string& value, unsigned int expireInSeconds)
  {
    return _impl.put(key, value, expireInSeconds);
  }

  bool get(const std::string& key, std::string& value)
  {
    return _impl.get(key, value);
  }

  bool del(const std::string& key)
  {
    return _impl.del(key);
  }
  
  bool getKeys(KVKeys& keys)
  {
    return _impl.getKeys(keys);
  }
  
  bool getKeys(const std::string& filter, KVKeys& keys)
  {
    return _impl.getKeys(filter, keys);
  }
  
  bool getRecords(KVRecords& records)
  {
    return _impl.getRecords(records);
  }
  
  bool getRecords(const std::string& filter, KVRecords& records)
  {
    return _impl.getRecords(filter, records);
  }
  
  bool delKeys(const std::string& filter)
  {
    return _impl.delKeys(filter);
  }
  
  std::string getPath() const
  {
    return _impl.getPath();
  }
  
protected:
  KV _impl;  
};


} } // OSS::Persistent

#endif