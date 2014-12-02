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


#include <boost/thread.hpp>
#include "OSS/SIP/UA/SIPEventLoop.h"
#include "re.h"


namespace OSS {
namespace SIP {
namespace UA {


SIPEventLoop::SIPEventLoop() :
  _pRunThread(0),
  _memCheck(false),
  _tmrCheck(false),
  _closeDescriptors(false)
{
  libre_init();
}

SIPEventLoop::~SIPEventLoop()
{
  if (_pRunThread)
  {
    waitForTermination();
    delete _pRunThread;
  }
  
  if (_closeDescriptors)
    libre_close();

	/* check for memory leaks */
  if (_tmrCheck)
    tmr_debug();
  
  if (_memCheck)
    mem_debug();
}

void SIPEventLoop::waitForTermination()
{
  if (_pRunThread)
  {
    if (_pRunThread->joinable())
      _pRunThread->join();
  }
}

void SIPEventLoop::run(bool async)
{
  if (!async)
  {
    internal_run();
  }
  else
  {
    _pRunThread = new boost::thread(boost::bind(&SIPEventLoop::internal_run, this));
  }
}

void SIPEventLoop::stop(bool closeDescriptors, bool memCheck, bool tmrCheck)
{
  _memCheck = memCheck;
  _tmrCheck = tmrCheck;
  _closeDescriptors = closeDescriptors;
	re_cancel();
}

void SIPEventLoop::internal_run()
{
  re_main(0);
}

} } } // OSS::SIP::UA



