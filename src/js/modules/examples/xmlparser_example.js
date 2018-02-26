var parser = require("xml-parser");
var assert = require("assert");

var expect = function(what) {
  expect._what = what;
  return expect;
}

expect.toEqual = function(val) {
  assert.deepEqual(expect._what, val, "ASSERT!");
}

expect.toBe = function(val) {
  assert(expect._what == val);
}

var it = function(what, func) {
  console.log(what);
  func();
}

var describe = function() {

  it("should parse all values as string, int, or float", function() {
    var xmlData = "<rootNode><tag>value</tag><intTag>045</intTag><floatTag>65.34</floatTag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": "value",
        "intTag": 45,
        "floatTag": 65.34
      }
    };

    var result = parser.parse(xmlData);
    expect(result).toEqual(expected);
  });

  it("should parse all values as string", function() {
    var xmlData = "<rootNode><tag>value</tag><intTag>045</intTag><floatTag>65.34</floatTag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": "value",
        "intTag": "045",
        "floatTag": "65.34"
      }
    };

    var result = parser.parse(xmlData, {
      textNodeConversion: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse all values of attributes as string", function() {
    var xmlData = "<rootNode><tag int='045' float='65.34'>value</tag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": {
          "#text": "value",
          "@_int": "045",
          "@_float": "65.34"
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false,
      textAttrConversion: false
    });

    expect(result).toEqual(expected);
  });

  it("should parse number values of attributes as number", function() {
    var xmlData = "<rootNode><tag int='045' float='65.34'>value</tag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": {
          "#text": "value",
          "@_int": 45,
          "@_float": 65.34
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false,
      ignoreNonTextNodeAttr: false,
      textAttrConversion: true
    });

    expect(result).toEqual(expected);
  });

  it("should parse number values as number if flag is set", function() {
    var xmlData = "<rootNode><tag>value</tag><intTag>045</intTag><intTag>0</intTag><floatTag>65.34</floatTag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": "value",
        "intTag": [45, 0],
        "floatTag": 65.34
      }
    };

    var result = parser.parse(xmlData, {
      textNodeConversion: true
    });
    expect(result).toEqual(expected);
  });


  it("should skip tag arguments", function() {
    var xmlData = "<rootNode><tag ns:arg='value'>value</tag><intTag ns:arg='value' ns:arg2='value2' >45</intTag><floatTag>65.34</floatTag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": "value",
        "intTag": 45,
        "floatTag": 65.34
      }
    };

    var result = parser.parse(xmlData);
    expect(result).toEqual(expected);
  });

  it("should ignore namespace and text node attributes", function() {
    var xmlData = "<root:node><tag ns:arg='value'>value</tag><intTag ns:arg='value' ns:arg2='value2' >45</intTag><floatTag>65.34</floatTag></root:node>";
    var expected = {
      "node": {
        "tag": {
          "@_arg": "value",
          "#text": "value"
        },
        "intTag": {
          "@_arg": "value",
          "@_arg2": "value2",
          "#text": 45
        },
        "floatTag": 65.34
      }
    };

    var result = parser.parse(xmlData, {
      ignoreNameSpace: true,
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse empty text Node", function() {
    var xmlData = "<rootNode><tag></tag></rootNode>";
    var expected = {
      "rootNode": {
        "tag": ""
      }
    };

    var result = parser.parse(xmlData);
    expect(result).toEqual(expected);
  });

  it("should parse self closing tags", function() {
    var xmlData = "<rootNode><tag ns:arg='value'/></rootNode>";
    var expected = {
      "rootNode": {
        "tag": {
          "@_ns:arg": "value"
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse single self closing tag", function() {
    var xmlData = "<tag arg='value'/>";
    var expected = {
      "tag": {
        "@_arg": "value"
      }
    };

    //console.log(parser.getTraversalObj(xmlData));
    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });



  it("should parse repeated nodes in array", function() {
    var xmlData = "<rootNode>" +
      "<tag>value</tag>" +
      "<tag>45</tag>" +
      "<tag>65.34</tag>" +
      "</rootNode>";
    var expected = {
      "rootNode": {
        "tag": ["value", 45, 65.34]
      }
    };

    var result = parser.parse(xmlData);
    expect(result).toEqual(expected);
  });

  it("should parse nested nodes in nested properties", function() {
    var xmlData = "<rootNode>" +
      "<parenttag>" +
      "<tag>value</tag>" +
      "<tag>45</tag>" +
      "<tag>65.34</tag>" +
      "</parenttag>" +
      "</rootNode>";
    var expected = {
      "rootNode": {
        "parenttag": {
          "tag": ["value", 45, 65.34]
        }
      }
    };

    var result = parser.parse(xmlData);
    expect(result).toEqual(expected);
  });

  it("should parse text nodes with value", function() {
    var xmlData = "<rootNode>" +
      "<parenttag attr1='some val' attr2='another val'>" +
      "<tag attr1='val'>value</tag>" +
      "<tag attr1='val' attr2='234'>45</tag>" +
      "<tag>65.34</tag>" +
      "</parenttag>" +
      "</rootNode>";
    var expected = {
      "rootNode": {
        "parenttag": {
          "tag": [{
            "@_attr1": "val",
            "#text": "value"
          }, {
            "@_attr1": "val",
            "@_attr2": "234",
            "#text": 45
          }, 65.34]
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse non-text nodes with value", function() {
    var xmlData = "<rootNode>" +
      "<parenttag attr1='some val' attr2='another val'>" +
      "<tag>value</tag>" +
      "<tag attr1='val' attr2='234'>45</tag>" +
      "<tag>65.34</tag>" +
      "</parenttag>" +
      "</rootNode>";
    var expected = {
      "rootNode": {
        "parenttag": {
          "@_attr1": "some val",
          "@_attr2": "another val",
          "tag": ["value", 45, 65.34]
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreNonTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse non-text nodes with value for repeated nodes", function() {
    var xmlData = "<rootNode>" +
      "<parenttag attr1='some val' attr2='another val'>" +
      "<tag>value</tag>" +
      "<tag attr1='val' attr2='234'>45</tag>" +
      "<tag>65.34</tag>" +
      "</parenttag>" +
      "<parenttag attr1='some val' attr2='another val'>" +
      "<tag>value</tag>" +
      "<tag attr1='val' attr2='234'>45</tag>" +
      "<tag>65.34</tag>" +
      "</parenttag>" +
      "</rootNode>";
    var expected = {
      "rootNode": {
        "parenttag": [{
          "@_attr1": "some val",
          "@_attr2": "another val",
          "tag": ["value", 45, 65.34]
        }, {
          "@_attr1": "some val",
          "@_attr2": "another val",
          "tag": ["value", 45, 65.34]
        }]
      }
    };

    var result = parser.parse(xmlData, {
      ignoreNonTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should preserve node value", function() {
    var xmlData = "<rootNode attr1=' some val ' name='another val'> some val </rootNode>";
    var expected = {
      "rootNode": {
        "@_attr1": " some val ",
        "@_name": "another val",
        "#text": " some val "
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse with attributes and value when there is single node", function() {
    var xmlData = "<rootNode attr1='some val' attr2='another val'>val</rootNode>";
    var expected = {
      "rootNode": {
        "@_attr1": "some val",
        "@_attr2": "another val",
        "#text": "val"
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse different tags", function() {
    var xmlData = "<tag.1>val1</tag.1><tag.2>val2</tag.2>";
    var expected = {
      "tag.1": "val1",
      "tag.2": "val2"
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false
    });
    expect(result).toEqual(expected);
  });

  it("should parse nested elements with attributes", function() {
    var xmlData = '<root>' +
      '<Meet date="2017-05-03" type="A" name="Meeting \'A\'">' +
      '<Event time="00:05:00" ID="574" Name="Some Event Name">' +
      '<User ID="1">Bob</User>' +
      '</Event>' +
      '</Meet>' +
      '</root>';
    var expected = {
      "root": {
        "Meet": {
          "@_date": "2017-05-03",
          "@_type": "A",
          "@_name": "Meeting 'A'",
          "Event": {
            "@_time": "00:05:00",
            "@_ID": "574",
            "@_Name": "Some Event Name",
            "User": {
              "@_ID": "1",
              "#text": "Bob"
            }
          }
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreTextNodeAttr: false,
      ignoreNonTextNodeAttr: false
    });

    expect(result).toEqual(expected);
  });

  it("should parse nested elements with attributes wrapped in array", function() {
    var xmlData = '<root>' +
      '<Meet date="2017-05-03" type="A" name="Meeting \'A\'">' +
      '<Event time="00:05:00" ID="574" Name="Some Event Name">' +
      '<User ID="1">Bob</User>' +
      '</Event>' +
      '</Meet>' +
      '</root>';
    var expected = {
      "root": {
        "Meet": {
          "$": {
            "date": "2017-05-03",
            "type": "A",
            "name": "Meeting 'A'"
          },
          "Event": {
            "$": {
              "time": "00:05:00",
              "ID": "574",
              "Name": "Some Event Name"
            },
            "User": {
              "$": {
                "ID": "1"
              },
              "#text": "Bob"
            }
          }
        }
      }
    };

    var result = parser.parse(xmlData, {
      attrPrefix: "",
      attrNodeName: "$",
      ignoreTextNodeAttr: false,
      ignoreNonTextNodeAttr: false
    });

    expect(result).toEqual(expected);
  });


  it("should intermediate traversable JS object which can later covert to JSON", function() {
    var xmlData = "<rootNode><tag></tag><tag>1</tag><tag>val</tag></rootNode>";

    var tobj = parser.getTraversalObj(xmlData);
    expect(tobj.parent).toBe(undefined);
    expect(tobj.tagname).toBe("!xml");
    expect(tobj.child.length).toBe(1);
    expect(tobj.child[0].parent.tagname).toBe("!xml");
    expect(tobj.child[0].tagname).toBe("rootNode");
    expect(tobj.child[0].val).toBe(undefined);
    expect(tobj.child[0].child.length).toBe(3);
    expect(tobj.child[0].child[0].parent).toBe(tobj.child[0].child[1].parent);
    expect(tobj.child[0].child[1].parent).toBe(tobj.child[0].child[2].parent);
    expect(tobj.child[0].child[0].val).toBe("");
    expect(tobj.child[0].child[1].val).toBe(1);
    expect(tobj.child[0].child[2].val).toBe("val");

    var expected = {
      "rootNode": {
        "tag": ["", 1, "val"]
      }
    };
    var jsobj = parser.convertToJson(tobj);
    expect(jsobj).toEqual(expected);
  });


  it("should skip namespace", function() {
    var xmlData = '<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" >' +
      '   <soapenv:Header>' +
      '      <cor:applicationID>dashboardweb</cor:applicationID>' +
      '      <cor:providerID>abc</cor:providerID>' +
      '   </soapenv:Header>' +
      '   <soapenv:Body>' +
      '      <man:getOffers>' +
      '         <man:customerId>' +
      '            <cor:msisdn>123456789</cor:msisdn>' +
      '         </man:customerId>' +
      '      </man:getOffers>' +
      '   </soapenv:Body>' +
      '</soapenv:Envelope>';
    var expected = {
      "Envelope": {
        "Header": {
          "applicationID": "dashboardweb",
          "providerID": "abc"
        },
        "Body": {
          "getOffers": {
            "customerId": {
              "msisdn": 123456789
            }
          }
        }
      }
    };

    var result = parser.parse(xmlData, {
      ignoreNameSpace: true
    });
    expect(result).toEqual(expected);
  });

};

describe();