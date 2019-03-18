"use-strict";

var _hook = null;
_hook = require("sbc-hook/_sbc_hook");
__copy_exports(_hook, exports);

var _parser = require("sip-parser");

exports.onEvent = function(arg, userData, result) {
  return TRN_ACCEPT;
}

//
// global sip variables
//
exports.sbc_create_global_variables = function(sipMessage) {
  var global = new Object();

  global.sbc_retarget_refer = "";
  global.sbc_dialog_state_placement = "";
  global.sbc_log_id = sipMessage.getLogId();
  global.sbc_disable_rtp_proxy = false;
  global.sbc_verbose_rtp_logging = false;
  global.sbc_local_100_rel = false;
  global.sbc_disable_181_relay = false;
  global.sbc_local_update = false;
  global.sbc_reject_code = null;
  global.sbc_reject_reason = null;
  global.sbc_ban_source_address = false;
  global.sbc_patch_ipv6_as_rfc1918_address = false;
  global.sbc_force_sdp_global_ip = null;
  global.sbc_force_sdp_audio_ip = null;
  global.sbc_force_sdp_audio_port = null;
  global.sbc_force_sdp_video_ip = null;
  global.sbc_force_sdp_video_port = null;
  global.sbc_sdp_audio_attributes = null;
  global.sbc_sdp_video_attributes = null;
  global.sbc_sdp_remove_audio_attributes = null;
  global.sbc_sdp_remove_video_attributes = null;
  global.sbc_bleg_cid = null;
  global.sbc_bleg_cid_suffix = null;

  if (typeof sbc_log_id === "undefined" || sbc_log_id.length === 0) {
    global.sbc_log_id = "[CID=00000000]\t";
  }

  //
  // Transcoder
  //
  global.sbc_enable_transcoding = false;
  global.sbc_offer_secure_rtp = false;
  global.sbc_target_behind_nat = false;
  global.sbc_caller_behind_nat = false;
  global.sbc_send_ptime_attribute = null;
  global.sbc_send_ssrc_attribute = null;
  global.sbc_inbound_codecs = [];
  global.sbc_outbound_codecs = [];

  global.sip_message = sipMessage;
  var request = sipMessage._request;

  //
  // Request URI
  //
  global.sip_raw_uri = _parser.msgGetRequestUri(request);
  global.sip_method = _parser.msgGetMethod(request);
  global.sip_target_user = _parser.msgGetRequestUriUser(request);
  global.sip_target_host = _parser.msgGetRequestUriHost(request);
  global.sip_target_port = _parser.msgGetRequestUriPort(request);
  global.sip_uri_parameters = _parser.msgGetRequestUriParameters(request);

  global.sip_target_user_value = null;
  global.sip_target_host_value = null;
  global.sip_target_port_value = null;
  global.sip_uri_parameters_value = null;
  global.sip_custom_headers_value = {};
  global.sip_local_reg_target_value = {
    user: "",
    host: "",
    port: ""
  };

  //
  // User-Agent Header
  //
  global.sip_remote_user_agent = _parser.msgHdrGet(request, "User-Agent");

  //
  // From Header
  //
  global.sip_from_raw_header = _parser.msgHdrGet(request, "From");
  global.sip_from_user = _parser.msgGetFromUser(request);
  global.sip_from_host = _parser.msgGetFromHost(request);
  global.sip_from_port = _parser.msgGetFromPort(request);
  global.sip_from_display_name = _parser.fromGetDisplayName(global.sip_from_raw_header);

  global.sip_from_user_value = null;
  global.sip_from_host_value = null;
  global.sip_from_port_value = null;
  global.sip_from_display_name_value = null;

  //
  // To Header
  //
  global.sip_to_raw_header = _parser.msgHdrGet(request, "To");
  global.sip_to_user = _parser.msgGetToUser(request);
  global.sip_to_host = _parser.msgGetToHost(request);
  global.sip_to_port = _parser.msgGetToPort(request);
  global.sip_to_display_name = _parser.toGetDisplayName(global.sip_to_raw_header);

  global.sip_to_user_value = null;
  global.sip_to_host_value = null;
  global.sip_to_port_value = null;
  global.sip_to_display_name_value = null;

  //
  // Transport
  //
  global.sip_source_address = sipMessage.getSourceAddress();
  global.sip_source_port = sipMessage.getSourcePort();
  global.sip_interface_address = sipMessage.getInterfaceAddress();
  global.sip_interface_port = sipMessage.getInterfacePort();

  global.sip_target_address_value = {
    protocol: "",
    address: "",
    port: ""
  };
  global.sip_local_interface_value = {
    address: "",
    port: ""
  };
  global.sip_target_domain_value = "";

  //
  // Assume rejection.  Modules will set this to accept or banned
  //
  global.sbc_route_action = "reject";

  return global;
}

exports.sbc_export_global_vars = function(sipMessage, global) {
  var request = sipMessage._request;

  if (global.sbc_retarget_refer.length > 0) {
    sipMessage.setProperty("retarget-refer", global.sbc_retarget_refer);
  }

  if (global.sbc_dialog_state_placement === DIALOG_STATE_IN_CONTACT) {
    sipMessage.setTransactionProperty("dialog-state-placement", DIALOG_STATE_IN_CONTACT);
  } else if (global.sbc_dialog_state_placement === DIALOG_STATE_IN_ROUTE) {
    sipMessage.setTransactionProperty("dialog-state-placement", DIALOG_STATE_IN_ROUTE);
  }

  if (global.sbc_disable_rtp_proxy) {
    sipMessage.setTransactionProperty("no-rtp-proxy", "1");
  }

  if (global.sbc_verbose_rtp_logging) {
    sipMessage.setTransactionProperty("enable-verbose-rtp", "1");
  }

  if (global.sbc_local_100_rel) {
    sipMessage.setTransactionProperty("local-100-rel", "1");
  }

  if (global.sbc_disable_181_relay) {
    sipMessage.setTransactionProperty("disable-181-relay", "1");
  }

  if (global.sbc_local_update) {
    sipMessage.setTransactionProperty("local-update", "1");
  }

  if (global.sbc_reject_code !== null) {
    sipMessage.setProperty("reject-code", global.sbc_reject_code);
  }

  if (global.sbc_reject_reason !== null) {
    sipMessage.setProperty("reject-reason", global.sbc_reject_reason);
  }

  if (global.sbc_ban_source_address) {
    global.sbc_route_action = "banned";
  }

  if (global.sbc_route_action) {
    sipMessage.setProperty("route-action", global.sbc_route_action);
  }

  if (global.sbc_patch_ipv6_as_rfc1918_address) {
    sipMessage.setTransactionProperty("patch-ipv6_as-rfc1918-address", "1");
  }

  if (global.sbc_force_sdp_audio_ip !== null) {
    sipMessage.setTransactionProperty("force-sdp-audio-ip", global.sbc_force_sdp_audio_ip);
  }

  if (global.sbc_force_sdp_audio_port !== null) {
    sipMessage.setTransactionProperty("force-sdp-audio-port", global.sbc_force_sdp_audio_port);
  }

  if (global.sbc_force_sdp_video_ip !== null) {
    sipMessage.setTransactionProperty("force-sdp-video-ip", global.sbc_force_sdp_video_ip);
  }

  if (global.sbc_force_sdp_video_port !== null) {
    sipMessage.setTransactionProperty("force-sdp-video-port", global.sbc_force_sdp_video_port);
  }

  if (global.sbc_force_sdp_global_ip !== null) {
    sipMessage.setTransactionProperty("force-sdp-global-ip", global.sbc_force_sdp_global_ip);
  }

  var attributes = "";
  var i = 0;
  if (global.sbc_sdp_audio_attributes !== null && global.sbc_sdp_audio_attributes.length) {
    for (i = 0; i < global.sbc_sdp_audio_attributes.length; i++) {
      attributes += global.sbc_sdp_audio_attributes[i];
      if (i < global.sbc_sdp_audio_attributes.length - 1) {
        attributes += "~";
      }
    }

    sipMessage.setTransactionProperty("sdp-audio-attributes", attributes);
  }

  if (global.sbc_sdp_video_attributes !== null && global.sbc_sdp_video_attributes.length) {
    attributes = "";
    i = 0;
    for (i = 0; i < global.sbc_sdp_video_attributes.length; i++) {
      attributes += global.sbc_sdp_video_attributes[i];
      if (i < global.sbc_sdp_video_attributes.length - 1) {
        attributes += "~";
      }
    }
    sipMessage.setTransactionProperty("sdp-video-attributes", attributes);
  }

  if (global.sbc_sdp_remove_audio_attributes !== null && global.sbc_sdp_remove_audio_attributes.length) {
    attributes = "";
    i = 0;
    for (i = 0; i < global.sbc_sdp_remove_audio_attributes.length; i++) {
      attributes += global.sbc_sdp_remove_audio_attributes[i];
      if (i < global.sbc_sdp_remove_audio_attributes.length - 1) {
        attributes += "~";
      }
    }

    sipMessage.setTransactionProperty("sdp-remove-audio-attributes", attributes);
  }

  if (global.sbc_sdp_remove_video_attributes !== null && global.sbc_sdp_remove_video_attributes.length) {
    attributes = "";
    i = 0;
    for (i = 0; i < global.sbc_sdp_remove_video_attributes.length; i++) {
      attributes += global.sbc_sdp_remove_video_attributes[i];
      if (i < global.sbc_sdp_remove_video_attributes.length - 1) {
        attributes += "~";
      }
    }
    sipMessage.setTransactionProperty("sdp-remove-video-attributes", attributes);
  }

  if (sipMessage.isRequest("INVITE")) {
    if (global.sbc_bleg_cid !== null && sbc_bleg_cid === B2B) {
      sipMessage.setTransactionProperty("cid-correlation", B2B);
      if (global.sbc_bleg_cid_suffix === null) {
        global.sbc_bleg_cid_suffix = "-leg2";
      }
      sipMessage.setTransactionProperty("cid-suffix", global.sbc_bleg_cid_suffix);
    } else if (global.sbc_bleg_cid !== null && global.sbc_bleg_cid === XCID) {
      sipMessage.setTransactionProperty("cid-correlation", XCID);
    }
  }


  //
  // Request URI
  //

  if (global.sip_local_reg_target_value.user.length > 0 && global.sip_local_reg_target_value.host.length > 0) {
    global.sip_target_user_value = global.sip_local_reg_target_value.user;
    global.sip_target_host_value = global.sip_local_reg_target_value.host;
    global.sip_target_port_value = global.sip_local_reg_target_value.port;
    sipMessage.setProperty("local-reg-retarget", "1");
  }

  if (global.sip_target_user_value !== null) {
    _parser.msgSetRequestUriUser(request, global.sip_target_user_value);
  }
  if (global.sip_target_host_value !== null) {
    if (global.sip_target_port_value !== null) {
      if (global.sip_target_port_value.length === 0) {
        _parser.msgSetRequestUriHostPort(request, global.sip_target_host_value);
      } else {
        _parser.msgSetRequestUriHostPort(request, global.sip_target_host_value + ":" + global.sip_target_port_value);
      }
    } else if (typeof global.sip_target_port === "string") {
      _parser.msgSetRequestUriHostPort(request, global.sip_target_host_value + ":" + global.sip_target_port);
    } else {
      _parser.msgSetRequestUriHostPort(request, global.sip_target_host_value);
    }
  }
  if (global.sip_target_port_value !== null && global.sip_target_host_value === null) {
    if (global.sip_target_port_value.length === 0) {
      _parser.msgSetRequestUriHostPort(request, global.sip_target_host);
    } else {
      _parser.msgSetRequestUriHostPort(request, global.sip_target_host + ":" + global.sip_target_port_value);
    }
  }
  if (global.sip_uri_parameters_value !== null) {
    _parser.msgSetRequestUriParameters(request, global.sip_uri_parameters_value);
  }

  //
  // FROM Header
  //
  if (global.sip_from_display_name_value !== null) {
    var from = _parser.msgHdrGet(request, "From");
    from = _parser.fromSetDisplayName(from, global.sip_from_display_name_value);
    _parser.msgHdrSet(request, "From", from);
  }
  if (global.sip_from_user_value !== null) {
    _parser.msgSetFromUser(request, global.sip_from_user_value)
  }
  if (global.sip_from_host_value !== null) {
    if (global.sip_from_port_value !== null) {
      if (global.sip_from_port_value.length === 0) {
        _parser.msgSetFromHostPort(request, global.sip_from_host_value);
      } else {
        _parser.msgSetFromHostPort(request, global.sip_from_host_value + ":" + global.sip_from_port_value);
      }
    } else if (typeof global.sip_from_port === "string") {
      _parser.msgSetFromHostPort(request, global.sip_from_host_value + ":" + global.sip_from_port);
    } else {
      _parser.msgSetFromHostPort(request, global.sip_from_host_value);
    }
  }
  if (global.sip_from_port_value !== null && global.sip_from_host_value === null) {
    if (global.sip_from_port_value.length === 0) {
      _parser.msgSetFromHostPort(request, global.sip_from_host);
    } else {
      _parser.msgSetFromHostPort(request, global.sip_from_host + ":" + global.sip_from_port_value);
    }
  }


  //
  // TO Header
  //
  if (global.sip_to_display_name_value !== null) {
    var to = _parser.msgHdrGet(request, "To");
    to = _parser.toSetDisplayName(to, global.sip_to_display_name_value);
    _parser.msgHdrSet(request, "To", to);
  }
  if (global.sip_to_user_value !== null) {
    _parser.msgSetToUser(request, global.sip_to_user_value)
  }
  if (global.sip_to_host_value !== null) {
    if (global.sip_to_port_value !== null) {
      if (global.sip_to_port_value.length === 0) {
        _parser.msgSetToHostPort(request, global.sip_to_host_value);
      } else {
        _parser.msgSetToHostPort(request, global.sip_to_host_value + ":" + global.sip_to_port_value);
      }
    } else if (typeof global.sip_to_port === "string") {
      _parser.msgSetToHostPort(request, global.sip_to_host_value + ":" + global.sip_to_port);
    } else {
      _parser.msgSetToHostPort(request, global.sip_to_host_value);
    }
  }
  if (global.sip_to_port_value !== null && global.sip_to_host_value === null) {
    if (global.sip_to_port_value.length === 0) {
      _parser.msgSetToHostPort(request, global.sip_to_host);
    } else {
      _parser.msgSetToHostPort(request, global.sip_to_host + ":" + global.sip_to_port_value);
    }
  }

  //
  // Transport
  //
  if (global.sip_target_domain_value.length > 0) {
    sipMessage.setRequestUriHostPort(global.sip_target_domain_value);
    sipMessage.setFromHostPort(global.sip_target_domain_value);
    sipMessage.setToHostPort(global.sip_target_domain_value);
  }

  if (global.sip_target_address_value.address.length > 0) {
    if (global.sip_target_address_value.protocol.length === 0) {
      global.sip_target_address_value.protocol = "udp";
    }
    sipMessage.setProperty("target-transport", global.sip_target_address_value.protocol);
    sipMessage.setProperty("target-address", global.sip_target_address_value.address);

    if (global.sip_target_address_value.port.length > 0) {
      sipMessage.setProperty("target-port", global.sip_target_address_value.port);
    }
  }

  if (global.sip_local_interface_value.address.length > 0) {
    sipMessage.setProperty("interface-address", global.sip_local_interface_value.address);
    if (global.sip_local_interface_value.port.length > 0) {
      sipMessage.setProperty("interface-port", global.sip_local_interface_value.port);
    }
  }
}