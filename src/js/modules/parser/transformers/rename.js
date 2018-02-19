const _transform = require('lodash/transform');

function RenameTransformer(parser, reverser) {
    if( !(this instanceof RenameTransformer) ) {
        return this.transform(new RenameTransformer(parser, reverser));
    }

    this._parser = parser;
    this._reverser = reverser;
}

RenameTransformer.prototype.parse = function(source) {
    return _transform(source, (result, value, key) => {
        key = this._parser(key, value);
        result[key] = value;
    }, {});
};

RenameTransformer.prototype.reverse = function(source) {
    return _transform(source, (result, value, key) => {
        key = this._reverser(key, value);
        result[key] = value;
    }, {});
};

module.exports = RenameTransformer;
