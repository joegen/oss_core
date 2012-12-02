SUBDIRS = .

lib_LTLIBRARIES = liboss_core.la

INCLUDES = -I$(top_srcdir)/include

## Libtool version info - update only immediately before a release
# Current - increment if any interfaces are added, removed, or changed
version_Current=1
# Revision - set to 0 if Current was incremented
#            increment if any implementation has changed
version_Revision=6
# Age - set to 0 of any interfaces have been removed
#       increment if any interfaces have been added
version_Age=0

liboss_core_la_LDFLAGS = \
	-version-info ${version_Current}:${version_Revision}:${version_Age}

dep_libs = \
	-lboost_date_time-mt \
	-lboost_filesystem-mt \
	-lboost_system-mt \
	-lboost_thread-mt \
	-lboost_regex-mt \
	-lboost_program_options-mt \
	-lboost_iostreams-mt \
	-lPocoFoundation \
	-lPocoUtil \
	-lPocoNet \
	-lPocoXML \
	-lhiredis \
	-lconfig++ \
	-lv8 \
	-lltdl \
	-lmcrypt \
	-lssl \
	-lcrypto \
	-lpthread \
	-ldl \
	-lrt \
	-lcrypt \
	-lssl \
	-lresolv

liboss_core_la_LIBADD = \
	${dep_libs}

# this is helpful if you're debugging something with lots of inlines (like the UtlLink code)
# 		-fno-gcse -O0 -fno-implement-inlines -fno-default-inline
liboss_core_la_CXXFLAGS = \
	-DOSSLIBS_TMPDIR=\"@OSSLIBS_TMPDIR@\" \
	-DOSSLIBS_CONFDIR=\"@OSSLIBS_CONFDIR@\" \
	-DOSSAPP_LIB_VERSION=${version_Current}.${version_Revision}.${version_Age} \
	-fno-gcse -O0 -fno-implement-inlines -fno-default-inline

liboss_core_la_CFLAGS = 	-std=c99

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
    b2bua/SIPB2BContact.cpp \
    js/JSBase.cpp \
    js/JSSIPMessage.cpp \
    rtp/RTPPacket.cpp

bin_PROGRAMS = oss_core_unit_test

oss_core_unit_test_CXXFLAGS = \
	${common_cxx_flags} \
	-fno-gcse -O0 -fno-implement-inlines -fno-default-inline

oss_core_unit_test_LDFLAGS = \
	-rdynamic \
	${dep_libs} \
	-lgtest

oss_core_unit_test_LDADD = \
	liboss_core.la \
	${common_libs}

oss_core_unit_test_SOURCES = \
	unit_test/TestSuite.cpp \
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
	unit_test/TestRTPPacket.cpp



DISTCLEANFILES = Makefile.in
