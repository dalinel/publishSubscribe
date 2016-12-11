# Simple publish subscribe

This program is a simple publish-subscribe. It is made of 2 publishers, 3 subscribers and one mediator.

##Setup

###How to run it ?

	*cd "path to the publish_subscribe dir"
	*make
	*./main

	
###How to stop it ?

	*ctrl + c


## Details

- mediator is cyclic and run every 1 second
- publisher1 is cyclic and run every 5 seconds
- publisher2 is cyclic and run every 3 seconds
- subscriber1 is cyclic and run every 7 seconds
- subscriber2 is cyclic and run every 8 seconds
- subscriber3 is cyclic and run every 9 seconds

publisher1 send lower case char
publisher 2 send upper case char

Lower case and upper case char are stored in the same circular buffer in the mediator.
There is one circular buffer per subscriber.

