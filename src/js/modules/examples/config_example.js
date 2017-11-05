#!/usr/bin/oss_js

"use-strict";

const cfg = require("config");
const assert = require("assert");
const opt = require("getopt");
const console = require("console");

var example1 = function()
{
  // This example reads the configuration file 'example.cfg' and displays
  // some of its contents.

  var config = new cfg.Config();
  assert(config.open(opt.argv[2]));
  var name = config.getString("name");
  assert(typeof name === "string")

  var books = config.getSetting("inventory.books");
  assert(typeof books === "object");

  var count = books.size();
  assert (count > 0);
  for (var i = 0; i < count; i++)
  {
    var element = books.getElement(i);
    var title = element.getString("title");
    var author = element.getString("author");
    var price = element.getFloat("price");
    var qty = element.getInt("qty");
    console.log(title +  " | " + author + " | " + price + " | " + qty);
  }

  var movies = config.getSetting("inventory.movies");
  assert(typeof movies === "object");
  count = movies.size();
  assert (count > 0);
  for (var i = 0; i < count; i++)
  {
    var element = movies.getElement(i);
    var title = element.getString("title");
    var media = element.getString("media");
    var price = element.getFloat("price");
    var qty = element.getInt("qty");
    console.log(title +  " | " + media + " | " + price + " | " + qty);
  }
}

var example2 = function()
{
  const output_file = "updated.cfg";
  var config = new cfg.Config();
  assert(config.open(opt.argv[2]));
  
  var root = config.self();
  assert(typeof root === "object");
  
  var inventory;
  if (config.exists("inventory"))
  {
    inventory = config.getSetting("inventory");
  }
  else
  {
    inventory = config.addGroup("inventory");
  }
    
  assert(typeof inventory === "object");
  
  var movies;
  if (inventory.exists("movies"))
  {
    movies = inventory.getSetting("movies");
  }
  else
  {
    movies = inventory.addGroup("movies");
  }
  assert(typeof movies === "object");
  
  /* Create the new movie entry. */
  var movie = movies.addGroup();
  movie.addString("title").set("Buckaroo Banzai");
  movie.addString("media").set("DVD");
  movie.addFloat("price").set(12.99);
  movie.addInt("qty").set(20);
  
  assert(config.save(output_file));
}

var example3 = function()
{
  // This example constructs a new configuration in memory and writes it to
  // 'newconfig.cfg'.
  
  var config = new cfg.Config();
  var root = config.self();
  
  var address = root.addGroup("address");
  
  // The long way
  var element;
  street = address.addString("street");
  street.set("1 Woz Way");
  assert(street.getString() === "1 Woz Way");
  assert(address.getString("street") === "1 Woz Way");
  
  // The short way
  address.addString("city").set("San Jose");
  address.addString("state").set("CA");
  address.addInt("sip").set(95110);
  
  var array = root.addArray("numbers");
  for (var i = 0; i < 10; i++)
  {
    array.addInt().set(10 * i);
  }
  
  for (var i = 0; i < array.size(); i++)
  {
    var element = array.getElement(i);
    console.log(element.get());
  }
  config.save("newconfig.cfg");
  
}

example1();
example2();
example3();

