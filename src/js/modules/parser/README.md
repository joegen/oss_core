# parse-js
[![CircleCI](https://circleci.com/gh/ambassify/parse-js.svg?style=svg)](https://circleci.com/gh/ambassify/parse-js)

Utility library for object structure conversion.

- [Installation](#installation)
- [Usage](#usage)
  - [.parse()](#parse)
  - [.reverse()](#reverse)
  - [.transform()](#transform)
  - [.chain()](#chain)
- [Configuration](#configuration)
  - [.setOption() / .getOption()](#configuration)
- [Transformers](#transformers)
  - [.select()](#select)
  - [.match()](#match)
  - [.rename()](#rename)
  - [.map()](#map)
  - [.group()](#group)
  - [.oneOf()](#oneof)
  - [.equals()](#equals)
  - [.constant()](#constant)
  - [.date()](#date)
  - [.bool()](#bool)
  - [.number()](#number)
  - [.string()](#string)
  - [.array()](#array)
  - [.base64()](#base64)
  - [.json()](#json)
  - [.spec()](#spec)
  - [.multilingual()](#multilingual)
  - [.stripPrefix()](#stripprefix)

## Installation

```shell
npm install --save parse-js
```

## Usage

A parser always starts with a call to `parse()`, next you can chain any transformer of of that as is required.

```javascript
const parse = require('parse-js');

parse().bool().parse('true'); // true
parse().match('a').parse({
    atest: 'test123',
    btest: 'test456'
}); // { atest: 'test123' }
```

Parse also accepts one argument which can be the key to extract from the object to parse. Passing this argument will automatically chain the `select` transformer.

```javascript
const parse = require('parse-js');

parse('a-key').parse({
    'a-key': 'a-value',
    'some-key': 'some-value'
}); // 'a-value'
```

This is equivalent to:

```javascript
const parse = require('parse-js');

parse().select('a-key').parse({
    'a-key': 'a-value',
    'some-key': 'some-value'
}); // 'a-value'
```

#### .parse()

```javascript
.parse(data)
```

Whenever `.parse()` is called the configured chain will be executed on the first argument `data`.

If the option `direction` is set to `REVERSE` this method will simply return
the `data` argument as is without modifying it.

Example:
```javascript
parse('test-key').base64().parse({ 'test-key': 'SGVsbG8gV29ybGQ=' });
// Hello World
```

#### .reverse()

```javascript
.reverse(sourceData)
```

The `.reverse()` method will apply all the `reverse` methods of each transformer
and attempts to reassembly the original object based on the sourceData.

Example:

```javascript
parse('test-key').base64().reverse('Hello World');
// {
//    'test-key': 'SGVsbG8gV29ybGQ='
// }
```

#### .transform()

```javascript
.transform(parser, [reverser])
```

This method allows you to chain your own custom `parser` and `reverser`. Both
the `parser` and `reverser` take one argument as input which is the value to
be parsed or reversed.

Instead of supplying both methods as separator arguments you can also pass
the `parser` and `reverser` as an object with both the keys defined.

Example:

```javascript
// Increment / decrement transformer
function parser(v) { return v + 1; };
function reverser(v) { return v - 1; };

parse().transform(parser, reverser).parse(1); // 2
parse().transform(parser, reverser).reverse(3); // 2

// or alternatively
const transformer = {
    parser: parser,
    reverser: reverser
}

parse().transform(transformer).parse(1); // 2
```

#### .chain()

```javascript
.chain(configurator)
```

The `.chain()` method allows your to create pre-defined chains which can be
easily re-used for different parsers.

Example:

This example created a pre-defined one-way base64 parser.

```javascript
function base64_decode(p) {
    return p.base64().setOption('direction', 'PARSE');
}

parse().select('some-key')
    .chain(base64_decode)
    .parse({ 'some-key': 'SGVsbG8gV29ybGQ=' }); // Hello World
```

### Configuration

Both `parse-js` instances and the `parse` method have methods to set global
options `setOption` and `getOption`, these can be used to configure
transformers that have global settings.

```javascript
Parse.setOption(key, value);
Parse.getOption(key);

parse().setOption(key, value);
parse().getOption(key);
```

Currently only the [multilingual transformer](#multilingual) has such options.

The behaviour of a `parse-js` chain can be altered using the `direction` option
which configures in which directions the transformers should be applied. When
set to `PARSE` the `.reverse()` calls will not touch the data supplied. Similarly
setting `direction` to `REVERSE` will leave the data untouched as `.parse()` is
called. By default both directions are enabled and the option is set to `ANY`.

### Transformers

#### .select()

```javascript
parse().select(key)
```

Selects a value from the object provided to the final `.parse()` call, the key supplied here can be any key supported by [lodash get](https://lodash.com/docs/4.16.0#get).


#### .match()

```javascript
parse().match(valueToMatch)
```

Only selects those properties from an object that match `valueToMatch`.

- `valueToMatch` can either be a string or a regular expression.

#### .rename()

```javascript
parse().rename(nameParser, nameReverser);
```

Converts key names using a function for each transition.

- `nameParser(key, value)` will be called with the original key and value as arguments and should return the new key.
- `nameReverser(key, value)` will be called with the generated key and the value set on the object and should return the key to which this value should be written.

#### .map()

```javascript
parse().map(callback)
```

Map will walk each key of a select value and call the `callback` function with a new instance of `parse-js` specific to that key.

- `callback(parse)` will be called with a new instance of parse for each key, which you can then customize by adding new chained transformers.

Example

```javascript
parse().map(p => p.number()).parse({
    a: '123',
    b: 0,
    c: '12.222,3'
}); // { a: 123, b: 0, c: 12222.3 }
```


#### .group()

```
parse().group(regex, key, index)
```

`group` will walk over each key of an object matching it against regular expression `regex`. If it matches the value will be stored at `result[match[key]][match[index]]`, where `match` is the set of matching groups from `regex`. If a key does not match the `regex` it will be re-attached to the object untouched.

- The `key` argument should be set to the index of the matching group that selects the new key to use.
- The `index` arguments should be set to the index of the matching group that selects the sub-key under which to store the value.

Example:

```javascript
parse().group(/(a|b|c)-(name|value)/, 1, 2).parse({
    'a-name': 'a-name',
    'b-name': 'b-name',
    'c-value': 'c-value',
    'b-value': 'b-value',
    'c-name': 'c-name',
    'a-value': 'a-value'
});
// {
//    a: { value: 'a-value', name: 'a-name' },
//    b: { value: 'b-value', name: 'b-name' },
//    c: { value: 'c-value', name: 'c-name' }
// }
```

#### .oneOf()

```javascript
parse().oneOf(parsers, [options = {}])
```

`oneOf` lets you define multiple parsers of which the first in the list with a
result that is valid according to the `test` option will be used.

- `parsers` an array of `parse-js` parsers to go through.
- `options`
  - `test` a method which returns true if the result of a parser is valid. (default: `!isEmpty(v)`)
  - `reverseAll` controls whether all parsers are called to reverse the value or only the first one. (default: `true`)

Example:

```javascript
parse().oneOf([
    parse().select('givenName').string(),
    parse().select('firstName').string(),
    parse().select('email')
]).parse({ 'firstName': 'John', email: 'john.doe@gmail.com' });
// 'John'
```

#### .equals()

```javascript
parse().equals(valueToMatch, [options = {}])
```

If the selected value matched `valueToMatch` it will return `true`, if it does not it will return `false`.

- `valueToMatch` can be either a regular expressions, function or simply any value.
- `options` can be used to change behaviour of the transformer
  - `strict` will ensure `===` comparison is used when comparing. (default: `false`).
  - `reverse` can be set to the value that should be set when reversing a `true` value. (default: `valueToMatch`)

Example:

```javascript
parse().equals('some-value').parse('some-other-value'); // false
parse().equals('some-value').parse('some-value'); // true
```

#### .constant()

```javascript
parse().constant(constantValue, [options = {}])
```

Always returns `constantValue` from `parse` and `reverse`.
The value returned from `reverse` can be different from `constantValue`
using the `reverseValue` option.

- `constantValue` the value that will be returned by this transformer.
- `options`
  - `reverseValue` If `reverse` should return a different value it can be configured using this option.

Example:

```javascript
parse().constant('a-constant').parse('some-value'); // 'a-constant'
parse().constant('a-constant', {
    reverseValue: 'b-constant'
}).reverse('some-value'); // 'b-constant'
```

#### .date()

```javascript
parse().date([nowOnInvalid = false])
```

Converts the selected value into a javascript date object.

- `nowOnInvalid` If set to true will return the current date-time whenever the value being parsed is not a valid date.

#### .bool()

```javascript
parse().bool([options = {}])
```

Converts the selected value to a boolean value.

- `options`
  - `defaultValue` when the value being parsed is `undefined` what should be set as the default value.
  - `reverseTo` configures the datatype to which the boolean values are reversed. Valid options are `BOOLEAN`, `STRING` or `NUMBER`.

#### .number()

```javascript
parse().number([options = {}])
```

Converts the selected value to a number.

- `options`
  - `NaNValue` the value that will be set when the selected value can not be converted. (default: `0`)
  - `normalizer` a function used to normalize strings to number-like strings. The default handles removing multiple comma or dots in the string.
  - `base` the base in which the value is expressed. (default: `10`)


#### .string()

```javascript
parse().string([options = {}])
```

Converts the selected value to a string.
The selected value will be concatenated with an empty string which will call the `toString()` method of most values.

- `options`
  - `defaultValue` the value to return whenever the selected value is `undefined`.

#### .array()

```javascript
parse().array([options = {}])
```

This transformer will ensure that the selected value will be converted to an array. Whenever this fails it will return an empty array.

- `options`
  - `mode` the methods that are allowed to be used to convert values to arrays. (default: `ANY`). Valid options are `ANY`, `JSON` and `SEPARATOR`.
  - `separator` the separator to be used when `mode` is set to `ANY` or `SEPARATOR`. (default: `,`)

#### .base64()

```javascript
parse().base64([options = {}])
```

Handles conversion from and to base64 strings.

- `options`
  - `allowBinary` when this option is set to `true` the `isPrintable` check will be disabled. Because of this any valid base64 formatted string will be decoded.

#### .json()

```javascript
parse().json([options = {}])
```

Converts the selected value from and to a JSON string.

- `options`
  - `defaultValue` will be returned whenever no valid JSON string is selected.

#### .spec()

```javascript
parse().spec(specification)
```

A specification is an object that has the desired properties of the target format, where the values are the parsers that generate the value to store with this property. This allows a source format to be converted to the desired format using `parse-js`.

- `specification` an object containing `key` - `parser` combinations.

Example:
```javascript
parse().spec({
    one: parse('two'),
    two: parse('one').number(),
    three: parse('four').array(),
    nested: {
        one: parse('one'),
        two: parse('four').json()
    }
}).parse({ one: '15.333.23', two: 'two', four: '[1,2,3,4,5]' });
// {
//     one: 'two',
//     two: 15333.23,
//     three: [1, 2, 3, 4, 5],
//     nested: {
//         one: '15.333.23',
//         two: [1, 2, 3, 4, 5]
//     }
// }
```

#### .multilingual()

```javascript
parse().multilingual(languages)
```

Will group keys with language suffixes as defined by `group()`

- `languages` configures the languages that are supported. This option can also be set using the `setOption()` method of the parse-js instance or `Parse.setOption()`. When using `setOption()` this option is configured using the key `multilingual.languages`.

Example:

```javascript
parse().multilingual(['en', 'nl', 'fr', 'de']).parse({
    keyEn: 'english text',
    keyNl: 'dutch text'
});
// {
//     key: { en: 'english text', nl: 'dutch text' }
// }
```

#### .stripPrefix()

```javascript
parse().stripPrefix(prefix)
```

Selects keys that start with `prefix` and removes that `prefix` from the target object.

- `prefix` the prefix that keys should contain and that will be removed.

Example:

```javascript
parse().stripPrefix('test').parse({
    atest: 'value-1',
    test1: 'value-2',
    test2: 'value-3'
});
// { 1: 'value-2', 2: 'value-3' }
```

## Contribute

We really appreciate any contribution you would like to make, so don't
hesitate to report issues or submit pull requests.

## License

This project is released under a MIT license.

## About us

If you would like to know more about us, be sure to have a look at [our website](https://www.ambassify.com), or our Twitter accounts [Ambassify](https://twitter.com/Ambassify), [Sitebase](https://twitter.com/Sitebase), [JorgenEvens](https://twitter.com/JorgenEvens)
