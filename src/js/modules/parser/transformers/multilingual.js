const SNAKE_CASE = 'SNAKE_CASE';
const CAMEL_CASE = 'CAMEL_CASE';

function ucfirst(str) {
    return str.replace(/^[a-z]/, l => l.toUpperCase());
}

const SUFFIXES = {
    SNAKE_CASE: {
        create: language => '_' + language,
        restore: suffix => suffix.replace(/^_/, '')
    },
    CAMEL_CASE: {
        create: language => ucfirst(language),
        restore: suffix => suffix.toLowerCase()
    }
};

function compileRegex(languages, suffixer) {
    const langSuffix = languages
        .map(l => suffixer(l))
        .join('|');

    return new RegExp(`(.+)(${langSuffix})$`);
}

function createRestorer(casing) {
    if (typeof casing != 'object')
        casing = SUFFIXES[casing];

    if (!casing || typeof casing.restore != 'function')
        throw new Error('Invalid multilingual suffixer specified');

    return casing.restore;
}

function createSuffixer(casing) {
    if (typeof casing != 'object')
        casing = SUFFIXES[casing];

    if (!casing || typeof casing.create != 'function')
        throw new Error('Invalid multilingual suffixer specified');

    return casing.create;
}

function MultilingualTransformer(languages, { languageCase } = {}) {
    languages = languages || this.getOption('multilingual.languages') || [];
    languageCase = languageCase || this.getOption('multilingual.languageCase') || CAMEL_CASE;

    const suffixer = createSuffixer(languageCase);
    const restorer = createRestorer(languageCase);
    const regex = compileRegex(languages, suffixer);

    return this.match(regex)
        .group(regex, 1, 2)
        .map(p => p.rename(restorer, suffixer));
}

MultilingualTransformer.SNAKE_CASE = SNAKE_CASE;
MultilingualTransformer.CAMEL_CASE = CAMEL_CASE;

module.exports = MultilingualTransformer;
