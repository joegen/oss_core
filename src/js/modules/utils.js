"use-strict"

exports.bufferToString =  function(buf)
{
  return buf.map(function(i){return String.fromCharCode(i)}).join("")
}
