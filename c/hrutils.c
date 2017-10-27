#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <linux/if.h>
#include <linux/if_link.h>
#include <linux/if_packet.h>
#include <Python.h>

// dictionary keys
#define NAME_KEY			"name"
#define FLAGS_KEY			"flags"
#define ADDRESS_KEY			"address"
#define ADDRESS6_KEY		"address6"
#define NETMASK_KEY			"netmask"
#define BROADCAST_KEY		"broadcast"
#define MAC_KEY				"mac"
#define TXPACKETS_KEY		"txpackets"
#define TXBYTES_KEY			"txbytes"
#define TXERRORS_KEY		"txerrors"
#define RXPACKETS_KEY		"rxpackets"
#define RXBYTES_KEY			"rxbytes"
#define RXERRORS_KEY		"rxerrors"
#define AUTO_KEY			"auto"
#define TYPE_KEY			"type"

//*****************************************************************************
// read_file
//*****************************************************************************
char **read_file(char *file_name, int *num_lines)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t n;

	// initialize the number of lines
	*num_lines = 0;
	
	// open the file
	FILE *fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		return NULL;
	}

	// read the lines of the file
	int num_allocated_lines = 20;
	char **lines = (char **)malloc(sizeof(char *) * num_allocated_lines);
	while ((n = getline(&line, &len, fp)) != -1)
	{
		if (n > 0 && line[n - 1] == '\n')
		{
			line[--n] = 0;
		}
		if (n > 0 && line[0] != '#')
		{
			if (*num_lines == num_allocated_lines)
			{
				num_allocated_lines += 20;
				lines = (char **)realloc(lines, sizeof(char *) * num_allocated_lines);
			}
			lines[(*num_lines)++] = line;
			line = NULL;
			len = 0;
		}
	}

	// clean up
	fclose(fp);
	if (line)
	{
		free(line);
	}
	
	// return the array of lines
	return lines;
}

//*****************************************************************************
// free_lines
//*****************************************************************************
void free_lines(char **lines, int num_lines)
{
	for (int i = 0; i < num_lines; i++)
	{
		free(lines[i]);
	}
	free(lines);
}

//*****************************************************************************
// find_interface
//*****************************************************************************
PyObject *find_interface(PyObject *list, char *name)
{
	for (int i = 0; i < PyList_Size(list); i++)
	{
		PyObject *interface = PyList_GetItem(list, i);
		PyObject *str = PyDict_GetItemString(interface, NAME_KEY);
		PyObject *encoded_str = PyUnicode_AsUTF8String(str);
		const char *interface_name = PyBytes_AsString(encoded_str);
		if (strcmp(interface_name, name) == 0)
		{
			return interface;
		}
	}
	return NULL;
}

//*****************************************************************************
// get_host_name
//*****************************************************************************
static PyObject* get_host_name(PyObject *self, PyObject *args)
{
	char name[64];
	int result = gethostname(name, 64);
	if (result == 0)
	{
		return Py_BuildValue("s", name);
	}
	Py_RETURN_NONE;
}

//*****************************************************************************
// set_host_name
//*****************************************************************************
static PyObject* set_host_name(PyObject *self, PyObject *args)
{
	const char* name;
	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return NULL;
	}
	int result = sethostname(name, strlen(name));
	return Py_BuildValue("i", result);
}

//*****************************************************************************
// get_domain_name
//*****************************************************************************
static PyObject* get_domain_name(PyObject *self, PyObject *args)
{
	char name[64];
	int result = getdomainname(name, 64);
	if (result == 0)
	{
		return Py_BuildValue("s", name);
	}
	Py_RETURN_NONE;
}

//*****************************************************************************
// set_domain_name
//*****************************************************************************
static PyObject* set_domain_name(PyObject *self, PyObject *args)
{
	const char* name;
	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return NULL;
	}
	int result = setdomainname(name, strlen(name));
	return Py_BuildValue("i", result);
}

//*****************************************************************************
// get_interfaces
//*****************************************************************************
static PyObject* get_interfaces(PyObject *self, PyObject *args)
{
	struct ifaddrs *ifaddr;
	char address[NI_MAXHOST];

	// get the linked list of interfaces
	if (getifaddrs(&ifaddr) == -1)
	{
		return Py_BuildValue("i", -1);
	}

	// traverse the linked list of interfaces
	PyObject *list = PyList_New(0);
	for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL) continue;
		if (ifa->ifa_addr->sa_family != AF_PACKET &&
			ifa->ifa_addr->sa_family != AF_INET &&
			ifa->ifa_addr->sa_family != AF_INET6)
		{
			continue;
		}

		// look for an interface dictionary with this name
		PyObject *interface = find_interface(list, ifa->ifa_name);

		// create a new interface dictionary if not found
		if (interface == NULL)
		{
			interface = PyDict_New();
			PyDict_SetItemString(interface, NAME_KEY, Py_BuildValue("s", ifa->ifa_name));
			PyDict_SetItemString(interface, FLAGS_KEY, Py_BuildValue("i", ifa->ifa_flags));
			PyDict_SetItemString(interface, ADDRESS_KEY, Py_BuildValue("s", ""));
			PyDict_SetItemString(interface, ADDRESS6_KEY, Py_BuildValue("s", ""));
			PyDict_SetItemString(interface, NETMASK_KEY, Py_BuildValue("s", ""));
			PyDict_SetItemString(interface, BROADCAST_KEY, Py_BuildValue("s", ""));
			PyDict_SetItemString(interface, MAC_KEY, Py_BuildValue("s", ""));
			PyDict_SetItemString(interface, TXPACKETS_KEY, Py_BuildValue("i", 0));
			PyDict_SetItemString(interface, TXBYTES_KEY, Py_BuildValue("i", 0));
			PyDict_SetItemString(interface, TXERRORS_KEY, Py_BuildValue("i", 0));
			PyDict_SetItemString(interface, RXPACKETS_KEY, Py_BuildValue("i", 0));
			PyDict_SetItemString(interface, RXBYTES_KEY, Py_BuildValue("i", 0));
			PyDict_SetItemString(interface, RXERRORS_KEY, Py_BuildValue("i", 0));
			PyDict_SetItemString(interface, AUTO_KEY, Py_BuildValue("O", Py_False));
			PyDict_SetItemString(interface, TYPE_KEY, Py_BuildValue("s", ""));
			PyList_Append(list, interface);
		}

		// set the addresses
		switch (ifa->ifa_addr->sa_family)
		{
			case AF_PACKET:
				if ((ifa->ifa_flags & IFF_LOOPBACK) == 0)
				{
					struct sockaddr_ll *mac = (struct sockaddr_ll *)ifa->ifa_addr;
					char *addr = address + sprintf(address, "%02x", mac->sll_addr[0]);
					for (int i = 1; i < mac->sll_halen; i++)
					{
						addr += sprintf(addr, ":%02x", mac->sll_addr[i]);
					}
					PyDict_SetItemString(interface, MAC_KEY, Py_BuildValue("s", address));
				}
				if (ifa->ifa_data != NULL)
				{
					struct rtnl_link_stats *stats = (struct rtnl_link_stats *)ifa->ifa_data;
					PyDict_SetItemString(interface, TXPACKETS_KEY, Py_BuildValue("i", stats->tx_packets));
					PyDict_SetItemString(interface, TXBYTES_KEY, Py_BuildValue("i", stats->tx_bytes));
					PyDict_SetItemString(interface, TXERRORS_KEY, Py_BuildValue("i", stats->tx_errors));
					PyDict_SetItemString(interface, RXPACKETS_KEY, Py_BuildValue("i", stats->rx_packets));
					PyDict_SetItemString(interface, RXBYTES_KEY, Py_BuildValue("i", stats->rx_bytes));
					PyDict_SetItemString(interface, RXERRORS_KEY, Py_BuildValue("i", stats->rx_errors));
				}
				break;
			case AF_INET:
				getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), address, NI_MAXHOST,
							NULL, 0, NI_NUMERICHOST);
				PyDict_SetItemString(interface, ADDRESS_KEY, Py_BuildValue("s", address));
				getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in), address, NI_MAXHOST,
							NULL, 0, NI_NUMERICHOST);
				PyDict_SetItemString(interface, NETMASK_KEY, Py_BuildValue("s", address));
				getnameinfo(ifa->ifa_ifu.ifu_broadaddr, sizeof(struct sockaddr_in), address, NI_MAXHOST,
							NULL, 0, NI_NUMERICHOST);
				PyDict_SetItemString(interface, BROADCAST_KEY, Py_BuildValue("s", address));
				break;
			case AF_INET6:
				getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), address, NI_MAXHOST,
							NULL, 0, NI_NUMERICHOST);
				char *percent = strchr(address, '%');
				if (percent != NULL)
				{
					*percent = 0;
				}
				PyDict_SetItemString(interface, ADDRESS6_KEY, Py_BuildValue("s", address));
				break;
			default:
				continue;
		}
	}

	// free the linked list of interfaces
	freeifaddrs(ifaddr);

	// process the /etc/network/interfaces file
	int num_lines;
	char **lines = read_file("/etc/network/interfaces", &num_lines);
	for (int i = 0; i < num_lines; i++)
	{
		int n = strlen(lines[i]);
		if (n > 5 && strncmp(lines[i], "auto ", 5) == 0)
		{
			char *name = lines[i] + 5;
			PyObject *interface = find_interface(list, name);
			if (interface != NULL)
			{
				PyDict_SetItemString(interface, AUTO_KEY, Py_BuildValue("O", Py_True));
			}
		}
		if (n > 6 && strncmp(lines[i], "iface ", 6) == 0)
		{
			char name[50], inet[50], type[50];
			sscanf(lines[i] + 6, "%s %s %s", name, inet, type);
			PyObject *interface = find_interface(list, name);
			if (interface != NULL)
			{
				PyDict_SetItemString(interface, TYPE_KEY, Py_BuildValue("s", type));
			}
		}
	}
	free_lines(lines, num_lines);
	
	// return the list of interfaces
	return list;
}

//*****************************************************************************
// entry points table
//*****************************************************************************
static PyMethodDef _hrutils_methods[] =
{
	{
		"get_host_name", get_host_name, METH_NOARGS,
		"Gets the host name."
	},
	{
		"set_host_name", set_host_name, METH_VARARGS,
		"Sets the host name."
	},
	{
		"get_domain_name", get_domain_name, METH_NOARGS,
		"Gets the domain name."
	},
	{
		"set_domain_name", set_domain_name, METH_VARARGS,
		"Sets the domain name."
	},
	{
		"get_interfaces", get_interfaces, METH_NOARGS,
		"Gets a list of the available network interfaces."
	},
	{NULL, NULL, 0, NULL}
};

//*****************************************************************************
// module definition
//*****************************************************************************
static struct PyModuleDef _hrutils_definition =
{ 
	PyModuleDef_HEAD_INIT,
	"_hrutils",
	"Home Router utilities.",
	-1, 
	_hrutils_methods
};

//*****************************************************************************
// module initialization
//*****************************************************************************
PyMODINIT_FUNC PyInit__hrutils(void)
{
	// initialze and create the module
	Py_Initialize();
	PyObject *module = PyModule_Create(&_hrutils_definition);
	
	// declare the global constants
	PyModule_AddIntConstant(module, "IFF_ALLMULTI", IFF_ALLMULTI);
	PyModule_AddIntConstant(module, "IFF_AUTOMEDIA", IFF_AUTOMEDIA);
	PyModule_AddIntConstant(module, "IFF_BROADCAST", IFF_BROADCAST);
	PyModule_AddIntConstant(module, "IFF_DEBUG", IFF_DEBUG);
	PyModule_AddIntConstant(module, "IFF_DORMANT", IFF_DORMANT);
	PyModule_AddIntConstant(module, "IFF_DYNAMIC", IFF_DYNAMIC);
	PyModule_AddIntConstant(module, "IFF_ECHO", IFF_ECHO);
	PyModule_AddIntConstant(module, "IFF_LOOPBACK", IFF_LOOPBACK);
	PyModule_AddIntConstant(module, "IFF_LOWER_UP", IFF_LOWER_UP);
	PyModule_AddIntConstant(module, "IFF_MASTER", IFF_MASTER);
	PyModule_AddIntConstant(module, "IFF_MULTICAST", IFF_MULTICAST);
	PyModule_AddIntConstant(module, "IFF_NOARP", IFF_NOARP);
	PyModule_AddIntConstant(module, "IFF_NOTRAILERS", IFF_NOTRAILERS);
	PyModule_AddIntConstant(module, "IFF_POINTOPOINT", IFF_POINTOPOINT);
	PyModule_AddIntConstant(module, "IFF_PORTSEL", IFF_PORTSEL);
	PyModule_AddIntConstant(module, "IFF_PROMISC", IFF_PROMISC);
	PyModule_AddIntConstant(module, "IFF_RUNNING", IFF_RUNNING);
	PyModule_AddIntConstant(module, "IFF_SLAVE", IFF_SLAVE);
	PyModule_AddIntConstant(module, "IFF_UP", IFF_UP);
	
	// return the module
	return module;
}
