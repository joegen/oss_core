"use-strict"

const _utils = require("./_utils.jso");
__copy_exports(_utils, exports);

exports.bufferToString = function(buf) {
  return buf.map(function(i) {
    return String.fromCharCode(i)
  }).join("")
}

exports.stringReplace = function(target, search, replacement) {
  return target.replace(new RegExp(search, 'g'), replacement);
}

exports.getObjectKeys = Object.keys || function(obj) {
  var keys = [];
  for (var key in obj) {
    if (Object.prototype.hasOwnProperty.call(obj, key)) keys.push(key);
  }
  return keys;
};

// Taken from https://github.com/sindresorhus/multiline/blob/master/index.js
var reCommentContents = /\/\*!?(?:\@preserve)?[ \t]*(?:\r\n|\n)([\s\S]*?)(?:\r\n|\n)[ \t]*\*\//;
exports.multiline = function(fn) {
  if (typeof fn !== 'function') {
    throw new TypeError('Expected a function');
  }

  var match = reCommentContents.exec(fn.toString());

  if (!match) {
    throw new TypeError('Multiline comment missing.');
  }
  return match[1];
};
