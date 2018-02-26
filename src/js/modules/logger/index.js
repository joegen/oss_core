"use-strict"

const _logger = require("./_logger.jso");

__copy_exports(_logger, exports);

var _log = function(level, args) {
  var msg;
  if (args.length == 1) {
    msg = args[0];
  } else {
    for (var i = 0; i < args.length; i++) {
      if (i == 0)
        msg = args[i];
      else
        msg += args[i];

      if (i < args.length - 1) {
        msg += " ";
      }
    }
  }

  if (typeof msg === "undefined") {
    msg = "undefined";
  }

  _logger.log(level, msg.toString());
}

var log_notice = function() {
  _log(_logger.NOTICE, arguments);
}

var log_info = function() {
  _log(_logger.INFO, arguments);
}

var log_debug = function() {
  _log(_logger.DEBUG, arguments);
}

var log_trace = function() {
  _log(_logger.TRACE, arguments);
}

var log_warning = function() {
  _log(_logger.WARNING, arguments);
}

var log_error = function() {
  _log(_logger.ERROR, arguments);
}

var log_critical = function() {
  _log(_logger.CRITICAL, arguments);
}

var log_fatal = function() {
  _log(_logger.FATAL, arguments);
}

var setLevel = function(level) {
  _logger.level = level;
}

exports.log_notice = log_notice;
exports.log_info = log_info;
exports.log_debug = log_debug;
exports.log_trace = log_trace;
exports.log_warning = log_warning;
exports.log_error = log_error;
exports.log_critical = log_critical;
exports.log_fatal = log_fatal;
exports.setLevel = setLevel;

exports.MSG_SOURCE = "%s"; // - message source
exports.MSG_TEXT = "%t"; // - message text
exports.PRIO_NUM = "%l"; // - message priority level (1 .. 7)
exports.PRIO_TEXT = "%p"; // - message priority (Fatal, Critical, Error, Warning, Notice, Information, Debug, Trace)
exports.PRIO_LETTER = "%q"; // - abbreviated message priority (F, C, E, W, N, I, D, T)
exports.PROCESS_ID = "%P"; // - message process identifier
exports.THREAD_NAME = "%T"; // - message thread name
exports.THREAD_ID = "%I"; // - message thread identifier (numeric)
exports.HOST = "%N"; // - node or host name
exports.FILE = "%U"; // - message source file path (empty string if not set)
exports.LINE = "%u"; // - message source line number (0 if not set)
exports.DAY_SHORT = "%w"; // - message date/time abbreviated weekday (Mon, Tue, ...)
exports.DAY_LONG = "%W"; // - message date/time full weekday (Monday, Tuesday, ...)
exports.MONTH_SHORT = "%b"; // - message date/time abbreviated month (Jan, Feb, ...)
exports.MONTH_LONG = "%B"; // - message date/time full month (January, February, ...)
exports.DAY_NUM_ZERO_PADDED = "%d" // - message date/time zero-padded day of month (01 .. 31)
exports.DAY_NUM = "%e"; // - message date/time day of month (1 .. 31)
exports.DAY_NUM_SPACE_PADDED = "%f"; // - message date/time space-padded day of month ( 1 .. 31)
exports.MONTH_NUM_ZERO_PADDED = "%m" // - message date/time zero-padded month (01 .. 12)
exports.MONTH_NUM = "%n"; // - message date/time month (1 .. 12)
exports.MONTH_NUM_SPACE_PADDED = "%o"; // - message date/time space-padded month ( 1 .. 12)
exports.YEAR_SHORT = "%y"; // - message date/time year without century (70)
exports.YEAR = "%Y"; // - message date/time year with century (1970)
exports.HOUR_24 = "%H"; // - message date/time hour (00 .. 23)
exports.HOUR_12 = "%h"; // - message date/time hour (00 .. 12)
exports.AM_PM = "%A"; // - message date/time AM/PM
exports.MINUTE = "%M"; // - message date/time minute (00 .. 59)
exports.SECOND = "%S"; // - message date/time second (00 .. 59)
exports.MILLISECOND = "%i"; // - message date/time millisecond (000 .. 999)
exports.TIME_ZOME_ISO_8601_FORMAT = "%z"; // - time zone differential in ISO 8601 format (Z or +NN.NN)
exports.TIME_ZONE_RFC_FORMAT = "%Z"; // - time zone differential in RFC format (GMT or +NNNN)
exports.TIME_USE_LOCAL_TIME = "%L"; //  - convert time to local time (must be specified before any date/time specifier; does not itself output anything)
exports.EPOCH = "%E"; // - epoch time (UTC, seconds since midnight, January 1, 1970)