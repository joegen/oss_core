function CustomTransformer(parse, reverse) {
    if( !(this instanceof CustomTransformer) ) {
        return this.transform(new CustomTransformer(parse, reverse));
    }

    this.parse = parse;
    this.reverse = reverse;
}

module.exports = CustomTransformer;
