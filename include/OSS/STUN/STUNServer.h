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
//


#ifndef OSS_STUNSERVER_H
#define	OSS_STUNSERVER_H


#include "OSS/OSS.h"
#include "OSS/IPAddress.h"
#include "OSS/Thread.h"

namespace OSS {
namespace STUN {

class OSS_API STUNServer
{
public:
  STUNServer();

  ~STUNServer();

  bool initialize(
    const OSS::IPAddress& primary,
    const OSS::IPAddress& secondary);

  void run();

  void stop();
private:

  void internal_run();

  OSS::IPAddress _primaryIp;
  OSS::IPAddress _secondaryIp;
  OSS_HANDLE _config;
  OSS::semaphore _exitSync;
};

} } // OSS::STUN


#endif //OSS_STUNCLIENT_H



