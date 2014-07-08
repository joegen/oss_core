SUBDIRS = .

corelibs = liboss_carp.la
corelibldadd = @OSS_CORE_DEP_LIBS@ liboss_core.la liboss_carp.la
if ENABLE_TURN
    corelibs += liboss_turn.la
    corelibldadd += liboss_turn.la
endif
corelibs += liboss_core.la


lib_LTLIBRARIES = $(corelibs)

localedir = $(datadir)/locale

AM_CPPFLAGS = \
    -I@OSS_CORE_SRCDIR@/include \
    -I@OSS_CORE_SRCDIR@/src/net/turn/carp \
    -I@OSS_CORE_SRCDIR@/src/net/turn \
    -I@OSS_CORE_SRCDIR@/src/net/turn/client \
    -I@OSS_CORE_SRCDIR@/src/net/turn/client++ \
    -I@OSS_CORE_SRCDIR@/src/net/turn/server \
    -I@OSS_CORE_SRCDIR@/src/net/turn/apps/common \
    -I@OSS_CORE_SRCDIR@/src/net/turn/apps/relay

version_Current=@OSS_CORE_VERSION_CURRENT@
version_Revision=@OSS_CORE_VERSION_REVISION@
version_Age=@OSS_CORE_VERSION_AGE@

LDADD = $(corelibldadd)

LDFLAGS = -rdynamic

liboss_carp_la_LDFLAGS = -version-info @OSS_CORE_VERSION_INFO@

liboss_carp_la_LIBADD = $(LTLIBINTL)

liboss_carp_la_CFLAGS = -DLOCALEDIR=\"$(localedir)\"

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



CHIMERA_SOURCES = \
	p2p/chimera/base.h \
	p2p/chimera/chimera.h \
	p2p/chimera/dllist.h \
	p2p/chimera/dtime.h \
	p2p/chimera/host.h \
	p2p/chimera/include.h \
	p2p/chimera/job_queue.h \
	p2p/chimera/jrb.h \
	p2p/chimera/jval.h \
	p2p/chimera/key.h \
	p2p/chimera/log.h \
	p2p/chimera/message.h \
	p2p/chimera/network.h \
	p2p/chimera/priqueue.h \
	p2p/chimera/route.h \
	p2p/chimera/semaphore.h \
	p2p/chimera/chimera.c \
	p2p/chimera/network.c \
	p2p/chimera/message.c \
	p2p/chimera/log.c \
	p2p/chimera/jrb.c \
	p2p/chimera/jval.c \
	p2p/chimera/key.c \
	p2p/chimera/route.c \
	p2p/chimera/host.c \
	p2p/chimera/dllist.c \
	p2p/chimera/job_queue.c \
	p2p/chimera/dtime.c \
	p2p/chimera/semaphore.c

UDT4_SOURCES = \
	p2p/udt/api.cpp \
	p2p/udt/api.h \
	p2p/udt/buffer.cpp \
	p2p/udt/buffer.h \
	p2p/udt/cache.cpp \
	p2p/udt/cache.h \
	p2p/udt/ccc.cpp \
	p2p/udt/ccc.h \
	p2p/udt/channel.cpp \
	p2p/udt/channel.h \
	p2p/udt/common.cpp \
	p2p/udt/common.h \
	p2p/udt/core.cpp \
	p2p/udt/core.h \
	p2p/udt/epoll.cpp \
	p2p/udt/epoll.h \
	p2p/udt/list.cpp \
	p2p/udt/list.h \
	p2p/udt/md5.h \
	p2p/udt/packet.cpp \
	p2p/udt/packet.h \
	p2p/udt/queue.cpp \
	p2p/udt/queue.h \
	p2p/udt/udt.h \
	p2p/udt/window.cpp \
	p2p/udt/window.h



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
    boost_pch.hpp \
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
    exec/ManagedDaemon.cpp \
    exec/ManagedDaemonRunner.cpp \
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
    persistent/unqlite/unqlite.h \
    persistent/unqlite/unqlite.c \
    persistent/KVUnqlite.cpp \
    persistent/KVLevelDB.cpp \
    persistent/RESTKeyValueStore.cpp \
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
    sipfsm/SIPTransportSession.cpp \
    sipfsm/SIPFsm.cpp \
    sipfsm/SIPNict.cpp \
    sipfsm/SIPTransaction.cpp \
    stun/STUNProto.cpp \
    stun/STUNMappedAddress.cpp \
    stun/STUNServer.cpp \
    stun/STUNClient.cpp \
    ua/SIPSessionState.cpp \
    ua/SIPSession.cpp \
    ua/SIPSessionManager.cpp \
    b2bua/SIPB2BClientTransaction.cpp \
    b2bua/SIPB2BTransaction.cpp \
    b2bua/SIPB2BScriptableHandler.cpp \
    b2bua/SIPB2BTransactionManager.cpp \
    b2bua/SIPB2BDialogStateManager.cpp \
    b2bua/SIPB2BContact.cpp \
    b2bua/SIPB2BUserAgentHandlerList.cpp \
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
    net/HTTPServer.cpp \
    net/AccessControl.cpp \
    net/TLSManager.cpp \
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
    ws/src/uri.cpp \
    ws/src/common.hpp \
    ws/src/connection.hpp \
    ws/src/endpoint.hpp \
    ws/src/network_utilities.hpp \
    ws/src/shared_const_buffer.hpp \
    ws/src/uri.hpp \
    ws/src/websocket_frame.hpp \
    ws/src/websocketpp.hpp \
    ws/src/base64/base64.h \
    ws/src/http/parser.hpp \
    ws/src/http/constants.hpp \
    ws/src/logger/logger.hpp \
    ws/src/md5/md5.h \
    ws/src/md5/md5.hpp \
    ws/src/messages/control.hpp \
    ws/src/messages/data.hpp \
    ws/src/processors/hybi.hpp \
    ws/src/processors/hybi_header.hpp \
    ws/src/processors/hybi_legacy.hpp \
    ws/src/processors/hybi_util.hpp \
    ws/src/processors/processor.hpp \
    ws/src/rng/blank_rng.hpp \
    ws/src/rng/boost_rng.hpp \
    ws/src/roles/client.hpp \
    ws/src/roles/server.hpp \
    ws/src/sha1/sha1.h \
    ws/src/sockets/plain.hpp \
    ws/src/sockets/socket_base.hpp \
    ws/src/sockets/tls.hpp \
    ws/src/utf8_validator/utf8_validator.hpp \
    ${CHIMERA_SOURCES} \
    ${UDT4_SOURCES} \
    p2p/Overlay.cpp \
    p2p/DhtStore.cpp

if ENABLE_TURN
    liboss_core_la_SOURCES += net/TurnServer.cpp
endif

bin_PROGRAMS = \
    oss_core_unit_test \
    oss_garp \
    oss_carp \
    oss_core \
    oss_rest

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
	unit_test/TestFirewall.cpp \
	unit_test/TestKeyValueStore.cpp \
	unit_test/TestOverlay.cpp \
	unit_test/TestAccessControl.cpp
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

#
# oss_rest - REST Server implementation
#
oss_rest_CXXFLAGS = ${common_cxx_flags} -fno-gcse -O0 -fno-implement-inlines -fno-default-inline
oss_rest_SOURCES = apps/oss_rest.cpp


DISTCLEANFILES = Makefile.in

#
# Build rule for Boost precompiled header
#
BUILT_SOURCES = boost_pch.hpp.gch
%.hpp.gch: %.hpp
	$(CXX) -D_REENTRANT -D_FILE_OFFSET_BITS=64 -fno-gcse -O0 -fno-implement-inlines -g -O2 -fmessage-length=0 -Wall -Wformat -Wwrite-strings -Wpointer-arith -fPIC -DPIC -c $<
