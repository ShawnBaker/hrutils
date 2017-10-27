Home Router Utilities
=====================

This is a C extension that provides the ability to read and write various
Linux networking configuration values. At this time it has only been tested
with Python 3.5 on Ubuntu Server 17.04.

Copyright and License
---------------------

Copyright |copy| 2017 Shawn Baker using the `MIT License <https://opensource.org/licenses/MIT>`_

Entry Points
------------

	- get_host_name()
	- set_host_name(name)
	- get_domain_name()
	- set_domain_name(name)
	- get_interfaces()
	- get_interface(name)

Constants
---------

	- IFF_ALLMULTI
	- IFF_AUTOMEDIA
	- IFF_BROADCAST
	- IFF_DEBUG
	- IFF_DORMANT
	- IFF_DYNAMIC
	- IFF_ECHO
	- IFF_LOOPBACK
	- IFF_LOWER_UP
	- IFF_MASTER
	- IFF_MULTICAST
	- IFF_NOARP
	- IFF_NOTRAILERS
	- IFF_POINTOPOINT
	- IFF_PORTSEL
	- IFF_PROMISC
	- IFF_RUNNING
	- IFF_SLAVE
	- IFF_UP

Classes
-------

Interface
	* fields
		- name = ""
		- flags = 0
		- address = ""
		- address6 = ""
		- netmask = ""
		- broadcast = ""
		- mac = ""
		- txpackets = 0
		- txbytes = 0
		- txerrors = 0
		- rxpackets = 0
		- rxbytes = 0
		- rxerrors = 0
		- auto = False
		- type = ""
	* properties
		- broadcast = False
		- dhcp = False
		- loopback = False
		- manual = False
		- multicast = False
		- running = False
		- static = False
		- up = False
