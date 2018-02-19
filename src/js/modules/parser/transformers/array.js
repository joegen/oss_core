const isArray = require('lodash/isArray');
const parseJSON = require('../lib/json').parse;

const MODE_ANY = 'ANY';
const MODE_JSON = 'JSON';
const MODE_SEPARATOR = 'SEPARATOR';

function ArrayTransformer(options = {}) {
    if( !(this instanceof ArrayTransformer) ) {
        return this.transform(new ArrayTransformer(options));
    }

    this._mode = options.mode || MODE_ANY;
    this._separator = options.separator || ',';
}

ArrayTransformer.ANY = MODE_ANY;
ArrayTransformer.JSON = MODE_JSON;
ArrayTransformer.SEPARATOR = MODE_SEPARATOR;

ArrayTransformer.prototype.parse = function(value) {
    if (!value && typeof value !== 'number')
        return [];

    if (isArray(value))
        return value;

    let result = value;

    if (typeof value !== 'string')
        value = value + '';

    if (this._mode === MODE_JSON || this._mode === MODE_ANY)
        result = parseJSON(value);

    if (!isArray(result))
        result = null;

    if (this._mode === MODE_SEPARATOR || (this._mode === MODE_ANY && !result))
        result = value.split(this._separator);

    if (!isArray(result))
        result = [];

    return result;
};

ArrayTransformer.prototype.reverse = function(value) {
    if (!isArray(value))
        return value;

    if (this._mode === MODE_JSON || this._mode === MODE_ANY)
        return JSON.stringify(value);

    if (this._mode === MODE_SEPARATOR)
        return value.join(this._separator);

    return value;
};

module.exports = ArrayTransformer;
