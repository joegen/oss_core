if ENABLE_FEATURE_COMPILE_SIPTEST
SIP_TEST = \
    oss_test_sip_invite_simple
oss_test_sip_invite_simple_SOURCES = siptest/TestInviteSimple.cpp
endif

bin_PROGRAMS += \
    $(SIP_TEST)

if ENABLE_FEATURE_B2BUA
if ENABLE_FEATURE_CARP 
if ENABLE_FEATURE_LIBRE 
if ENABLE_FEATURE_V8
bin_PROGRAMS += \
    oss_core \
    oss_rest
endif
endif
endif
endif

if ENABLE_FEATURE_CARP
bin_PROGRAMS += \
    oss_garp \
    oss_carp
endif

#
# oss_core - A simple back-to-back user agent
#
oss_core_SOURCES = apps/oss_core.cpp

#
# oss_garp - tool for sending gratuitous ARP requests
#
oss_garp_SOURCES = apps/oss_garp.cpp

#
# oss_carp - tool for Common Address Redundancy Protocol
#
#oss_carp_LDFLAGS = -rdynamic
oss_carp_SOURCES = apps/oss_carp.cpp

#
# oss_rest - REST Server implementation
#
oss_rest_SOURCES = apps/oss_rest.cpp
