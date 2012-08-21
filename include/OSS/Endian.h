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


#ifndef OSS_ENDIAN_H_INCLUDED
#define OSS_ENDIAN_H_INCLUDED


#include "OSS/OSS.h"


namespace OSS {

	Int16 OSS_API endian_flip_bytes(Int16 value);

	UInt16 OSS_API endian_flip_bytes(UInt16 value);
	
  Int32 OSS_API endian_flip_bytes(Int32 value);
	
  UInt32 OSS_API endian_flip_bytes(UInt32 value);

#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_flip_bytes(Int64 value);
	
  UInt64 OSS_API endian_flip_bytes(UInt64 value);
#endif

	Int16 OSS_API endian_to_big_endian(Int16 value);
	
  UInt16 OSS_API endian_to_big_endian (UInt16 value);
	
  Int32 OSS_API endian_to_big_endian(Int32 value);
	
  UInt32 OSS_API endian_to_big_endian (UInt32 value);

#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_to_big_endian(Int64 value);
	
  UInt64 OSS_API endian_to_big_endian (UInt64 value);
#endif

	Int16 OSS_API endian_from_big_endian(Int16 value);
	
  UInt16 OSS_API endian_from_big_endian (UInt16 value);
	
  Int32 OSS_API endian_from_big_endian(Int32 value);
	
  UInt32 OSS_API endian_from_big_endian (UInt32 value);

#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_from_big_endian(Int64 value);
	
  UInt64 OSS_API endian_from_big_endian (UInt64 value);
#endif

	Int16 OSS_API endian_to_little_endian(Int16 value);
	
  UInt16 OSS_API endian_to_little_endian (UInt16 value);
	
  Int32 OSS_API endian_to_little_endian(Int32 value);
	
  UInt32 OSS_API endian_to_little_endian (UInt32 value);

#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_to_little_endian(Int64 value);
	
  UInt64 OSS_API endian_to_little_endian (UInt64 value);
#endif

	Int16 OSS_API endian_from_little_endian(Int16 value);
	
  UInt16 OSS_API endian_from_little_endian (UInt16 value);
	
  Int32 OSS_API endian_from_little_endian(Int32 value);
	
  UInt32 OSS_API endian_from_little_endian (UInt32 value);

#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_from_little_endian(Int64 value);
	
  UInt64 OSS_API endian_from_little_endian (UInt64 value);
#endif

	Int16 OSS_API endian_to_network_order (Int16 value);
	
  UInt16 OSS_API endian_to_network_order(UInt16 value);
	
  Int32 OSS_API endian_to_network_order(Int32 value);
	
  UInt32 OSS_API endian_to_network_order (UInt32 value);
#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_to_network_order(Int64 value);
	
  UInt64 OSS_API endian_to_network_order (UInt64 value);
#endif

	Int16 OSS_API endian_from_network_order(Int16 value);
	
  UInt16 OSS_API endian_from_network_order (UInt16 value);
	
  Int32 OSS_API endian_from_network_order(Int32 value);
	
  UInt32 OSS_API endian_from_network_order (UInt32 value);

#if defined(OSS_HAVE_INT64)
	Int64 OSS_API endian_from_network_order(Int64 value);
	
  UInt64 OSS_API endian_from_network_order (UInt64 value);
#endif

} // OSS


#endif // OSS_ENDIAN_H_INCLUDED


