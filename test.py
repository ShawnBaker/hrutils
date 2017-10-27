import hrutils

print(dir(hrutils))
print('Host name: ' + hrutils.get_host_name())
print('Domain name: ' + hrutils.get_domain_name())
print('Interfaces:')
name = None
interfaces = hrutils.get_interfaces()
for interface in interfaces:
	print(interface)
	name = interface.name
interface = hrutils.get_interface(name)
print('Interface ' + name + ':')
print(interface)
