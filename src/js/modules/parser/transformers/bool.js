const _isUndefined = require('lodash/isUndefined');
const _includes = require('lodash/includes');

const TRUE = ['1', 'true', 'yes', 'y'];

const BOOLEAN = 'BOOLEAN';
const STRING = 'STRING';
const NUMBER = 'NUMBER';

function BooleanTransformer(options = {}) {
    if( !(this instanceof BooleanTransformer) ) {
        return this.transform(new BooleanTransformer(options));
    }

    this._defaultValue = options.defaultValue;
    this._reverseTo = options.reverseTo || BOOLEAN;
}

BooleanTransformer.BOOLEAN = BOOLEAN;
BooleanTransformer.STRING = STRING;
BooleanTransformer.NUMBER = NUMBER;

BooleanTransformer.prototype.parse = function(value) {
    if (_isUndefined(value) && !_isUndefined(this._defaultValue))
        return this._defaultValue;

    return (typeof value === 'string') ?
        _includes(TRUE, value) : !!value;
};

BooleanTransformer.prototype.reverse = function(source) {
    source = !!source;

    if (this._reverseTo === STRING) {
        return source ? 'true' : 'false';
    }

    if (this._reverseTo === NUMBER) {
        return source ? 1 : 0;
    }

    return source;
};

module.exports = BooleanTransformer;
