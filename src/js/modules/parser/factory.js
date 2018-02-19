const ParseClass = require('./parse');

const staticMethods = Object.keys(ParseClass).reduce((out, k) => {
    if (ParseClass.hasOwnProperty(k))
        out[k] = ParseClass[k];
    return out;
}, {});

module.exports =
function factory() {
    function Parse(path, options) {
        if (!(this instanceof Parse))
            return new Parse(path, options);

        return ParseClass.call(this, path, options);
    }

    // Make static methods available
    Object.assign(Parse, staticMethods);

    // Copy prototype and ensure (new Parse()) instanceof ParseClass == true
    Parse.prototype = Object.create(ParseClass.prototype);
    Parse.prototype.constructor = Parse;

    // Create an empty options object
    Parse.options = {};

    // Register default transforms
    Parse.register('select', require('./transformers/select'));
    Parse.register('match', require('./transformers/match'));
    Parse.register('rename', require('./transformers/rename'));
    Parse.register('map', require('./transformers/map'));
    Parse.register('group', require('./transformers/group'));
    Parse.register('oneOf', require('./transformers/oneOf'));
    Parse.register('equals', require('./transformers/equals'));

    Parse.register('constant', require('./transformers/constant'));
    Parse.register('date', require('./transformers/date'));
    Parse.register('bool', require('./transformers/bool'));
    Parse.register('number', require('./transformers/number'));
    Parse.register('string', require('./transformers/string'));
    Parse.register('array', require('./transformers/array'));
    Parse.register('base64', require('./transformers/base64'));
    Parse.register('json', require('./transformers/json'));

    Parse.register('spec', require('./transformers/spec'));
    Parse.register('multilingual', require('./transformers/multilingual'));
    Parse.register('stripPrefix', require('./transformers/stripPrefix'));

    return Parse;
};
