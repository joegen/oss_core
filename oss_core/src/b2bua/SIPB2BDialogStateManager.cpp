// OSS Software Solutions Application Programmer Interface
// Package: B2BUA
// Author: Joegen E. Baclor - mailto:joegen@ossapp.com
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


#include "OSS/SIP/B2BUA/SIPB2BDialogStateManager.h"


namespace OSS {
namespace SIP {
namespace B2BUA {

//
// Note SIPB2BDialogStateManager is a header only template class.
// This CPP file only serves as an early catch-all for compilation errors
//

struct DummyDataStore
{
  bool persist(const SIPB2BDialogData&){return false;}
};

typedef  SIPB2BDialogStateManager<DummyDataStore> DummyManager_Base;

class DummyManager : public DummyManager_Base
{
public:
  DummyManager(SIPB2BTransactionManager& trn) : DummyManager_Base(trn)
  {
  }
};

} } }