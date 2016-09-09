"use strict";

const assert = require("assert");
try
{
    assert.ok(true, "Well done!");
    assert.ok(false, "Oopsie!");
}
catch(e)
{
    print(e.message);
}
