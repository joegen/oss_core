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


#include "OSS/BSON/BSONBool.h"


namespace OSS {
namespace BSON {

  

BSONBool::BSONBool()
{
  _type = TYPE_BOOL;
  _value = bool(false);
}

BSONBool::BSONBool(bool value)
{
  _type = TYPE_BOOL;
  _value = value;
}

BSONBool::BSONBool(const BSONBool& value)
{
  _type = TYPE_BOOL;
  _value = value._value;
}


BSONBool& BSONBool::operator=(bool value)
{
  _value = value;
  return *this;
}

BSONBool& BSONBool::operator=(const BSONBool& value)
{
  BSONBool clonable(value);
  swap(clonable);
  return *this;
}


} } // OSS::BSON


