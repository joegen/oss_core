#ifndef SBCDBWORKSPACE_H
#define SBCDBWORKSPACE_H

#include "OSS/Persistent/BerkeleyDb.h"
#include "OSS/JSON/Json.h"
#include "OSS/UTL/Thread.h"

namespace OSS {
namespace SIP {
namespace SBC {

class SBCWorkSpace
{
public:
  typedef OSS::BerkeleyDb LocalDb;
  SBCWorkSpace(const std::string& name);
  ~SBCWorkSpace();
  
  bool set(const std::string& key, const std::string& value, int expires = -1);
  bool set(const std::string& key, const json::Object& value, int expires = -1);
  bool get(const std::string& key, std::string& value) const;
  bool get(const std::string& key, json::Object& value) const;
  bool del(const std::string& key);
  bool getKeys(const std::string& pattern, std::vector<std::string>& keys);
  bool open(const std::string& localDbFile);
  bool open();
  void close();
  const std::string& getName() const;
  void setName(const std::string& name);

private:
  mutable OSS::mutex_critic_sec _dbMutex;
  LocalDb _db;
  std::string _name;
};

//
// Inlines
//

inline const std::string& SBCWorkSpace::getName() const
{
  return _name;
}

inline void SBCWorkSpace::setName(const std::string& name)
{
  _name = name;
}


} } } // OSS::SIP::SBC
#endif /* SBCDBWORKSPACE_H */

