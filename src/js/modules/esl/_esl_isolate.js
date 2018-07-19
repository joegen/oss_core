"use-strict";

const ESLConnection = require("esl/_esl_connection.jso").ESLConnection;
const isolate = require("isolate");

var connections = {}

isolate.on("esl_new_connection", function(uuid) {
    connection[uuid] = new ESLConnection();
    isolate.notifyParentIsolate("esl_new_connection", uuid);
});

isolate.on("esl_call_connect", function(args_) {
    var args = JSON.parse(args_);
    if (!args.uuid in connections) {
        var error = {uuid : args.uuid, method : "esl_call_connect", error : "Non-existent UUID"};
        isolate.notifyParentIsolate("esl_error", JSON.stringify(error));
    }
    if (connection[args.uuid].connect(args.host, args.port, args.password)) {
        isolate.notifyParentIsolate("esl_call_connect", args.uuid);
    } else {
        var error = {uuid : args.uuid, method : "esl_call_connect", error : "Unable to connect to remote"};
        isolate.notifyParentIsolate("esl_error", JSON.stringify(error));
    }
});

isolate.on("esl_call_api", function(args_) {
    var args = JSON.parse(args_);
    if (!args.uuid in connections) {
        var error = {uuid : args.uuid, method : "esl_call_api", error : "Non-existent UUID"};
        isolate.notifyParentIsolate("esl_error", JSON.stringify(error));
    }
    
    var response = connection[args.uuid].api(args.cmd, args.args);
    if (response) {
        isolate.notifyParentIsolate("esl_call_api", response);
    } else {
        var error = {uuid : args.uuid, method : "esl_call_connect", error : "Unable to connect to remote"};
        isolate.notifyParentIsolate("esl_error", JSON.stringify(error));
    }
});
