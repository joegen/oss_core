// OSS Software Solutions Application Programmer Interface
// Package: OSSAPI
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSS Core SDK.
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


#include "OSS/OSS.h"

#if OSS_OS_FAMILY_WINDOWS
  #include <Mmsystem.h>
  #include <winsock2.h>
  #include <windows.h>
  #include <stdio.h>
  #include <stdarg.h>
  #include <tchar.h>
  #include <process.h>
  #include <sys/types.h>
  #include <sys/timeb.h>
  #include <time.h>
  #include <stdlib.h>
#else
	#include <stdio.h>
	#include <stdarg.h>
	#include <unistd.h>
	#include <pthread.h>
	#include <sys/times.h>
	#include <limits.h>
	#include <sys/time.h>
	#include <time.h>
	#include <stdlib.h>
#endif

#include <string>
#include <sstream>
#include <locale>

#include <boost/algorithm/string.hpp>
#include <boost/functional/hash.hpp>
#include "OSS/Core.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/MD5Engine.h"
#include "Poco/File.h"
#include "Poco/DateTime.h"
#include "Poco/Path.h"
#include "Poco/TemporaryFile.h"

//
// Header only implementations so we catch any compile errors
//
#include "OSS/RedisClient.h"

namespace OSS {

//
// Static dependencies
//
#if defined (OSS_OS_FAMILY_WINDOWS)
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "IPHlpApi.Lib") 
#pragma comment(lib, "Winmm.lib")
#endif

//
// String helper functions
//
bool string_starts_with(const std::string& str, const char* key)
{
  return str.find(key) == 0;
}

bool string_caseless_starts_with(const std::string& str, const char* key_)
{
  std::string key = key_;
  boost::to_lower(key);
  std::string lstr = str;
  boost::to_lower(lstr);
  return lstr.find(key) == 0; 
}

bool string_ends_with(const std::string& str, const char* key)
{
  size_t i = str.rfind(key);
  return (i != std::string::npos) && (i == (str.length() - ::strlen(key)));
}

bool string_caseless_ends_with(const std::string& str, const char* key)
{
  std::string lstr = str;
  boost::to_lower(lstr);
  size_t i = lstr.rfind(key);
  return (i != std::string::npos) && (i == (lstr.length() - ::strlen(key)));
}

void string_to_lower(std::string& str)
{
  boost::to_lower(str);
}

void string_to_upper(std::string& str)
{
  boost::to_upper(str);
}

std::string string_left(const std::string& str, size_t size)
{
  if (str.size() <= size)
    return str;
  return str.substr(0, size);
}

std::string string_left_until(const std::string& str, const char* key)
{
  size_t i = str.find(key);
  if (i != std::string::npos)
    return  str.substr(0, i);
  return str;
}

std::string string_right(const std::string& str, size_t size)
{
  if (str.size() <= size)
    return str;

  size_t index = str.size() - size;

  return str.substr(index, size);
}

void string_replace(std::string& str, const char* what, const char* with)
{
  size_t pos = 0;
  while((pos = str.find(what, pos)) != std::string::npos)
  {
     str.replace(pos, strlen(what), with);
     pos += strlen(with);
  }
}

std::vector<std::string> string_tokenize(const std::string& str, const char* tok)
{
  std::vector<std::string> tokens;
  boost::split(tokens, str, boost::is_any_of(tok), boost::token_compress_on);
  return tokens;
}

std::string string_create_uuid(bool random)
{
  if (random)
  {
    Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().createRandom();
    return uuid.toString();
  }
  else
  {
    Poco::UUID uuid = Poco::UUIDGenerator::defaultGenerator().createOne();
    return uuid.toString();
  }
}

unsigned int string_to_js_hash(const std::string& str)
  /// A bitwise hash function written by Justin Sobel
{
  unsigned int hash = 1315423911;
  for(std::size_t i = 0; i < str.length(); i++)
  {
    hash ^= ((hash << 5) + str[i] + (hash >> 2));
  }
  return hash;
}

std::string string_to_64_bit_hash(const std::string& id)
{
  unsigned int hash = string_to_js_hash(id);
  std::ostringstream newId;
  newId << std::setfill('0') << std::hex << std::setw(8)
       << hash
       << std::setfill(' ') << std::dec;
  return newId.str();
}

//
// Misc functions
//

OSS::UInt64 getTicks()
{
#if OSS_OS_FAMILY_WINDOWS
  return (OSS::UInt64)timeGetTime();
#elif OSS_OS == OSS_OS_LINUX
	struct tms sTMS;
	clock_t ticks = times( &sTMS );
  return (OSS::UInt64)( ticks * ( 1000.0 / sysconf( _SC_CLK_TCK ) ) );
#elif OSS_OS == OSS_OS_SOLARIS
	hrtime_t tTime = gethrtime();
	return (OSS::UInt64)( tTime / 1000000 );
#else
  //
  //Find something for other OS
  //
  return 0;
#endif
}

int getRandom()
{
#if OSS_OS_FAMILY_WINDOWS
	static bool bInit = false;
	if ( !bInit )
	{
		srand( (unsigned int)::time( 0 ) + (unsigned int)getTicks() );
		bInit = true;
	}
	return rand();
#else
	static bool bInit = false;
	static unsigned int uSeed;
	if ( !bInit )
	{
		uSeed = (unsigned int)::time( 0 ) + (unsigned int)getTicks();
		bInit = true;
	}
	return rand_r( &uSeed );
#endif
}

OSS::UInt64 getTime()
{	
#if OSS_OS_FAMILY_WINDOWS
	struct _timeb sTimeB;
	_ftime( &sTimeB );
	return (OSS::UInt64)( sTimeB.time * 1000 + sTimeB.millitm );
#else
	struct timeval sTimeVal;
	gettimeofday( &sTimeVal, NULL );
	return (OSS::UInt64)( sTimeVal.tv_sec * 1000 + ( sTimeVal.tv_usec / 1000 ) );
#endif
}

std::string string_md5_hash(const char* input)
{
  OSS_VERIFY_NULL(input);
  OSS_VERIFY((*input) != 0x00);
  Poco::MD5Engine engine;
  engine.update(input, ::strlen(input));
  return Poco::MD5Engine::digestToHex(engine.digest());
}

size_t OSS_API string_hash(const char* str)
{
  static boost::hash<std::string> str_hash;
  return str_hash(str);
}

bool string_wildcard_compare(const char* wild, const std::string& str)
{
  const char* string = str.c_str();

  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

void string_trim(std::string& str)
{
  std::locale loc;
  std::string::iterator front = str.begin();
  while (front != str.end())
  {
    if (std::isspace(*front, loc))
      front = str.erase(front);
    else
      break;
  }

  std::string::iterator back = str.end();
  while (back != str.begin())
  {
    back--;
    if (std::isspace(*back, loc))
    {
      str.erase(back);
      back = str.end();
    }
    else
      break;
  }
}


bool isFileOlderThan(const boost::filesystem::path& file, int minutes)
{
  Poco::File f(OSS::boost_path(file));
  Poco::Timestamp stamp = f.getLastModified();
  Poco::DateTime modified(stamp);
  Poco::DateTime now;
  Poco::Timespan elapsed = now - modified;
  return elapsed.totalMinutes() > minutes;
}

std::string boost_file_name(const boost::filesystem::path& path)
{
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION >= 3
  return path.filename().native();
#else
  return path.filename();
#endif
}

std::string boost_path(const boost::filesystem::path& path)
{
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION >= 3
  return path.native();
#else
  return path.string();
#endif
}


bool boost_temp_file(std::string& tempfile)
{
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION >= 3
  try
  {
    boost::filesystem::path tmp = boost::filesystem::temp_directory_path();
    boost::filesystem::path fn = boost::filesystem::unique_path();
    boost::filesystem::path tempPath = operator/(tmp, OSS::boost_file_name(fn));
    tempfile = OSS::boost_path(tempPath);
    return true;
  }
  catch(...)
  {
  }
  return false;
#else
  tempfile = Poco::Path::temp();
  tempfile += "/";
  tempfile += Poco::TemporaryFile::tempName();
  return true;
#endif
}

void vectorToCArray(const std::vector<std::string>& args, char*** argv)
{
  *argv = (char**)std::malloc((args.size() + 1) * sizeof(char*));
  int i=0;
  for(std::vector<std::string>::const_iterator iter = args.begin();
      iter != args.end();
      iter++, ++i)
  {
    std::string arg = *iter;
    (*argv)[i] = (char*)std::malloc((arg.length()+1) * sizeof(char));
    std::strcpy((*argv)[i], arg.c_str());
  }
  (*argv)[args.size()] = NULL; // argv must be NULL terminated
}

void freeCArray(int argc, char*** argv)
{
  for (int i = 0; i < argc; i++)
    free((*argv)[i]);
  free(*argv);
}

} // OSS

