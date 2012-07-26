// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
//
// Basic definitions for the OSSAPI API.
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


#ifndef SIP_OSSSIPClientTransaction_INCLUDED
#define SIP_OSSSIPClientTransaction_INCLUDED


#include "OSS/SIP/SIPB2BTransaction.h"


namespace OSS {
namespace SIP{


class SIPB2BClientTransaction: public SIPB2BTransaction
{
  //
  // This class initiates a locally generated clietn transaction.
  // This enabled the B2BUA to act as a pure UAC.  Nomal use case for this
  // class is for the implementation of statful SIP Trunk which REGISTERs and
  // authenticates against an ITSP
  //
public:
  explicit SIPB2BClientTransaction(SIPB2BTransactionManager* pManager);
    /// Creates a new SIPB2BClientTransaction object

  virtual ~SIPB2BClientTransaction();
    /// Destroys the SIPB2BClientTransaction object

  virtual void runTask();
    /// Execute the transaction tasks
    ///
    /// This method runs in its own thread and will not block any operation
    /// in the subsystem.  It is therefore safe to call blocking functions
    /// in this method.
    ///

  virtual void runResponseTask();
    /// Execute the transaction tasks for handling responses
    ///
    /// This method runs in its own thread and will not block any operation
    /// in the subsystem.  It is therefore safe to call blocking functions
    /// in this method.
    ///

};


} } // OSS::SIP

#endif //SIP_OSSSIPClientTransaction_INCLUDED

