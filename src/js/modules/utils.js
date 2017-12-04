"use-strict"

exports.bufferToString =  function(buf)
{
  return buf.map(function(i){return String.fromCharCode(i)}).join("")
}

exports.stringReplace = function(target, search, replacement) 
{
    return target.replace(new RegExp(search, 'g'), replacement);
}
