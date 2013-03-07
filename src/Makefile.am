SUBDIRS = .

corelibs = liboss_carp.la
corelibldadd = @OSS_CORE_DEP_LIBS@ liboss_core.la liboss_carp.la
if ENABLE_TURN
    corelibs += liboss_turn.la
    corelibldadd += liboss_turn.la
endif
corelibs += liboss_core.la


lib_LTLIBRARIES = $(corelibs)

localedir = $(datadir)/locale -DLOCALEDIR=\"$(localedir)\"

INCLUDES = \
    -I$(top_srcdir)/include \
    -I$(top_srcdir)/src/net/turn \
    -I$(top_srcdir)/src/net/turn/carp \
    -I$(top_srcdir)/src/net/turn/client \
    -I$(top_srcdir)/src/net/turn/client++ \
    -I$(top_srcdir)/src/net/turn/server \
    -I$(top_srcdir)/src/net/turn/apps/common \
    -I$(top_srcdir)/src/net/turn/apps/relay

version_Current=@OSS_CORE_VERSION_CURRENT@
version_Revision=@OSS_CORE_VERSION_REVISION@
version_Age=@OSS_CORE_VERSION_AGE@

LDADD = $(corelibldadd)

LDFLAGS = -rdynamic

liboss_carp_la_LDFLAGS = -version-info @OSS_CORE_VERSION_INFO@

liboss_carp_la_LIBADD = $(LTLIBINTL)

liboss_carp_la_SOURCES = \
	net/carp/gettext.h \
	net/carp/ucarp.c \
	net/carp/ucarp.h \
	net/carp/carp.c \
	net/carp/carp_p.h \
	net/carp/ucarp_p.h \
	net/carp/ip_carp.h \
	net/carp/crypto-sha1.c \
	net/carp/crypto.h \
	net/carp/crypto-sha1.h \
	net/carp/globals.h \
	net/carp/fillmac.c \
	net/carp/fillmac.h \
	net/carp/bsd-getopt_long.h \
	net/carp/bsd-getopt_long.c \
	net/carp/garp.c \
	net/carp/garp.h \
	net/carp/spawn.c \
	net/carp/spawn.h \
	net/carp/fakesnprintf.c \
	net/carp/fakesnprintf.h \
	net/carp/mysnprintf.c \
	net/carp/mysnprintf.h \
	net/carp/log.c \
	net/carp/log.h \
	net/carp/syslognames.h

if ENABLE_TURN

    liboss_turn_la_LDFLAGS = -version-info @OSS_CORE_VERSION_INFO@
    liboss_turn_la_LIBADD = @OSS_CORE_DEP_LIBS@
    liboss_turn_la_CPPFLAGS = -D__USE_OPENSSL__

    liboss_turn_la_SOURCES = \
        net/turn/client++/TurnMsgLib.h \
        net/turn/client/ns_turn_msg_addr.c \
        net/turn/client/ns_turn_utils.c \
        net/turn/client/ns_turn_msg_defs.h \
        net/turn/client/ns_turn_ioaddr.h \
        net/turn/client/ns_turn_msg.h \
        net/turn/client/ns_turn_msg_addr.h \
        net/turn/client/ns_turn_ioaddr.c \
        net/turn/client/ns_turn_utils.h \
        net/turn/client/ns_turn_msg.c \
        net/turn/server/ns_turn_allocation.h \
        net/turn/server/ns_turn_khash.h \
        net/turn/server/ns_turn_ioalib.h \
        net/turn/server/ns_turn_server.c \
        net/turn/server/ns_turn_allocation.c \
        net/turn/server/ns_turn_maps_rtcp.h \
        net/turn/server/ns_turn_maps_rtcp.c \
        net/turn/server/ns_turn_maps.c \
        net/turn/server/ns_turn_session.h \
        net/turn/server/ns_turn_server.h \
        net/turn/server/ns_turn_maps.h \
        net/turn/ns_turn_defs.h \
        net/turn/apps/relay/dtls_listener.c \
        net/turn/apps/relay/ns_ioalib_impl.h \
        net/turn/apps/relay/turn_ports.h \
        net/turn/apps/relay/tcp_listener.c \
        net/turn/apps/relay/userdb.c \
        net/turn/apps/relay/udp_listener.c \
        net/turn/apps/relay/udp_listener.h \
        net/turn/apps/relay/tcp_listener.h \
        net/turn/apps/relay/dtls_listener.h \
        net/turn/apps/relay/tls_listener.h \
        net/turn/apps/relay/mainrelay.c \
        net/turn/apps/relay/mainrelay.h \
        net/turn/apps/relay/userdb.h \
        net/turn/apps/relay/turn_ports.c \
        net/turn/apps/relay/ns_ioalib_engine_impl.c \
        net/turn/apps/relay/tls_listener.c \
        net/turn/apps/relay/turnmutex.c \
        net/turn/apps/common/apputils.c \
        net/turn/apps/common/stun_buffer.h \
        net/turn/apps/common/stun_buffer.c \
        net/turn/apps/common/apputils.h
endif


liboss_core_la_LDFLAGS = \
	-version-info @OSS_CORE_VERSION_INFO@

liboss_core_la_LIBADD = @OSS_CORE_DEP_LIBS@

# this is helpful if you're debugging something with lots of inlines (like the UtlLink code)
# 		-fno-gcse -O0 -fno-implement-inlines -fno-default-inline
liboss_core_la_CXXFLAGS = 
	-DOSSLIBS_TMPDIR=\"@OSSLIBS_TMPDIR@\" \
	-DOSSLIBS_CONFDIR=\"@OSSLIBS_CONFDIR@\" \
	-DOSSAPP_LIB_VERSION=\"@OSS_CORE_VERSION_FULL@\" \
	-fno-gcse -O0 -fno-implement-inlines -fno-default-inline 

liboss_core_la_SOURCES =  \
    DNS.cpp \
    Endian.cpp \
    Application.cpp \
    Exception.cpp \
    Core.cpp \
    Logger.cpp \
    Crypto.cpp \
    ServiceDaemon.cpp \
    Cache.cpp \
    Compress.cpp \
    OSS.cpp \
    DynamicHashTable.cpp \
    DebugMemory.cpp \
    IPAddress.cpp \
    Net.cpp \
    Thread.cpp \
    StackTrace.cpp \
    exec/Command.cpp \
    exec/Process.cpp \
    exec/stringtoargcargv.cpp \
    abnf/ABNFSIPURIParameter.cpp \
    abnf/ABNFSIPScheme.cpp \
    abnf/ABNFMethodParam.cpp \
    abnf/ABNFSIPOpaquePart.cpp \
    abnf/ABNFSIPHostName.cpp \
    abnf/ABNFSIPSrvr.cpp \
    abnf/ABNFSIPUser.cpp \
    abnf/ABNFSIPPort.cpp \
    abnf/ABNFSIPReasonPhrase.cpp \
    abnf/ABNFSIPURIParameters.cpp \
    abnf/ABNFSIPHex4.cpp \
    abnf/ABNFSIPRegName.cpp \
    abnf/ABNFSIPURI.cpp \
    abnf/ABNFSIPContactParam.cpp \
    abnf/ABNFSIPHexPart.cpp \
    abnf/ABNFSIPGenericParams.cpp \
    abnf/ABNFSIPIPV6Address.cpp \
    abnf/ABNFSIPCSeq.cpp \
    abnf/ABNFSIPQdText.cpp \
    abnf/ABNFSIPStatusLine.cpp \
    abnf/ABNFSIPAbsoluteURI.cpp \
    abnf/ABNFSIPStatusCode.cpp \
    abnf/ABNFSIPTextUtf8Char.cpp \
    abnf/ABNFSIPHost.cpp \
    abnf/ABNFParser.cpp \
    abnf/ABNFSIPQuotedPair.cpp \
    abnf/ABNFSIPRequestLine.cpp \
    abnf/ABNFSIPHostPort.cpp \
    abnf/ABNFSIPUserInfo.cpp \
    abnf/ABNFSIPFromSpec.cpp \
    abnf/ABNFSIPQuotedString.cpp \
    abnf/ABNFSIPVersion.cpp \
    abnf/ABNFSIPNetPath.cpp \
    abnf/ABNFSIPUtf8NonAscii.cpp \
    abnf/ABNFSIPTransportParam.cpp \
    abnf/ABNFSIPPassword.cpp \
    abnf/ABNFSIPToken.cpp \
    abnf/ABNFSIPURIHeaders.cpp \
    abnf/ABNFSIPAuthority.cpp \
    abnf/ABNFSIPSegment.cpp \
    abnf/ABNFSIPMethod.cpp \
    abnf/ABNFSIPURIHeader.cpp \
    abnf/ABNFSIPNameAddr.cpp \
    abnf/ABNFSIPHexSeq.cpp \
    abnf/ABNFSIPIPV4Address.cpp \
    abnf/ABNFSIPDisplayName.cpp \
    abnf/ABNFSIPTopLabel.cpp \
    abnf/ABNFSIPPathSegments.cpp \
    abnf/ABNFSIPTextUtf8Trim.cpp \
    abnf/ABNFSIPUserParam.cpp \
    abnf/ABNFSIPDomainLabel.cpp \
    abnf/ABNFSIPRequestURI.cpp \
    abnf/ABNFSIPGenericParam.cpp \
    abnf/ABNFSIPHierPart.cpp \
    crypto/BlockCipher.cpp \
    crypto/FileEncryptor.cpp \
    persistent/ClassType.cpp \
    persistent/DataType.cpp \
    persistent/Reference.cpp \
    sdp/SDPHeader.cpp \
    sdp/SDPHeaderList.cpp \
    sdp/SDPMedia.cpp \
    sdp/SDPSession.cpp \
    sipparser/SIPAuthorization.cpp \
    sipparser/SIPContact.cpp \
    sipparser/SIPCSeq.cpp \
    sipparser/SIPDigestAuth.cpp \
    sipparser/SIPFrom.cpp \
    sipparser/SIPHeaderTokens.cpp \
    sipparser/SIPMessage.cpp \
    sipparser/SIPParser.cpp \
    sipparser/SIPProxyAuthenticate.cpp \
    sipparser/SIPRequestLine.cpp \
    sipparser/SIPRoute.cpp \
    sipparser/SIPStatusLine.cpp \
    sipparser/SIPURI.cpp \
    sipparser/SIPVia.cpp \
    sipfsm/SIPTCPConnectionManager.cpp \
    sipfsm/SIPTransportService.cpp \
    sipfsm/SIPTLSConnection.cpp \
    sipfsm/SIPNist.cpp \
    sipfsm/SIPTransactionTimers.cpp \
    sipfsm/SIPUDPListener.cpp \
    sipfsm/SIPXOR.cpp \
    sipfsm/SIPIst.cpp \
    sipfsm/SIPIstPool.cpp \
    sipfsm/SIPTLSListener.cpp \
    sipfsm/SIPIctPool.cpp \
    sipfsm/SIPTransactionPool.cpp \
    sipfsm/SIPUDPConnection.cpp \
    sipfsm/SIPTCPListener.cpp \
    sipfsm/SIPIct.cpp \
    sipfsm/SIPTCPConnection.cpp \
    sipfsm/SIPUDPConnectionClone.cpp \
    sipfsm/SIPNictPool.cpp \
    sipfsm/SIPNistPool.cpp \
    sipfsm/SIPFSMDispatch.cpp \
    sipfsm/SIPTLSConnectionManager.cpp \
    sipfsm/OSSSIP.cpp \
    sipfsm/SIPListener.cpp \
    sipfsm/SIPTransportRateLimitStrategy.cpp \
    sipfsm/SIPTransportSession.cpp \
    sipfsm/SIPFsm.cpp \
    sipfsm/SIPNict.cpp \
    sipfsm/SIPTransaction.cpp \
    stun/STUNProto.cpp \
    stun/STUNMappedAddress.cpp \
    stun/STUNServer.cpp \
    stun/STUNClient.cpp \
    b2bua/SIPB2BClientTransaction.cpp \
    b2bua/SIPB2BTransaction.cpp \
    b2bua/SIPB2BScriptableHandler.cpp \
    b2bua/SIPB2BTransactionManager.cpp \
    b2bua/SIPB2BDialogStateManager.cpp \
    b2bua/SIPB2BContact.cpp \
    js/JSBase.cpp \
    js/JSSIPMessage.cpp \
    rtp/RTPPacket.cpp \
    rtp/RTPPCAPReader.cpp \
    rtp/RTPProxy.cpp \
    rtp/RTPProxyClient.cpp \
    rtp/RTPProxyManager.cpp \
    rtp/RTPProxyRecord.cpp \
    rtp/RTPProxySession.cpp \
    rtp/RTPProxyTuple.cpp \
    rtp/RTPResizer.cpp \
    rtp/RTPResizingQueue.cpp \
    net/FirewallRule.cpp \
    net/Firewall.cpp \
    net/Carp.cpp \
    utl/AdaptiveDelay.cpp \
    sipfsm/SIPWebSocketConnection.cpp \
    sipfsm/SIPWebSocketListener.cpp \
    sipfsm/SIPWebSocketConnectionManager.cpp \
    ws/src/base64/base64.cpp \
    ws/src/md5/md5.c \
    ws/src/messages/data.cpp \
    ws/src/network_utilities.cpp \
    ws/src/processors/hybi_header.cpp \
    ws/src/processors/hybi_util.cpp \
    ws/src/sha1/sha1.cpp \
    ws/src/uri.cpp

if ENABLE_TURN
    liboss_core_la_SOURCES += net/TurnServer.cpp
endif

bin_PROGRAMS = \
    oss_core_unit_test \
    oss_garp \
    oss_carp \
    oss_core

oss_core_unit_test_CXXFLAGS = \
	${common_cxx_flags} \
	-fno-gcse -O0 -fno-implement-inlines -fno-default-inline


oss_core_unit_test_SOURCES = \
	unit_test/TestSuite.cpp \
        unit_test/TestSemaphore.cpp \
	unit_test/TestRequestLine.cpp \
	unit_test/TestBasicParser.cpp \
	unit_test/TestSDP.cpp \
	unit_test/TestCSeq.cpp \
	unit_test/TestCache.cpp \
	unit_test/TestFoundationAPI.cpp \
	unit_test/TestVia.cpp \
	unit_test/TestContact.cpp \
	unit_test/TestRouteHeader.cpp \
	unit_test/TestFrom.cpp \
	unit_test/TestPersistent.cpp \
	unit_test/TestDNS.cpp \
	unit_test/TestSIPURI.cpp \
	unit_test/TestRTPPacket.cpp \
	unit_test/TestFirewall.cpp
#        unit_test/TestWebSocket.cpp

#
# oss_core - A simple back-to-back user agent
#
oss_core_CXXFLAGS = ${common_cxx_flags} -fno-gcse -O0 -fno-implement-inlines -fno-default-inline
oss_core_SOURCES = apps/oss_core.cpp

#
# oss_garp - tool for sending gratuitous ARP requests
#
oss_garp_CXXFLAGS = ${common_cxx_flags} -fno-gcse -O0 -fno-implement-inlines -fno-default-inline
oss_garp_SOURCES = apps/oss_garp.cpp

#
# oss_carp - tool for Common Address Redundancy Protocol
#
oss_carp_CXXFLAGS = ${common_cxx_flags} -fno-gcse -O0 -fno-implement-inlines -fno-default-inline
#oss_carp_LDFLAGS = -rdynamic
oss_carp_SOURCES = apps/oss_carp.cpp


DISTCLEANFILES = Makefile.in
