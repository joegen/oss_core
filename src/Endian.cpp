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


#include "OSS/Endian.h"
#include "Poco/ByteOrder.h"

namespace OSS {

using Poco::ByteOrder;

OSS::Int16 endian_flip_bytes(OSS::Int16 value)
{
  return ByteOrder::flipBytes(value);
}

OSS::UInt16 endian_flip_bytes(OSS::UInt16 value)
{
  return ByteOrder::flipBytes(value);
}

OSS::Int32 endian_flip_bytes(OSS::Int32 value)
{
  return ByteOrder::flipBytes(value);
}

OSS::UInt32 endian_flip_bytes(OSS::UInt32 value)
{
  return ByteOrder::flipBytes(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_flip_bytes(OSS::Int64 value)
{
  return ByteOrder::flipBytes(value);
}

OSS::UInt64 endian_flip_bytes(OSS::UInt64 value)
{
  return ByteOrder::flipBytes(value);
}

#endif

OSS::Int16 endian_to_big_endian(OSS::Int16 value)
{
  return ByteOrder::toBigEndian(value);
}

OSS::UInt16 endian_to_big_endian (OSS::UInt16 value)
{
  return ByteOrder::toBigEndian(value);
}

OSS::Int32 endian_to_big_endian(OSS::Int32 value)
{
  return ByteOrder::toBigEndian(value);
}

OSS::UInt32 endian_to_big_endian (OSS::UInt32 value)
{
  return ByteOrder::toBigEndian(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_to_big_endian(OSS::Int64 value)
{
  return ByteOrder::toBigEndian(value);
}

OSS::UInt64 endian_to_big_endian (OSS::UInt64 value)
{
  return ByteOrder::toBigEndian(value);
}

#endif

OSS::Int16 endian_from_big_endian(OSS::Int16 value)
{
  return ByteOrder::fromBigEndian(value);
}

OSS::UInt16 endian_from_big_endian (OSS::UInt16 value)
{
  return ByteOrder::fromBigEndian(value);
}

OSS::Int32 endian_from_big_endian(OSS::Int32 value)
{
  return ByteOrder::fromBigEndian(value);
}

OSS::UInt32 endian_from_big_endian (OSS::UInt32 value)
{
  return ByteOrder::fromBigEndian(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_from_big_endian(OSS::Int64 value)
{
  return ByteOrder::fromBigEndian(value);
}

OSS::UInt64 endian_from_big_endian (OSS::UInt64 value)
{
  return ByteOrder::fromBigEndian(value);
}
#endif

OSS::Int16 endian_to_little_endian(OSS::Int16 value)
{
  return ByteOrder::toLittleEndian(value);
}

OSS::UInt16 endian_to_little_endian (OSS::UInt16 value)
{
  return ByteOrder::toLittleEndian(value);
}

OSS::Int32 endian_to_little_endian(OSS::Int32 value)
{
  return ByteOrder::toLittleEndian(value);
}

OSS::UInt32 endian_to_little_endian (OSS::UInt32 value)
{
  return ByteOrder::toLittleEndian(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_to_little_endian(OSS::Int64 value)
{
  return ByteOrder::toLittleEndian(value);
}

OSS::UInt64 endian_to_little_endian (OSS::UInt64 value)
{
  return ByteOrder::toLittleEndian(value);
}
#endif

OSS::Int16 endian_from_little_endian(OSS::Int16 value)
{
  return ByteOrder::fromLittleEndian(value);
}

OSS::UInt16 endian_from_little_endian (OSS::UInt16 value)
{
  return ByteOrder::fromLittleEndian(value);
}

OSS::Int32 endian_from_little_endian(OSS::Int32 value)
{
  return ByteOrder::fromLittleEndian(value);
}

OSS::UInt32 endian_from_little_endian (OSS::UInt32 value)
{
  return ByteOrder::fromLittleEndian(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_from_little_endian(OSS::Int64 value)
{
  return ByteOrder::fromLittleEndian(value);
}

OSS::UInt64 endian_from_little_endian (OSS::UInt64 value)
{
  return ByteOrder::fromLittleEndian(value);
}
#endif

OSS::Int16 endian_to_network_order (OSS::Int16 value)
{
  return ByteOrder::toNetwork(value);
}

OSS::UInt16 endian_to_network_order(OSS::UInt16 value)
{
  return ByteOrder::toNetwork(value);
}

OSS::Int32 endian_to_network_order(OSS::Int32 value)
{
  return ByteOrder::toNetwork(value);
}

OSS::UInt32 endian_to_network_order (OSS::UInt32 value)
{
  return ByteOrder::toNetwork(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_to_network_order(OSS::Int64 value)
{
  return ByteOrder::toNetwork(value);
}

OSS::UInt64 endian_to_network_order (OSS::UInt64 value)
{
  return ByteOrder::toNetwork(value);
}
#endif

OSS::Int16 endian_from_network_order(OSS::Int16 value)
{
  return ByteOrder::fromNetwork(value);
}

OSS::UInt16 endian_from_network_order (OSS::UInt16 value)
{
  return ByteOrder::fromNetwork(value);
}

OSS::Int32 endian_from_network_order(OSS::Int32 value)
{
  return ByteOrder::fromNetwork(value);
}

OSS::UInt32 endian_from_network_order (OSS::UInt32 value)
{
  return ByteOrder::fromNetwork(value);
}

#if defined(OSS_HAVE_INT64)
OSS::Int64 endian_from_network_order(OSS::Int64 value)
{
  return ByteOrder::fromNetwork(value);
}

OSS::UInt64 endian_from_network_order (OSS::UInt64 value)
{
  return ByteOrder::fromNetwork(value);
}
#endif

} // OSS





