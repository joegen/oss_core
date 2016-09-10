"use strict";

const _bson = require("_bson");

function BSON() {
    var ptr = _bson.bson_create();
    Object.defineProperty(this, "_ptr", 
    {
        value: ptr,
        writable: false
    });
}

Duktape.fin(BSON.prototype, function (obj) {
    if (obj !== BSON.prototype) {
        _bson.bson_destroy(obj._ptr);
    }
});

BSON.prototype.setString = function(key, value) {
    return _bson.bson_append_string(this._ptr, key, value);
}

BSON.prototype.getString = function(key) {
    return _bson.bson_get_string(this._ptr, key);
}

BSON.prototype.setBoolean = function(key, value) {
    return _bson.bson_append_boolean(this._ptr, key, value);
}

BSON.prototype.getBoolean = function(key) {
    return _bson.bson_get_boolean(this._ptr, key);
}

BSON.prototype.setInt = function(key, value) {
    return _bson.bson_append_int(this._ptr, key, value);
}

BSON.prototype.getInt = function(key) {
    return _bson.bson_get_int(this._ptr, key);
}

BSON.prototype.setDouble = function(key, value) {
    return _bson.bson_append_double(this._ptr, key, value);
}

BSON.prototype.getDouble = function(key) {
    return _bson.bson_get_double(this._ptr, key);
}

exports.BSON = BSON;