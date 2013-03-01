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


#include "OSS/SIP/SIPFsm.h"
#include "OSS/SIP/SIPFSMDispatch.h"
#include "OSS/SIP/SIPTransaction.h"
#include "OSS/SIP/SIPTransactionPool.h"


namespace OSS {
namespace SIP {


SIPFsm::SIPFsm(
  boost::asio::io_service& ioService,
  const SIPTransactionTimers& timerProps) :
  _owner(0),
  _ioService(ioService),
  _pDispatch(0),
  _timerProps(timerProps),
  _timerA(_ioService, boost::posix_time::milliseconds(0)),
  _timerB(_ioService, boost::posix_time::milliseconds(0)),
  _timerC(_ioService, boost::posix_time::milliseconds(0)),
  _timerD(_ioService, boost::posix_time::milliseconds(0)),
  _timerE(_ioService, boost::posix_time::milliseconds(0)),
  _timerF(_ioService, boost::posix_time::milliseconds(0)),
  _timerG(_ioService, boost::posix_time::milliseconds(0)),
  _timerH(_ioService, boost::posix_time::milliseconds(0)),
  _timerI(_ioService, boost::posix_time::milliseconds(0)),
  _timerJ(_ioService, boost::posix_time::milliseconds(0)),
  _timerK(_ioService, boost::posix_time::milliseconds(0)),
  _timerMaxLifetime(_ioService, boost::posix_time::milliseconds(0))
  
{
}

SIPFsm::~SIPFsm()
{
  delete static_cast<SIPTransaction::WeakPtr*>(_owner);
}

SIPTransaction* SIPFsm::getOwner() const
{
  SIPTransaction::Ptr owner = static_cast<SIPTransaction::WeakPtr*>(_owner)->lock();
  return owner.get();
}

void SIPFsm::setOwner(OSS_HANDLE owner)
{
  _owner = owner;
}

void SIPFsm::startTimerA(unsigned long expire)
{
  _timerA.cancel();
  _timerA.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerA() : expire));
  _timerA.async_wait(boost::bind(&SIPFsm::handleTimerA, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerB(unsigned long expire)
{
  _timerB.cancel();
  _timerB.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerB() : expire));
  _timerB.async_wait(boost::bind(&SIPFsm::handleTimerB, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerC(unsigned long expire)
{
  _timerC.cancel();
  _timerC.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerC() : expire));
  _timerC.async_wait(boost::bind(&SIPFsm::handleTimerC, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerD(unsigned long expire)
{
  _timerD.cancel();
  _timerD.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerD() : expire));
  _timerD.async_wait(boost::bind(&SIPFsm::handleTimerD, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerE(unsigned long expire)
{
  _timerE.cancel();
  _timerE.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerE() : expire));
  _timerE.async_wait(boost::bind(&SIPFsm::handleTimerE, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerF(unsigned long expire)
{
  _timerF.cancel();
  _timerF.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerF() : expire));
  _timerF.async_wait(boost::bind(&SIPFsm::handleTimerF, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerG(unsigned long expire)
{
  _timerG.cancel();
  _timerG.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerG() : expire));
  _timerG.async_wait(boost::bind(&SIPFsm::handleTimerG, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerH(unsigned long expire)
{
  _timerH.cancel();
  _timerH.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerH() : expire));
  _timerH.async_wait(boost::bind(&SIPFsm::handleTimerH, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerI(unsigned long expire)
{
  _timerI.cancel();
  _timerI.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerI() : expire));
  _timerI.async_wait(boost::bind(&SIPFsm::handleTimerI, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerJ(unsigned long expire)
{
  _timerJ.cancel();
  _timerJ.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerJ() : expire));
  _timerJ.async_wait(boost::bind(&SIPFsm::handleTimerJ, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerK(unsigned long expire)
{
  _timerK.cancel();
  _timerK.expires_from_now(boost::posix_time::milliseconds(expire == 0 ? _timerProps.timerK() : expire));
  _timerK.async_wait(boost::bind(&SIPFsm::handleTimerK, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::startTimerMaxLifetime(unsigned long expire)
{
  _timerMaxLifetime.cancel();
  _timerMaxLifetime.expires_from_now(boost::posix_time::milliseconds(expire));
  _timerMaxLifetime.async_wait(boost::bind(&SIPFsm::handleTimerMaxLifetime, shared_from_this(), boost::asio::placeholders::error));
}

void SIPFsm::handleTimerA(const boost::system::error_code& e)
{
  if (!e)_timerAFunc();
}

void SIPFsm::handleTimerB(const boost::system::error_code& e)
{
  if (!e)_timerBFunc();
}

void SIPFsm::handleTimerC(const boost::system::error_code& e)
{
  if (!e)_timerCFunc();
}

void SIPFsm::handleTimerD(const boost::system::error_code& e)
{
  if (!e)_timerDFunc();
}

void SIPFsm::handleTimerE(const boost::system::error_code& e)
{
  if (!e)_timerEFunc();
}

void SIPFsm::handleTimerF(const boost::system::error_code& e)
{
  if (!e)_timerFFunc();
}

void SIPFsm::handleTimerG(const boost::system::error_code& e)
{
  if (!e)_timerGFunc();
}

void SIPFsm::handleTimerH(const boost::system::error_code& e)
{
  if (!e)_timerHFunc();
}

void SIPFsm::handleTimerI(const boost::system::error_code& e)
{
  if (!e)_timerIFunc();
}

void SIPFsm::handleTimerJ(const boost::system::error_code& e)
{
  if (!e)_timerJFunc();
}

void SIPFsm::handleTimerK(const boost::system::error_code& e)
{
  if (!e)_timerKFunc();
}

void SIPFsm::handleTimerMaxLifetime(const boost::system::error_code& e)
{
  if (!e)_timerMaxLifetimeFunc();
}

void SIPFsm::cancelAllTimers()
{
  _timerA.cancel();
  _timerB.cancel();
  _timerC.cancel();
  _timerD.cancel();
  _timerE.cancel();
  _timerF.cancel();
  _timerG.cancel();
  _timerH.cancel();
  _timerI.cancel();
  _timerJ.cancel();
  _timerK.cancel();
  _timerMaxLifetime.cancel();
}

void SIPFsm::onTerminate()
{

}

} } // OSS::SIP

