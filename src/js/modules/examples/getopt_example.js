"use-strict";

console.log(1);
try {
  const opt = require("getopt");
} catch (e) {
  console.log(e);
}
console.log(2);

var aflag = 0;
var bflag = 0;
var cvalue = "";

opt.opterr = 0; // disable printing of errors

var c;
while ((c = opt.getopt("abc:")) != -1) {
  switch (String.fromCharCode(c)) {
    case "a":
      aflag = 1;
      break;
    case "b":
      bflag = 1;
      break;
    case "c":
      cvalue = opt.optarg;
      break;
    case "?":
      if (String.fromCharCode(opt.optopt) === "c") {
        console.error("Option -c requires an argument.");
      } else {
        console.error("Unknown option -" + String.fromCharCode(opt.optopt));
      }
      break;
    default:
      system.exit(1);
  }
}

console.log("aflag: " + aflag);
console.log("bflag: " + bflag);
console.log("cvalue: " + cvalue);