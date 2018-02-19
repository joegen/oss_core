const Base64 = require('compact-base64');

const _rBase64 = /^[ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/]+=*$/;
const _rNonPrintable = /[\x00-\x08\x0E-\x1F\x7F\x80-\x9F]/;

function isBase64SizeCorrect(v) {
    if (typeof v !== 'string')
        return false;

    if ((v.length * 6) % 8 !== 0)
        return false;

    return true;
}

function isBase64FormatCorrect(v) {
    if (typeof v !== 'string')
        return false;

    return _rBase64.test(v);
}

function isBase64Printable(v) {
    if (typeof v !== 'string')
        return false;

    try {
        const value = Base64.decode(v);
        return !_rNonPrintable.test(value);
    } catch (err) {
        // compact-base64 can't handle some Base64 binary strings correctly.
    }

    return false;
}

function isBase64(v, { allowBinary = false } = {}) {
    if (!isBase64SizeCorrect(v))
        return false;

    if (!isBase64FormatCorrect(v))
        return false;

    return allowBinary || isBase64Printable(v);
}

module.exports = {
    isBase64,
    isBase64Printable,
    isBase64SizeCorrect,
    isBase64FormatCorrect,
    encode: Base64.encode,
    decode: Base64.decode
};
