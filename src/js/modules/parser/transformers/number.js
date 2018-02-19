const isString = require('lodash/isString');

function normalizer(number) {
    if (isString(number)) {
        number = number
            // match all dots and commas but the last one and remove
            .replace(/(,|\.)(?=[^,.]*(\,|\.))/g, '')
            .replace(',', '.');
    }

    return number;
}

function NumberTransformer(options = {}) {
    if( !(this instanceof NumberTransformer) ) {
        return this.transform(new NumberTransformer(options));
    }

    this._NaNValue = options.NaNValue || 0;
    this._normalizer = options.normalizer || normalizer;
    this._base = options.base || 10;
}

NumberTransformer.prototype.parse = function(value) {
    value = this._normalizer(value);

    const isNumber = (typeof value === 'number');

    if (!isNumber && this._base !== 10)
        value = parseInt(value, this._base);
    else if(!isNumber)
        value = parseFloat(value);

    if (isNaN(value))
        return this._NaNValue;

    return value;
};

NumberTransformer.prototype.reverse = function(value) {
    const isNumber = (typeof value === 'number');

    if (isNumber && this._base !== 10)
        value = value.toString(this._base);

    return value;
};

module.exports = NumberTransformer;
