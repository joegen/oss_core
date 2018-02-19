module.exports = {
    parse: function parse(str) {
        try {
            return JSON.parse(str);
        } catch(e) {
            return null;
        }
    }
};
