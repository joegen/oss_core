const Base64 = require('../lib/base64');

function Base64Transformer(options = {}) {
    if( !(this instanceof Base64Transformer) ) {
        return this.transform(new Base64Transformer(options));
    }

    this._allowBinary = options.allowBinary || false;

    this._validationOptions = {
        allowBinary: this._allowBinary
    };
}

Base64Transformer.prototype.parse = function(value) {
    if (this._allowBinary)
        return Base64.decode(value);

    if (!Base64.isBase64(value, this._validationOptions))
        return value;

    return Base64.decode(value);
};

Base64Transformer.prototype.reverse = function(source) {
    return Base64.encode(source);
};

module.exports = Base64Transformer;
