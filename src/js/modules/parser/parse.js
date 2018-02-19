const _set = require('lodash/set');
const _get = require('lodash/get');
const isString = require('lodash/isString');
const CustomTransformer = require('./transformers/custom');

const DIRECTION_ANY = 'ANY';
const DIRECTION_PARSE = 'PARSE';
const DIRECTION_REVERSE = 'REVERSE';

function Parse(path, options = {}) {
    if (!(this instanceof Parse))
        return new Parse(path, options);

    this._chain = [];
    this._options = options;

    if (isString(path))
        return this.select(path);

    return this;
}

Parse.DIRECTION_ANY = DIRECTION_ANY;
Parse.DIRECTION_PARSE = DIRECTION_PARSE;
Parse.DIRECTION_REVERSE = DIRECTION_REVERSE;

module.exports = Parse;

Parse.options = {};

Parse.register = function(name, handler, { overwrite = false } = {}) {
    if( !overwrite && this.prototype[name] )
        throw new Error(`${name} already has a handler.`);

    this.prototype[name] = handler;
};

Parse.setOption = function(key, value) {
    _set(this.options, key, value);
    return Parse;
};

Parse.getOption = function(key) {
    return _get(this.options, key);
};

Parse.prototype.setOption = function(key, value) {
    _set(this._options, key, value);
    return this;
};

Parse.prototype.getOption = function(key) {
    return _get(this._options, key, this.constructor.getOption(key));
};

Parse.prototype.transform = function(parse, reverse) {
    if (typeof parse !== 'object') {
        parse = new CustomTransformer(parse, reverse);
    }

    this._chain = this._chain.concat(parse);

    return this;
};

Parse.prototype.chain = function(configurator) {
    return configurator(this) || this;
};

Parse.prototype.isDirectionEnabled = function(direction) {
    direction = direction.toUpperCase();
    const configuredDirection = (this.getOption('direction') || DIRECTION_ANY);
    const enabledDirection = configuredDirection.toUpperCase();

    if (DIRECTION_ANY == enabledDirection)
        return true;

    return ([DIRECTION_ANY, enabledDirection].indexOf(direction) > -1);
};

Parse.prototype.parse = function(obj) {
    if (!this.isDirectionEnabled(DIRECTION_PARSE))
        return obj;

    const len = this._chain.length;

    for (let i = 0; i < len; i++)
        obj = this._chain[i].parse(obj);

    return obj;
};

Parse.prototype.reverse = function(obj) {
    if (!this.isDirectionEnabled(DIRECTION_REVERSE))
        return obj;

    let i = this._chain.length;

    while( i-- ) {
        obj = this._chain[i].reverse(obj);
    }

    return obj;
};

