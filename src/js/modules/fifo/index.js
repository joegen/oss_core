// Based on Queue implemention from Stephen Morley - http://code.stephenmorley.org/ 
"use-strict"

var Fifo = function() {
  this._fifo = [];
  this._offset = 0;
}

Fifo.prototype.size = function() {
  return this._fifo.length - this._offset;
}

Fifo.prototype.empty = function() {
  return this._fifo.length == 0;
}

Fifo.prototype.push = function(item) {
  this._fifo.push(item);
}

Fifo.prototype.pop = function() {
  if (this.empty()) {
    return undefined;
  }
  var item = this._fifo[this._offset];
  this.reclaim();
  return item;
}

Fifo.prototype.reclaim = function() {
  if (++this._offset * 2 >= this._fifo.length) {
    this._fifo = this._fifo.slice(this._offset);
    this._offset = 0;
  }
}

Fifo.prototype.top = function() {
  return this.empty() ? undefined : this._fifo[this._offset];
}

exports.Fifo = Fifo;