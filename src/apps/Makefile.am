if ENABLE_FEATURE_COMPILE_SIPTEST
  bin_PROGRAMS += oss_test_sip_invite_simple
  oss_test_sip_invite_simple_SOURCES = siptest/TestInviteSimple.cpp
endif

if ENABLE_FEATURE_B2BUA
if ENABLE_FEATURE_CARP 
if ENABLE_FEATURE_V8
    bin_PROGRAMS += oss_core
    oss_core_SOURCES = apps/oss_core.cpp
endif
endif
endif

if ENABLE_FEATURE_V8
    bin_PROGRAMS += oss_js
    oss_js_SOURCES = apps/oss_js.cpp
endif

if ENABLE_FEATURE_CARP
    bin_PROGRAMS += oss_garp oss_carp
    oss_garp_SOURCES = apps/oss_garp.cpp
    oss_carp_SOURCES = apps/oss_carp.cpp
endif


BUILT_SOURCES +=  \
    apps/AccessList.js.h \
    apps/AuthProfile.js.h \
    apps/PropertyObject.js.h \
    apps/RouteProfile.js.h \
    apps/SIPMessage.js.h \
    apps/TransactionProfile.js.h

CLEANFILES +=   \
    apps/AccessList.js.h \
    apps/AuthProfile.js.h \
    apps/PropertyObject.js.h \
    apps/RouteProfile.js.h \
    apps/SIPMessage.js.h \
    apps/TransactionProfile.js.h

#
# Build rule for Scripts
#
apps/%.js.h: apps/scripts/%.js
	$(top_srcdir)/config/doublequoteme $< $@
