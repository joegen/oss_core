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


#ifndef OSS_PERSISTENT_CLASSTYPE_H_INCLUDED
#define OSS_PERSISTENT_CLASSTYPE_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_CONFIG
#if OSS_HAVE_CONFIGPP

#include "OSS/OSS.h"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "OSS/Persistent/Persistent.h"
#include "OSS/Persistent/DataType.h"
#include "OSS/Persistent/Reference.h"
#include "OSS/UTL/Thread.h"


namespace OSS {
namespace Persistent {

class OSS_API ClassType
{
public:
  ClassType();
    /// Create a new persistent class

  ClassType(const ClassType& classType);
    /// Create a class type from another

  ~ClassType();
    /// Destroy the class type

  ClassType& operator=(const ClassType& classType);
    /// Copy a class type from another

  void swap(ClassType& classType);
    /// Exchange the value of two class type

  DataType& self();
    /// Return the root datatype for the persistent class

  DataType operator[](const char* memberPath) const;
    /// Return the member datatype for the persistent class represented by memberPath;

  DataType operator[](const std::string& memberPath) const;
    /// Return the member datatype for the persistent class represented by memberPath;


  bool load(const boost::filesystem::path& file);
    /// Load the persistent class.
    /// Throws an exception if the file cannot be parsed

  bool loadString(const std::string& config);
    /// Load a classtype from a string

  void persist(const boost::filesystem::path& file);
    /// Persist the class to a file.
    /// Throws an exception if the file cannot be written to.

  void remove();
    /// Delete the persistent file

  static void remove(const boost::filesystem::path& file);
    /// Delete the persitent file

  static std::string createSafeFileName(const char* str);
    /// Create a safe file name from the given string.
    /// The behavior of this function is to convert
    /// all none alpha-numeric characters in the string
    /// to hex.  If the resulting string exceeds 256
    /// characters in length, it will be truncated.
    ///
    /// Take note that this function is only intended for atual file
    /// names and not the path. This function will convert '/' pr '\\' to hex
    /// making the path invalid!

  typedef boost::function<std::string(const boost::filesystem::path&)> FileOpenFilter;
  typedef boost::function<void(const boost::filesystem::path&)> FileSaveFilter;
private:
  Reference::Ptr _persistentClass;
  DataType _root;
  bool _isLoaded;
  boost::filesystem::path _currentFile;
  static OSS::mutex_critic_sec _csFileMutex;
  friend class DataType;
};

//
// Inlines
//

inline DataType ClassType::operator[] (const char* memberPath) const
{
  return const_cast<ClassType&>(*this).self()[memberPath];
}

inline DataType ClassType::operator[] (const std::string& memberPath) const
{
  return const_cast<ClassType&>(*this).self()[memberPath.c_str()];
}

inline void ClassType::remove()
{
  remove( _currentFile);
}

} } // OSS::Persistent


#endif // OSS_HAVE_CONFIGPP
#endif // ENABLE_FEATURE_CONFIG
#endif // OSS_PERSISTENT_CLASSTYPE_H_INCLUDED
