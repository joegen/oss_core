const _ua = require("./_resip_ua.jso");
const assert = require("assert");
__copy_exports(_ua, exports);

exports.watch_reg_events = _ua._watch_reg_events;
exports.profile_set_from = _ua._profile_set_from;
exports.profile_set_outbound_proxy = _ua._profile_set_outbound_proxy;
exports.profile_set_digest_credential = _ua._profile_set_digest_credential;
exports.profile_set_callback = _ua._profile_set_callback;
exports.profile_set_default_subscription_time = _ua._profile_set_default_subscription_time;

