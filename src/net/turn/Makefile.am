if ENABLE_FEATURE_TURN

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