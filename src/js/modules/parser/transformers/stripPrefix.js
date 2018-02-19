const _transform = require('lodash/transform');

function StripPrefixTransformer(prefix) {
    if( !(this instanceof StripPrefixTransformer) ) {
        return this.transform(new StripPrefixTransformer(prefix));
    }

    this._prefix = prefix;
}

StripPrefixTransformer.prototype.parse = function(source) {
    const prefix = this._prefix;
    const length = prefix.length;

    return _transform(source, (result, value, key) => {
        if( key.indexOf(prefix) !== 0 ) return;

        result[key.substr(length)] = value;
    }, {});
};

StripPrefixTransformer.prototype.reverse = function(source) {
    const prefix = this._prefix;
    return _transform(source, (result, value, key) => {
        result[prefix + key] = value;
    }, {});
};

module.exports = StripPrefixTransformer;
