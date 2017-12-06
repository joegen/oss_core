"use-strict"

exports.bufferToString =  function(buf)
{
  return buf.map(function(i){return String.fromCharCode(i)}).join("")
}

exports.stringReplace = function(target, search, replacement) 
{
    return target.replace(new RegExp(search, 'g'), replacement);
}

exports.getObjectKeys = Object.keys || function (obj) 
{
  var keys = [];
  for (var key in obj) 
  {
    if (Object.prototype.hasOwnProperty.call(obj, key)) keys.push(key);
  }
  return keys;
};
