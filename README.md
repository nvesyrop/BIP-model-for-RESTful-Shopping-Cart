# BIP-model-for-RESTful-Shopping-Cart-

A BIP model developed for the needs of an academic publication titled "Correct by Construction Web Service Architecture" (link: http://depend.csd.auth.gr/publications.php), in collaboration with the DSG group of the Department of Informatics, Aristotle University of Thessaloniki.

It pertains to a RESTful client-server Web Service architecture for a Shopping Cart. The Web Service provides the following operations:

a) GET availItems, for getting the available items.
 	- The request contains a list of available items.
	
b) PUT cartsItems/cartId/itemId, for adding an item to a shopping cart
	- The request contains a cartId and an itemId
	- If the item exists, it is removed from the available items and added to the cart.
	- If the item doesn't exist, the response contains an error code for the exception.
  

In order to compile an executable for the BIP model from BIP source code, you must fullfil the following prerequisites: 

1)OS: LINUX (tested on an Ubuntu 12.04 x64 machine)

2)The new versions of the BIP compiler + engine (link: http://www-verimag.imag.fr/New-BIP-tools.html) 

Then you have to follow the following steps: 

1. Place the source files to some location <root_loc> 
2. Run the build.sh ($ cd <root_loc> and then $ ./build.sh) 
3. Run the generated executable ($ ./build/system -i) 


