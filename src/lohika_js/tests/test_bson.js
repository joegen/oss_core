"use strict";

const bson = require("bson");
const assert = require("assert");
const system = require("system");

try {
    var bsonObject = new bson.BSON();
    assert.ok(bsonObject.setString("str-key", "string-value"));
    assert.ok(bsonObject.getString("str-key") == "string-value");
    assert.ok(bsonObject.setBoolean("bool-key", false));
    assert.ok(bsonObject.getBoolean("bool-key") == false);
    assert.ok(bsonObject.setInt("int-key", 123));
    assert.ok(bsonObject.getInt("int-key") == 123);
    assert.ok(bsonObject.setDouble("double-key", 123.456));
    assert.ok(bsonObject.getDouble("double-key") == 123.456);
} catch(e) {
    print(e.message);
    system.exit(-1);
}