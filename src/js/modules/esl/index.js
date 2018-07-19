"use-strict";

const esl_event = require("./_esl_event.jso");
const esl_connection = require("./_esl_connection.jso");
const isolate = require("isolate");

__copy_exports(esl_event, exports);
__copy_exports(esl_connection, exports);

isolate.on("esl_call_api_result", function(args) {
  var result = JSON.parse(args);
  var uuid = args.esl_uuid;
});


var esl_create_connection = function(host, port, password) {
    var connection = new ESLConnection();
    if (!connection.connect(host, port, password)) {
        return undefined;
    }
    return connection;
}

var esl_call_api = function(connection, command, args, callback) {
    
}