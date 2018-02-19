const INVALID_DATE = 'Invalid Date';

function DateTransformer(nowOnInvalid = false) {
    if( !(this instanceof DateTransformer) ) {
        return this.transform(new DateTransformer(nowOnInvalid));
    }

    this._nowOnInvalid = nowOnInvalid;
}

DateTransformer.prototype.parse = function(value) {
    const parsedDate = new Date(value);
    if (parsedDate.toString() != INVALID_DATE)
        return parsedDate;

    if (this._nowOnInvalid)
        return new Date();

    return undefined;
};

DateTransformer.prototype.reverse = function(source) {
    if (source instanceof Date && source.toJSON)
        return source.toJSON();

    return source;
};

module.exports = DateTransformer;
