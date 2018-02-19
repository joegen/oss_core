function ConstantTransformer(value, options = {}) {
    if( !(this instanceof ConstantTransformer) ) {
        return this.transform(new ConstantTransformer(value, options));
    }

    this._value = value;
    this._reverseValue = options.reverseValue || value;
}

ConstantTransformer.prototype.parse = function() {
    return this._value;
};

ConstantTransformer.prototype.reverse = function() {
    return this._reverseValue;
};

module.exports = ConstantTransformer;
