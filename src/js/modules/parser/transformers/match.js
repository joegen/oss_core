const _transform = require('lodash/transform');

function MatchTransformer(match) {
    if( !(this instanceof MatchTransformer) ) {
        return this.transform(new MatchTransformer(match));
    }

    if( typeof match === 'string' ) {
        // Escape string
        match = match.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
        match = new RegExp(match);
    }

    this._match = match;
}

MatchTransformer.prototype.parse = function(source) {
    return _transform(source, (result, value, key) => {
        if( this._match.test(key) )
            result[key] = value;
    }, {});
};

MatchTransformer.prototype.reverse = MatchTransformer.prototype.parse;

module.exports = MatchTransformer;
