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

#ifndef CONSOLE_H_INCLUDED
#define	CONSOLE_H_INCLUDED


#include "OSS/UTL/CoreUtils.h"


namespace OSS {
namespace UTL {


class Console
{
public:
  static std::string prompt(const std::string& message); 
  static void registerCompletion(const std::string& prefix, const std::string& completion);
  static void registerHint(const std::string& prefix, const std::string& completion);
  static void setHintProperties(int color, bool bold);
  static void setMultiLine(bool multiLine);
  static void disableSigInt(const std::string& message);
};


} } // OSS::UTL
  
  
#endif	// CONSOLE_H_INCLUDED

