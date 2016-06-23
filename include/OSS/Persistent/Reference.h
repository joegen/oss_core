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


#ifndef OSS_PERSISTENT_REFERENCE_H_INCLUDED
#define OSS_PERSISTENT_REFERENCE_H_INCLUDED

#include "OSS/build.h"
#if ENABLE_FEATURE_CONFIG
#if OSS_HAVE_CONFIGPP

#include <boost/shared_ptr.hpp>
#include "OSS/OSS.h"


namespace OSS {
namespace Persistent {

class OSS_API Reference
{
public:
  typedef boost::shared_ptr<Reference> Ptr;
  Reference();
  ~Reference();

private:
  OSS::OSS_HANDLE _config;
  friend class ClassType;
  friend class DataType;
};

} } // OSS::Persistent

#endif // OSS_HAVE_CONFIGPP
#endif // ENABLE_FEATURE_CONFIG
#endif// OSS_PERSISTENT_REFERENCE_H_INCLUDED

