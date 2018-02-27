//
// Buffer is exported as a global object 
// so no need to save the exports into a variable
//
//
// Create a buffer with preallocated size
//
var buf1 = new Buffer(1024);
assert.ok(buf1.size() === 1024);
console.log("Create buffer of size " + buf1.size());

//
// Create a buffer from an array of bytes
//
var buf2 = new Buffer([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
assert.ok(buf2.size() === 10);
console.log("Create buffer of size " + buf2.size());
//
// Check content
//
for (var i = 0; i < 10; i++) {
  assert.ok(i == buf2[i]);
}
console.log("Buffer content checked");
//
// Create a buffer from a string
//
var buf3 = new Buffer("The Quick Brown Fox.");
assert.ok(buf3.size() === 20);
console.log("Buffer size checked");

//
// Create a buffer from another buffer
//
var buf4 = new Buffer(buf3);
assert.ok(buf4.size() === 20);
assert.ok(buf4.toString() === buf3.toString());
console.log("Buffer equality checked");
//
// Assign to a buffer from a String
//
buf4.fromString("Ate The Lazy Dog");
assert.ok(buf4.size() === 16);
assert.ok(buf4.toString() === "Ate The Lazy Dog");
console.log("Buffer fromString checked");
//
// Assign to a buffer from an array
//
buf4.fromArray([1, 2, 3, 4, 5, 6, 7, 8, 9, 0]);
assert.ok(buf4.size() === 10);
console.log("Buffer fromArray checked");

//
// Assign to a buffer from another buffer
//
buf4.fromBuffer(buf2);
assert.ok(buf2.equals(buf4));
console.log("Buffer assignment checked");
