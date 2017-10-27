import _hrutils

interfaces = None

IFF_ALLMULTI = _hrutils.IFF_ALLMULTI
IFF_AUTOMEDIA = _hrutils.IFF_AUTOMEDIA
IFF_BROADCAST = _hrutils.IFF_BROADCAST
IFF_DEBUG = _hrutils.IFF_DEBUG
IFF_DORMANT = _hrutils.IFF_DORMANT
IFF_DYNAMIC = _hrutils.IFF_DYNAMIC
IFF_ECHO = _hrutils.IFF_ECHO
IFF_LOOPBACK = _hrutils.IFF_LOOPBACK
IFF_LOWER_UP = _hrutils.IFF_LOWER_UP
IFF_MASTER = _hrutils.IFF_MASTER
IFF_MULTICAST = _hrutils.IFF_MULTICAST
IFF_NOARP = _hrutils.IFF_NOARP
IFF_NOTRAILERS = _hrutils.IFF_NOTRAILERS
IFF_POINTOPOINT = _hrutils.IFF_POINTOPOINT
IFF_PORTSEL = _hrutils.IFF_PORTSEL
IFF_PROMISC = _hrutils.IFF_PROMISC
IFF_RUNNING = _hrutils.IFF_RUNNING
IFF_SLAVE = _hrutils.IFF_SLAVE
IFF_UP = _hrutils.IFF_UP

def get_host_name():
	return _hrutils.get_host_name()

def set_host_name(name):
	return _hrutils.set_host_name(name)

def get_domain_name():
	return _hrutils.get_domain_name()

def set_domain_name(name):
	return _hrutils.set_domain_name(name)

def get_interfaces():
	global interfaces
	if not interfaces:
		refresh_interfaces()
	return interfaces

def refresh_interfaces():
	global interfaces
	interfaces = []
	_interfaces = _hrutils.get_interfaces()
	for _interface in _interfaces:
		interface = Interface(_interface)
		interfaces.append(interface)
	return interfaces

def get_interface(name):
	interfaces = get_interfaces()
	return next((interface for interface in interfaces if interface.name == name), None)

class Interface():

	name = ""
	flags = 0
	address = ""
	address6 = ""
	netmask = ""
	broadcast = ""
	mac = ""
	txpackets = 0
	txbytes = 0
	txerrors = 0
	rxpackets = 0
	rxbytes = 0
	rxerrors = 0
	auto = False
	type = ""

	def __init__(self, _interface):
	
		self.name = _interface['name']
		self.flags = _interface['flags']
		self.auto = _interface['auto']
		self.type = _interface['type']
		self.address = _interface['address']
		self.address6 = _interface['address6']
		self.netmask = _interface['netmask']
		self.broadcast_address = _interface['broadcast']
		self.mac = _interface['mac']
		self.txpackets = _interface['txpackets']
		self.txbytes = _interface['txbytes']
		self.txerrors = _interface['txerrors']
		self.rxpackets = _interface['rxpackets']
		self.rxbytes = _interface['rxbytes']
		self.rxerrors = _interface['rxerrors']

	def __repr__(self):
		return "{name:%s, flags:%d, auto:%s, type:%s, address:%s, address6:%s, netmask:%s, broadcast:%s, mac:%s, txpackets:%d, txbytes:%d, txerrors:%d, rxpackets:%d, rxbytes:%d, rxerrors:%d}" % (self.name, self.flags, self.auto, self.type, self.address, self.address6, self.netmask, self.broadcast_address, self.mac, self.txpackets, self.txbytes, self.txerrors, self.rxpackets, self.rxbytes, self.rxerrors)

	@property
	def broadcast(self):
		return self.type == 'broadcast'

	@property
	def dhcp(self):
		return self.type == 'dhcp'

	@property
	def loopback(self):
		return (self.flags & IFF_LOOPBACK) == IFF_LOOPBACK

	@property
	def manual(self):
		return self.type == 'manual'

	@property
	def multicast(self):
		return self.type == 'multicast'

	@property
	def running(self):
		return (self.flags & IFF_RUNNING) == IFF_RUNNING

	@property
	def static(self):
		return self.type == 'static'

	@property
	def up(self):
		return (self.flags & IFF_UP) == IFF_UP
