"use-strict";

const _config = require("./_config.jso");
const fs = require("filesystem");

//
// Config file prototype
//

var ConfigFile = function() {
  this._config = new _config.Config();
}

ConfigFile.prototype.open = function(path) {
  if (!fs.exists(path)) {
    return false;
  }
  return this._config.readFile(path);
}

ConfigFile.prototype.save = function(path) {
  return this._config.writeFile(path);
}

ConfigFile.prototype.toString = function(bufLen) {
  if (typeof bufLen === "undefined") {
    bufLen = 1024 * 1024;
  }
  return this._config.toString(bufLen);
}

ConfigFile.prototype.getString = function(key) {
  return this._config.lookupString(key);
}

ConfigFile.prototype.getInt = function(key) {
  return this._config.lookupInteger(key);
}

ConfigFile.prototype.getFloat = function(key) {
  return this._config.lookupFloat(key);
}

ConfigFile.prototype.getBool = function(key) {
  return this._config.lookupBoolean(key);
}

ConfigFile.prototype.getSetting = function(key) {
  var id = this._config.lookupSetting(key);
  if (typeof id !== "undefined") {
    return new ConfigSetting(id, this);
  }
}

ConfigFile.prototype.exists = function(key) {
  var id = this._config.lookupSetting(key);
  return (typeof id !== "undefined");
}

ConfigFile.prototype.self = function() {
  var id = this._config.rootSetting();
  if (typeof id !== "undefined") {
    return new ConfigSetting(id, this);
  }
}

//
// Setting Prototype
//
var ConfigSetting = function(id, config) {
  this._id = id;
  this._configFile = config;
  this._config = config._config;
  this._type = this._config.settingType(this._id);
}

ConfigSetting.prototype.size = function() {
  return this._config.settingLength(this._id);
}

ConfigSetting.prototype.getType = function() {
  return this._type;
}

ConfigSetting.prototype.getElement = function(index) {
  var childId = this._config.settingLookupElement(this._id, index);
  if (typeof childId !== "undefined") {
    return new ConfigSetting(childId, this._configFile);
  }
}

ConfigSetting.prototype.getString = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return this._config.settingLookupString(this._id, name);
}

ConfigSetting.prototype.getInt = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return this._config.settingLookupInteger(this._id, name);
}

ConfigSetting.prototype.getFloat = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return this._config.settingLookupFloat(this._id, name);
}

ConfigSetting.prototype.getBool = function(key) {
  var name = typeof key === "undefined" ? "" : name;
  return this._config.settingLookupBoolean(this._id, key);
}

ConfigSetting.prototype.getSetting = function(key) {
  var id = this._config.settingLookupSetting(this._id, key);
  if (typeof id !== "undefined") {
    return new ConfigSetting(id, this._configFile);
  }
}

ConfigSetting.prototype.get = function(key) {
  switch (this._type) {
    case _config.CONFIG_TYPE_STRING:
      return this.getString(key);
    case _config.CONFIG_TYPE_BOOL:
      return this.getBool(key);
    case _config.CONFIG_TYPE_INT:
      return this.getInt(key);
    case _config.CONFIG_TYPE_FLOAT:
      return this.getFloat(key);
    default:
      throw new Error("Invalid Argument Type");
  }
}

ConfigSetting.prototype.exists = function(key) {
  var id = this._config.settingLookupSetting(this._id, key);
  return (typeof id !== "undefined")
}

ConfigSetting.prototype.addGroup = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_GROUP),
    this._configFile
  );
}

ConfigSetting.prototype.addInt = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_INT),
    this._configFile
  );
}

ConfigSetting.prototype.addFloat = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_FLOAT),
    this._configFile
  );
}

ConfigSetting.prototype.addString = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_STRING),
    this._configFile
  );
}

ConfigSetting.prototype.addBool = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_BOOL),
    this._configFile
  );
}

ConfigSetting.prototype.addArray = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_ARRAY),
    this._configFile
  );
}

ConfigSetting.prototype.addList = function(key) {
  var name = typeof key === "undefined" ? "" : key;
  return new ConfigSetting(
    this._config.settingAddSetting(this._id, name, _config.CONFIG_TYPE_LIST),
    this._configFile
  );
}

ConfigSetting.prototype.setAsString = function(value) {
  return this._config.settingSetAsString(this._id, value);
}

ConfigSetting.prototype.setAsInt = function(value) {
  return this._config.settingSetAsInteger(this._id, value);
}

ConfigSetting.prototype.setAsFloat = function(value) {
  return this._config.settingSetAsFloat(this._id, value);
}

ConfigSetting.prototype.setAsBool = function(value) {
  return this._config.settingSetAsBoolean(this._id, value);
}

ConfigSetting.prototype.set = function(value) {
  var type = typeof value;
  switch (type) {
    case "string":
      return this.setAsString(value);
    case "boolean":
      return this.setAsBool(value);
    case "number":
      if (this._type === _config.CONFIG_TYPE_FLOAT) {
        return this.setAsFloat(value);
      } else {
        return this.setAsInt(value);
      }
    default:
      throw new Error("Invalid Argument Type");
  }
}

exports.Config = ConfigFile;
