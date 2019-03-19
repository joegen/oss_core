var async = require("async");
var isolate = require("isolate");
var sbc = require("sbc-hook");
var logger = require("logger");

const SIPMessage = require("sip-parser").SIPMessage;

if (sbc.sbc_initialize("/usr/local/freeswitch/conf/oss_core") ) {
    //sbc.sbc_set_log_level("debug");
    sbc.sbc_run();
    sbc.sbc_start_options_keep_alive(true);
}

isolate.on("handle_sip_request_event", function(request, userData) {
    if (request.eventName === "routeRequest") {
        logger.log_notice("notice", JSON.stringify(request));
        request.sipMessage = new SIPMessage(userData);
        request.globals = sbc.sbc_create_global_variables(request.sipMessage);
        request.globals.sbc_route_action = "reject";
        sbc.sbc_export_global_vars(request.sipMessage, request.globals);
    }
    return { result: true };
});
