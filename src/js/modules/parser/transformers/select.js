const _get = require('lodash/get');
const _set = require('lodash/set');

function SelectTransformer(path) {
    if( !(this instanceof SelectTransformer) ) {
        return this.transform(new SelectTransformer(path));
    }

    this._path = path;
}

SelectTransformer.prototype.parse = function(source) {
    return _get(source, this._path);
};

SelectTransformer.prototype.reverse = function(source) {
    const result = {};
    _set(result, this._path, source);
    return result;
};

module.exports = SelectTransformer;
