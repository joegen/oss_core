
DHT test:
========
dht.h -> common file for the test
driver -> dhttest.c; dht.c is the code on each node.
the files ./hosts and ./blocks are needed for this test to run. The scripts needed to parse the output is in the directory scripts
The test also depends on a few #defines in the dht.h file, change them accordingly.

Mail box has some description of the test + some initial results.

The issue in the line below is fixed :)
Maclicious nodes: the malicious nodes in the current implementation just deny access to the data they store and if they are on the way of a req from  source to destination, they drop the request. BUT, this needs to be changed as this is not the exact type of malice we need for our paper.. we need the intermediate nodes to be able to respond to the requests themselves and the root node to be non-malicious. But then, this would be very similar to the implementation in the identity directory... look more into this.

The scripts for dht:
-------------------
script to parse the amount of sccuessful lookup is succ-lookups.pl


ISSUES TO FIX IN DHT:
It looks like the amount of hijack is way too much ... find out what is
happening by printing the hijackers and the source
