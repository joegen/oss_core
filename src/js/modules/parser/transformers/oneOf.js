const _isEmpty = require('lodash/isEmpty');
const _isObject = require('lodash/isObject');
const _merge = require('lodash/merge');

function isEmpty(v) {
    if (['boolean', 'number'].indexOf(typeof v) > -1)
        return v;

    return _isEmpty(v);
}

function OneOfTransformer(parsers, options = {}) {
    if( !(this instanceof OneOfTransformer) ) {
        return this.transform(new OneOfTransformer(parsers, options));
    }

    this._parsers = parsers;
    this._test = options.test || (v => !isEmpty(v));
    this._reverseAll = true;

    if (typeof options.reverseAll === 'boolean')
        this._reverseAll = options.reverseAll;
}

OneOfTransformer.prototype.parse = function(v) {
    const parsers = this._parsers;
    const len = parsers.length;
    const test = this._test;

    for (let i = 0; i < len; i++) {
        const result = parsers[i].parse(v);

        if (test(result))
            return result;
    }

    return;
};

OneOfTransformer.prototype.reverse = function(v) {
    const parsers = this._parsers;
    const len = parsers.length;

    if (len < 1)
        throw new Error('No parsers defined');

    if (!this._reverseAll)
        return parsers[0].reverse(v);

    let reversed = {};
    for (let i = 0; i < len; i++) {
        const result = parsers[i].reverse(v);

        if (typeof result === 'undefined')
            continue;

        if (_isObject(reversed) && _isObject(result))
            _merge(reversed, result);
        else
            reversed = result;
    }

    return reversed;
};

module.exports = OneOfTransformer;
