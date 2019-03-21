"use-strict";

var isolate = require("isolate");
var logger = require("logger");
const SIPMessage = require("sip-parser").SIPMessage;

var _hook = null;
_hook = require("sbc-hook/_sbc_hook");
__copy_exports(_hook, exports);

var _parser = require("sip-parser");

exports.onEvent = function(arg, userData, result) {
  return TRN_ACCEPT;
}

exports._on_initialize_transport = null;
exports._on_initialize_user_agent = null;
exports._on_route_request = null;
exports._on_inbound_request = null;

//
// global sip variables
//
exports.sbc_create_context_variables = function(sipMessage) {
  var context = new Object();

  context.sipMessage = sipMessage;
  context.sbc_retarget_refer = "";
  context.sbc_dialog_state_placement = "";
  context.sbc_log_id = sipMessage.getLogId();
  context.sbc_disable_rtp_proxy = false;
  context.sbc_verbose_rtp_logging = false;
  context.sbc_local_100_rel = false;
  context.sbc_disable_181_relay = false;
  context.sbc_local_update = false;
  context.sbc_reject_code = null;
  context.sbc_reject_reason = null;
  context.sbc_ban_source_address = false;
  context.sbc_patch_ipv6_as_rfc1918_address = false;
  context.sbc_force_sdp_global_ip = null;
  context.sbc_force_sdp_audio_ip = null;
  context.sbc_force_sdp_audio_port = null;
  context.sbc_force_sdp_video_ip = null;
  context.sbc_force_sdp_video_port = null;
  context.sbc_sdp_audio_attributes = null;
  context.sbc_sdp_video_attributes = null;
  context.sbc_sdp_remove_audio_attributes = null;
  context.sbc_sdp_remove_video_attributes = null;
  context.sbc_bleg_cid = null;
  context.sbc_bleg_cid_suffix = null;

  if (typeof sbc_log_id === "undefined" || sbc_log_id.length === 0) {
    context.sbc_log_id = "[CID=00000000]\t";
  }

  //
  // Transcoder
  //
  context.sbc_enable_transcoding = false;
  context.sbc_offer_secure_rtp = false;
  context.sbc_target_behind_nat = false;
  context.sbc_caller_behind_nat = false;
  context.sbc_send_ptime_attribute = null;
  context.sbc_send_ssrc_attribute = null;
  context.sbc_inbound_codecs = [];
  context.sbc_outbound_codecs = [];

  context.sip_message = sipMessage;
  var request = sipMessage._request;

  //
  // Request URI
  //
  context.sip_raw_uri = _parser.msgGetRequestUri(request);
  context.sip_method = _parser.msgGetMethod(request);
  context.sip_target_user = _parser.msgGetRequestUriUser(request);
  context.sip_target_host = _parser.msgGetRequestUriHost(request);
  context.sip_target_port = _parser.msgGetRequestUriPort(request);
  context.sip_uri_parameters = _parser.msgGetRequestUriParameters(request);

  context.sip_target_user_value = null;
  context.sip_target_host_value = null;
  context.sip_target_port_value = null;
  context.sip_uri_parameters_value = null;
  context.sip_custom_headers_value = {};
  context.sip_local_reg_target_value = {
    user: "",
    host: "",
    port: ""
  };

  //
  // User-Agent Header
  //
  context.sip_remote_user_agent = _parser.msgHdrGet(request, "User-Agent");

  //
  // From Header
  //
  context.sip_from_raw_header = _parser.msgHdrGet(request, "From");
  context.sip_from_user = _parser.msgGetFromUser(request);
  context.sip_from_host = _parser.msgGetFromHost(request);
  context.sip_from_port = _parser.msgGetFromPort(request);
  context.sip_from_display_name = _parser.fromGetDisplayName(context.sip_from_raw_header);

  context.sip_from_user_value = null;
  context.sip_from_host_value = null;
  context.sip_from_port_value = null;
  context.sip_from_display_name_value = null;

  //
  // To Header
  //
  context.sip_to_raw_header = _parser.msgHdrGet(request, "To");
  context.sip_to_user = _parser.msgGetToUser(request);
  context.sip_to_host = _parser.msgGetToHost(request);
  context.sip_to_port = _parser.msgGetToPort(request);
  context.sip_to_display_name = _parser.toGetDisplayName(context.sip_to_raw_header);

  context.sip_to_user_value = null;
  context.sip_to_host_value = null;
  context.sip_to_port_value = null;
  context.sip_to_display_name_value = null;

  //
  // Transport
  //
  context.sip_source_address = sipMessage.getSourceAddress();
  context.sip_source_port = sipMessage.getSourcePort();
  context.sip_interface_address = sipMessage.getInterfaceAddress();
  context.sip_interface_port = sipMessage.getInterfacePort();

  context.sip_target_address_value = {
    protocol: "",
    address: "",
    port: ""
  };
  context.sip_local_interface_value = {
    address: "",
    port: ""
  };
  context.sip_target_domain_value = "";

  //
  // Assume rejection.  Modules will set this to accept or banned
  //
  context.sbc_route_action = "reject";
  sipMessage.setProperty("route-action", context.sbc_route_action);

  return context;
}

exports.sbc_export_context_variables = function(sipMessage, context) {
  var request = sipMessage._request;

  if (context.sbc_retarget_refer.length > 0) {
    sipMessage.setProperty("retarget-refer", context.sbc_retarget_refer);
  }

  if (context.sbc_dialog_state_placement === DIALOG_STATE_IN_CONTACT) {
    sipMessage.setTransactionProperty("dialog-state-placement", DIALOG_STATE_IN_CONTACT);
  } else if (context.sbc_dialog_state_placement === DIALOG_STATE_IN_ROUTE) {
    sipMessage.setTransactionProperty("dialog-state-placement", DIALOG_STATE_IN_ROUTE);
  }

  if (context.sbc_disable_rtp_proxy) {
    sipMessage.setTransactionProperty("no-rtp-proxy", "1");
  }

  if (context.sbc_verbose_rtp_logging) {
    sipMessage.setTransactionProperty("enable-verbose-rtp", "1");
  }

  if (context.sbc_local_100_rel) {
    sipMessage.setTransactionProperty("local-100-rel", "1");
  }

  if (context.sbc_disable_181_relay) {
    sipMessage.setTransactionProperty("disable-181-relay", "1");
  }

  if (context.sbc_local_update) {
    sipMessage.setTransactionProperty("local-update", "1");
  }

  if (context.sbc_reject_code !== null) {
    sipMessage.setProperty("reject-code", context.sbc_reject_code);
  }

  if (context.sbc_reject_reason !== null) {
    sipMessage.setProperty("reject-reason", context.sbc_reject_reason);
  }

  if (context.sbc_ban_source_address) {
    context.sbc_route_action = "banned";
  }

  if (context.sbc_route_action) {
    sipMessage.setProperty("route-action", context.sbc_route_action);
  }

  if (context.sbc_patch_ipv6_as_rfc1918_address) {
    sipMessage.setTransactionProperty("patch-ipv6_as-rfc1918-address", "1");
  }

  if (context.sbc_force_sdp_audio_ip !== null) {
    sipMessage.setTransactionProperty("force-sdp-audio-ip", context.sbc_force_sdp_audio_ip);
  }

  if (context.sbc_force_sdp_audio_port !== null) {
    sipMessage.setTransactionProperty("force-sdp-audio-port", context.sbc_force_sdp_audio_port);
  }

  if (context.sbc_force_sdp_video_ip !== null) {
    sipMessage.setTransactionProperty("force-sdp-video-ip", context.sbc_force_sdp_video_ip);
  }

  if (context.sbc_force_sdp_video_port !== null) {
    sipMessage.setTransactionProperty("force-sdp-video-port", context.sbc_force_sdp_video_port);
  }

  if (context.sbc_force_sdp_global_ip !== null) {
    sipMessage.setTransactionProperty("force-sdp-global-ip", context.sbc_force_sdp_global_ip);
  }

  var attributes = "";
  var i = 0;
  if (context.sbc_sdp_audio_attributes !== null && context.sbc_sdp_audio_attributes.length) {
    for (i = 0; i < context.sbc_sdp_audio_attributes.length; i++) {
      attributes += context.sbc_sdp_audio_attributes[i];
      if (i < context.sbc_sdp_audio_attributes.length - 1) {
        attributes += "~";
      }
    }

    sipMessage.setTransactionProperty("sdp-audio-attributes", attributes);
  }

  if (context.sbc_sdp_video_attributes !== null && context.sbc_sdp_video_attributes.length) {
    attributes = "";
    i = 0;
    for (i = 0; i < context.sbc_sdp_video_attributes.length; i++) {
      attributes += context.sbc_sdp_video_attributes[i];
      if (i < context.sbc_sdp_video_attributes.length - 1) {
        attributes += "~";
      }
    }
    sipMessage.setTransactionProperty("sdp-video-attributes", attributes);
  }

  if (context.sbc_sdp_remove_audio_attributes !== null && context.sbc_sdp_remove_audio_attributes.length) {
    attributes = "";
    i = 0;
    for (i = 0; i < context.sbc_sdp_remove_audio_attributes.length; i++) {
      attributes += context.sbc_sdp_remove_audio_attributes[i];
      if (i < context.sbc_sdp_remove_audio_attributes.length - 1) {
        attributes += "~";
      }
    }

    sipMessage.setTransactionProperty("sdp-remove-audio-attributes", attributes);
  }

  if (context.sbc_sdp_remove_video_attributes !== null && context.sbc_sdp_remove_video_attributes.length) {
    attributes = "";
    i = 0;
    for (i = 0; i < context.sbc_sdp_remove_video_attributes.length; i++) {
      attributes += context.sbc_sdp_remove_video_attributes[i];
      if (i < context.sbc_sdp_remove_video_attributes.length - 1) {
        attributes += "~";
      }
    }
    sipMessage.setTransactionProperty("sdp-remove-video-attributes", attributes);
  }

  if (sipMessage.isRequest("INVITE")) {
    if (context.sbc_bleg_cid !== null && sbc_bleg_cid === B2B) {
      sipMessage.setTransactionProperty("cid-correlation", B2B);
      if (context.sbc_bleg_cid_suffix === null) {
        context.sbc_bleg_cid_suffix = "-leg2";
      }
      sipMessage.setTransactionProperty("cid-suffix", context.sbc_bleg_cid_suffix);
    } else if (context.sbc_bleg_cid !== null && context.sbc_bleg_cid === XCID) {
      sipMessage.setTransactionProperty("cid-correlation", XCID);
    }
  }


  //
  // Request URI
  //

  if (context.sip_local_reg_target_value.user.length > 0 && context.sip_local_reg_target_value.host.length > 0) {
    context.sip_target_user_value = context.sip_local_reg_target_value.user;
    context.sip_target_host_value = context.sip_local_reg_target_value.host;
    context.sip_target_port_value = context.sip_local_reg_target_value.port;
    sipMessage.setProperty("local-reg-retarget", "1");
  }

  if (context.sip_target_user_value !== null) {
    _parser.msgSetRequestUriUser(request, context.sip_target_user_value);
  }
  if (context.sip_target_host_value !== null) {
    if (context.sip_target_port_value !== null) {
      if (context.sip_target_port_value.length === 0) {
        _parser.msgSetRequestUriHostPort(request, context.sip_target_host_value);
      } else {
        _parser.msgSetRequestUriHostPort(request, context.sip_target_host_value + ":" + context.sip_target_port_value);
      }
    } else if (typeof context.sip_target_port === "string") {
      _parser.msgSetRequestUriHostPort(request, context.sip_target_host_value + ":" + context.sip_target_port);
    } else {
      _parser.msgSetRequestUriHostPort(request, context.sip_target_host_value);
    }
  }
  if (context.sip_target_port_value !== null && context.sip_target_host_value === null) {
    if (context.sip_target_port_value.length === 0) {
      _parser.msgSetRequestUriHostPort(request, context.sip_target_host);
    } else {
      _parser.msgSetRequestUriHostPort(request, context.sip_target_host + ":" + context.sip_target_port_value);
    }
  }
  if (context.sip_uri_parameters_value !== null) {
    _parser.msgSetRequestUriParameters(request, context.sip_uri_parameters_value);
  }

  //
  // FROM Header
  //
  if (context.sip_from_display_name_value !== null) {
    var from = _parser.msgHdrGet(request, "From");
    from = _parser.fromSetDisplayName(from, context.sip_from_display_name_value);
    _parser.msgHdrSet(request, "From", from);
  }
  if (context.sip_from_user_value !== null) {
    _parser.msgSetFromUser(request, context.sip_from_user_value)
  }
  if (context.sip_from_host_value !== null) {
    if (context.sip_from_port_value !== null) {
      if (context.sip_from_port_value.length === 0) {
        _parser.msgSetFromHostPort(request, context.sip_from_host_value);
      } else {
        _parser.msgSetFromHostPort(request, context.sip_from_host_value + ":" + context.sip_from_port_value);
      }
    } else if (typeof context.sip_from_port === "string") {
      _parser.msgSetFromHostPort(request, context.sip_from_host_value + ":" + context.sip_from_port);
    } else {
      _parser.msgSetFromHostPort(request, context.sip_from_host_value);
    }
  }
  if (context.sip_from_port_value !== null && context.sip_from_host_value === null) {
    if (context.sip_from_port_value.length === 0) {
      _parser.msgSetFromHostPort(request, context.sip_from_host);
    } else {
      _parser.msgSetFromHostPort(request, context.sip_from_host + ":" + context.sip_from_port_value);
    }
  }


  //
  // TO Header
  //
  if (context.sip_to_display_name_value !== null) {
    var to = _parser.msgHdrGet(request, "To");
    to = _parser.toSetDisplayName(to, context.sip_to_display_name_value);
    _parser.msgHdrSet(request, "To", to);
  }
  if (context.sip_to_user_value !== null) {
    _parser.msgSetToUser(request, context.sip_to_user_value)
  }
  if (context.sip_to_host_value !== null) {
    if (context.sip_to_port_value !== null) {
      if (context.sip_to_port_value.length === 0) {
        _parser.msgSetToHostPort(request, context.sip_to_host_value);
      } else {
        _parser.msgSetToHostPort(request, context.sip_to_host_value + ":" + context.sip_to_port_value);
      }
    } else if (typeof context.sip_to_port === "string") {
      _parser.msgSetToHostPort(request, context.sip_to_host_value + ":" + context.sip_to_port);
    } else {
      _parser.msgSetToHostPort(request, context.sip_to_host_value);
    }
  }
  if (context.sip_to_port_value !== null && context.sip_to_host_value === null) {
    if (context.sip_to_port_value.length === 0) {
      _parser.msgSetToHostPort(request, context.sip_to_host);
    } else {
      _parser.msgSetToHostPort(request, context.sip_to_host + ":" + context.sip_to_port_value);
    }
  }

  //
  // Transport
  //
  if (context.sip_target_domain_value.length > 0) {
    sipMessage.setRequestUriHostPort(context.sip_target_domain_value);
    sipMessage.setFromHostPort(context.sip_target_domain_value);
    sipMessage.setToHostPort(context.sip_target_domain_value);
  }

  if (context.sip_target_address_value.address.length > 0) {
    if (context.sip_target_address_value.protocol.length === 0) {
      context.sip_target_address_value.protocol = "udp";
    }
    sipMessage.setProperty("target-transport", context.sip_target_address_value.protocol);
    sipMessage.setProperty("target-address", context.sip_target_address_value.address);

    if (context.sip_target_address_value.port.length > 0) {
      sipMessage.setProperty("target-port", context.sip_target_address_value.port);
    }
  }

  if (context.sip_local_interface_value.address.length > 0) {
    sipMessage.setProperty("interface-address", context.sip_local_interface_value.address);
    if (context.sip_local_interface_value.port.length > 0) {
      sipMessage.setProperty("interface-port", context.sip_local_interface_value.port);
    }
  }
}


//
// Helper Functions
//
exports.set_target_address = function(context, protocol, address, port)
{
    context.sip_target_address_value.protocol = protocol;
    context.sip_target_address_value.address = address;
    if (typeof(port) == "string") {
        context.sip_target_address_value.port = port;
    } else if(typeof(port) == "number") {
        context.sip_target_address_value.port = port.toString();
    }
    context.sbc_route_action = "accept";
}

exports.reject_request = function(context, status_code, reason_phrase)
{
    context.sbc_route_action = "reject";
    if (typeof(status_code) === "string") {
          context.sbc_reject_code = status_code;
    } else if(typeof(status_code) === "number") {
          context.sbc_reject_code = status_code.toString();
    }
    if (typeof(reason_phrase) === "string") {
        context.sbc_reject_reason = reason_phrase;
    }
}


//
// Initialization
//
exports.initialize = function(configPath)
{
    return exports.sbc_initialize(configPath);
}

exports.run = function()
{
    isolate.on("handle_sip_request_event", function(request, userData) {
        var sipMessage = new SIPMessage(userData);
        if (request.eventName === "routeRequest") {
            var context = exports.sbc_create_context_variables(sipMessage);
            if (typeof(exports._on_route_request) === "function") {
                exports._on_route_request(context, sipMessage);
            } else {
                exports.reject_request(context, "500", "No Route Handler");
            }
            exports.sbc_export_context_variables(context.sipMessage, context);
        } else if (request.eventName === "inboundRequest") {
            if (typeof(exports._on_inbound_request) === "function") {
                exports._on_inbound_request(sipMessage);
            }
        }
        return { result: true };
    });

    isolate.on("handle_custom_event", function(request, userData) {
        var sipMessage = new SIPMessage(userData);
        var handler = null;
        if (request.eventName === "transportConfig") {
            handler = exports._on_initialize_transport;
        } else if (request.eventName === "userAgentConfig") {
            handler = exports._on_initialize_user_agent;
        }
        if (typeof(handler) === "function") {
            var config = handler();
            if (typeof(config) === "object") {
                sipMessage.setProperty("JSONConfig", JSON.stringify(config));
            }
        }
        return { result: true };
    });
    exports.sbc_run();
}

//
// Event Handlers
//
exports.on_route_request = function(handler)
{
    exports._on_route_request = handler;
}

exports.on_inbound_request = function(handler)
{
    exports._on_inbound_request = handler;
}

exports.on_initialize_transport = function(handler)
{
    exports._on_initialize_transport = handler;
}

exports.on_initialize_transport = function(handler)
{
    exports._on_initialize_transport = handler;
}

exports.on_initialize_user_agent = function(handler)
{
    exports._on_initialize_user_agent = handler;
}

