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
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USvoidE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef OSS_CORE_H_INCLUDED
#define OSS_CORE_H_INCLUDED


#include "OSS/OSS.h"
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

#include <boost/detail/atomic_count.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"


namespace OSS {

//
// basic type definitions
//

typedef boost::detail::atomic_count atomic_counter; /// A thread safe counter

//
// Various string helper functions
//

template <size_t size>
bool string_sprintf_string(std::string& str, const char * format, ...)
  /// Write printf() style formatted data to string.
  ///
  /// The size template argument specifies the capacity of
  /// the internal buffer that will store the string result.
  /// Take note that if the buffer size is not enough
  /// the result of vsprintf will result to an overrun
  /// and will corrupt memory.
{
    char buffer[size];
    va_list args;
    va_start(args, format);
    int ret = vsprintf(buffer,format, args);
    va_end (args);
    if (ret >= 0)
    {
      str = buffer;
      return true;
    }
    return false;
}

template <typename T>
std::string string_from_number(T var)
  /// This is a template function that converts basic types to std::string
{
  return boost::lexical_cast<std::string>(var);
}


bool OSS_API string_starts_with(const std::string& str, const char* key);
  /// Returns true if the string starts with a the string contained in key.
  /// This function is case-sensitive

bool OSS_API string_caseless_starts_with(const std::string& str, const char* key);
  /// Returns true if the string starts with a the string contained in key.
  /// This function is NONE case-sensitive

bool OSS_API string_ends_with(const std::string& str, const char* key);
  /// Returns true if the string ends with a the string contained in key.
  /// This function is case-sensitive

bool OSS_API string_caseless_ends_with(const std::string& str, const char* key);
  /// Returns true if the string ends with a the string contained in key.
  /// This function is NONE case-sensitive

template <typename T>
T string_to_number(const std::string& str, T errorValue = 0)
  /// Convert a string to a numeric value
{
  try { return boost::lexical_cast<T>(str);} catch(...){return errorValue;};
}

void OSS_API string_to_lower(std::string& str);
  /// Converts a string to lower case

void OSS_API string_to_upper(std::string& str);
  /// Converts a string to upper case

std::string OSS_API string_left(const std::string& str, size_t size);
  /// Returns a number of characters from the left of the string
  ///
  /// If the length of the string is shorter than size, the whole string is returned

std::string OSS_API string_left_until(const std::string& str, const char* key);
  /// Returns the characters before the occurence of key
  ///
  /// If the key is non-existent the whole string is returned


std::string OSS_API string_right(const std::string& str, size_t size);
  /// Returns a number of characters from the right of the string
  ///
  /// If the length of the string is shorter than size, the whole string is returned

void OSS_API string_trim(std::string& stringToTrim);
  /// Trim trailing and leading white spaces off a string

std::vector<std::string> OSS_API string_tokenize(const std::string& str, const char* tokens);
  /// Tokenize a string using characters found in tokens

void OSS_API string_replace(std::string& str, const char* what, const char* with);
  /// replace elements ina string

std::string OSS_API string_create_uuid(bool random = true);
  /// Generate a time based Universal Unique Identifier string

std::string OSS_API string_md5_hash(const char* input);
  /// Generate and Md5 string from a string

size_t OSS_API string_hash(const char* str);
  /// returns a numeric hash of the string


bool OSS_API string_wildcard_compare(const char* wildCard, const std::string& str);
  /// compare a string to a dos style wild card match

unsigned int string_to_js_hash(const std::string& str);
  /// A bitwise hash function written by Justin Sobel

std::string string_to_64_bit_hash(const std::string& id);
  /// Return a hex formatted string representation of the JS-Hash

bool string_format_json(std::string& str);
  /// Returns a formatted version of the json string.  
  /// Will return the unformatted string back if an exception is encountered
  /// and function returning false

//
// Misc functions
//

OSS::UInt64 OSS_API getTicks();
  /// Generate time ticks

int OSS_API getRandom();
  /// Generate pseudo random number based on time ticks

OSS::UInt64 OSS_API getTime();
  /// Return milliseconds since epoch

bool OSS_API isFileOlderThan(const boost::filesystem::path& file, int minutes);
  /// Utility function to check the modified date of the file

std::string OSS_API boost_file_name(const boost::filesystem::path& path);

std::string OSS_API boost_path(const boost::filesystem::path& path);

boost::filesystem::path OSS_API boost_path_concatenate(const boost::filesystem::path& path, const std::string& file);

bool OSS_API boost_temp_file(std::string& tempfile);

std::string boost_format_time(boost::posix_time::ptime time, const std::string& format = "%Y-%m-%d %H:%M:%S");
  /// Utitlity function to format boost ptime to string.  
  /// Example:
  /// boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
  /// std::cout << boost_format_time(now) << std::endl;

void vectorToCArray(const std::vector<std::string>& args, char*** argv);
  /// Convert a vector to argv format

void freeCArray(int argc, char*** argv);
  /// Free the array allocated by vectorToCArray

void __init_system_dir();
  /// Initialze system directory global map

const std::string& system_prefix();

const std::string& system_exec_prefix();

const std::string& system_bindir();

const std::string& system_sbindir();

const std::string& system_libexecdir();

const std::string& system_datadir();

const std::string& system_confdir();

const std::string& system_localstatedir();

const std::string& system_includedir();

const std::string& system_libdir();

} // OSS



#endif //OSS_CORE_H_INCLUDED


