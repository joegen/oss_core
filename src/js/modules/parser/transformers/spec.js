const _isPlainObject = require('lodash/isPlainObject');
const _transform = require('lodash/transform');
const _merge = require('lodash/merge');

function SpecTransformer(spec, parse) {
    if( !(this instanceof SpecTransformer) ) {
        return this.transform(new SpecTransformer(spec, this.constructor));
    }

    this._parse = parse || require('../index');
    this._spec = this._completeSpec(spec);
}

SpecTransformer.prototype._completeSpec = function(spec) {
    const parse = this._parse;

    return _transform(spec, (r, v, k) => {
        if (typeof v === 'string')
            v = parse(v);
        else if (_isPlainObject(v))
            v = this._completeSpec(v);

        r[k] = v;
    }, {});
};

SpecTransformer.prototype.parse = function(source) {
    return this._toSpec(this._spec, source);
};

SpecTransformer.prototype._toSpec = function(spec, data) {
    return _transform(spec, (r, v, k) => {
        if (_isPlainObject(v))
            r[k] = this._toSpec(v, data);
        else
            r[k] = v.parse(data);
    }, {});
};

SpecTransformer.prototype.reverse = function(source) {
    return this._fromSpec(this._spec, source);
};

SpecTransformer.prototype._fromSpec = function(spec, data) {
    return _transform(spec, (r, v, k) => {
        const value = data && data[k];

        if (_isPlainObject(v))
            _merge(r, this._fromSpec(v, value));
        else
            _merge(r, v.reverse(value));
    }, {});
};

module.exports = SpecTransformer;
