const parseJSON = require('../lib/json').parse;

function JSONTransformer(options = {}) {
    if( !(this instanceof JSONTransformer) ) {
        return this.transform(new JSONTransformer(options));
    }

    this._defaultValue = options.defaultValue;
}

JSONTransformer.prototype.parse = function(value) {
    if (typeof value !== 'string')
        return value;

    const result = parseJSON(value);
    if (result !== null)
        return result;

    return this._defaultValue || result;
};

JSONTransformer.prototype.reverse = function(value) {
    return JSON.stringify(value);
};

module.exports = JSONTransformer;
