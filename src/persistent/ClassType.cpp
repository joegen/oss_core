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


#include <sstream>
#include "OSS/Persistent/ClassType.h"
#include <libconfig.h++>
#include <boost/interprocess/creation_tags.hpp>
#include "OSS/Logger.h"
#include "OSS/Core.h"
#include <fstream>


namespace OSS {
namespace Persistent {


/* JEB:  VERY IMPORTANT NOTE !!!! 
   Using a mutex for statefiles will cause a deadlock on transactions.
   So far testing proves that we can get rid of the mutex here at least
   until such time when a safe solution is achieved.
*/

//OSS::mutex_critic_sec ClassType::_csFileMutex;

ClassType::ClassType() : _isLoaded(false)
{
  _persistentClass = Reference::Ptr(new Reference());
}

ClassType::ClassType(const ClassType& classType)
{
  _persistentClass = classType._persistentClass;
  _root = classType._root;
  _isLoaded = classType._isLoaded;
}

ClassType::~ClassType()
{
}

void ClassType::swap(ClassType& classType)
{
  ClassType clonable(classType);
  std::swap(_persistentClass, classType._persistentClass);
  std::swap(_isLoaded, classType._isLoaded);
  _root.swap(classType._root);
}

ClassType& ClassType::operator=(const ClassType& classType)
{
  ClassType clonable(classType);
  swap(clonable);
  return *this;
}

DataType& ClassType::self()
{
  libconfig::Setting& setting = static_cast<libconfig::Config*>(_persistentClass->_config)->getRoot();
  _root._isVolatile = false;
  _root._persistentvalue = &setting;
  _root._persistentClass = _persistentClass;
  return _root;
}

bool ClassType::load(const boost::filesystem::path& file)
{
  if (!boost::filesystem::exists(file))
  {
    std::ostringstream errorMsg;
    errorMsg << "Persistent::ClassType::load ** File does not exist ** " << file;
    OSS_LOG_ERROR(errorMsg.str());
    return false;
  }

 // _csFileMutex.lock();

  _currentFile = file;

  OSS_ASSERT(!_isLoaded);
  try
  {
      static_cast<libconfig::Config*>(_persistentClass->_config)->readFile(OSS::boost_path(file).c_str());
  }
  catch(libconfig::ParseException e)
  {
    std::ostringstream errorMsg;
    errorMsg << "Persistent::ClassType::load Error reading " << file << " at line " << e.getLine() << " with error " << e.getError();
    OSS_LOG_ERROR(errorMsg.str());
    return false;
  }
  catch(OSS::Exception e)
  {
    //_csFileMutex.unlock();
    std::ostringstream errorMsg;
    errorMsg << "Persistent::ClassType::load Error reading " << file << " - " << e.message();
    OSS_LOG_ERROR(errorMsg.str());
    return false;
  }
  catch(...)
  {
    std::ostringstream errorMsg;
    errorMsg << "Persistent::ClassType::load Error reading " << file;
    OSS_LOG_ERROR(errorMsg.str());
    return false;
  }

  _isLoaded = true;

  return true;
  
}

bool ClassType::loadString(const std::string& config)
{
  std::ofstream tempFile;
  std::string fn;
  if (OSS::boost_temp_file(fn))
  {
    std::ofstream tempFile(fn.c_str());
    if (tempFile.is_open())
    {
      tempFile.write(config.data(), config.size());
      tempFile.close();
      bool ok = load(fn);
      boost::filesystem::remove(fn);
      
      if (!ok)
      {
        OSS_LOG_ERROR("ClassType::loadString - Unable to load " << fn.c_str() << " for reading.");
      }
      return ok;
    }
    else
    {
      OSS_LOG_ERROR("ClassType::loadString - Unable to open " << fn.c_str() << " for writing.");
    }
  }
  else
  {
    OSS_LOG_ERROR("ClassType::loadString - Unable to generate new temp filename");
  }
  return false;
}

void ClassType::persist(const boost::filesystem::path& file)
{
  //_csFileMutex.lock();

  _currentFile = file;
  try
  {
    static_cast<libconfig::Config*>(_persistentClass->_config)->writeFile(OSS::boost_path(file).c_str());
  }
  catch(...)
  {
    //_csFileMutex.unlock();
    throw PersistenceException("Persistent::ClassType::persist Unable to save persistent class to file");
  }
  //_csFileMutex.unlock();
}

std::string ClassType::createSafeFileName(const char* str)
{
  if (!str || *str == '\0')
    throw PersistenceException("null parameter input while calling std::string ClassType::createSafeFileName(const char* str)");

  char* buff = const_cast<char*>(str);
  std::ostringstream strm;

  std::locale loc;
  for (char c = *buff; c != '\0' && strm.str().size() < 256; buff++)
  {
    c = *buff;
    if (c == '.' || c == '_' || c == '-' || ::std::isalnum(c, loc))
      strm << c;
    else
      strm << std::hex << (int)c << std::dec;
  }

  return strm.str();
}

void ClassType::remove(const boost::filesystem::path& file)
{
  //_csFileMutex.lock();
  try{boost::filesystem::remove(file);}catch(...){}
  //_csFileMutex.unlock();
}

} } // OSS::Persistent

