function EqualsTransformer(match, options = {}) {
    if( !(this instanceof EqualsTransformer) ) {
        return this.transform(new EqualsTransformer(match, options));
    }

    this._match = match;
    this._strict = options.strict || false;
    this._reverse = options.reverse || match;
}

EqualsTransformer.prototype.parse = function(value) {
    if (this._match instanceof RegExp)
        return this._match.test(value);

    if (typeof this._match === 'function')
        return this._match(value);

    if (this._strict)
        return this._match === value;

    return this._match == value;
};

EqualsTransformer.prototype.reverse = function(value) {
    if (value)
        return this._reverse;

    return null;
};

module.exports = EqualsTransformer;
