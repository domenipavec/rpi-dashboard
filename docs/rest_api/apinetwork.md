# /api/network

Provides information about network interfaces available in the system.

Default is to show information about all interfaces. You can filter which interfaces you want shown using iface in query string. Separate multiple interface with '|'. E.g. /api/network?iface=eth0|lo

Example:
```
{
	"packets":	{
		"rx":	{
			"eth0":	7130,
			"lo":	253
		},
		"tx":	{
			"eth0":	3391,
			"lo":	253
		},
		"total":	{
			"eth0":	10521,
			"lo":	506
		}
	},
	"bytes":	{
		"rx":	{
			"eth0":	674288,
			"lo":	13403
		},
		"tx":	{
			"eth0":	615422,
			"lo":	13403
		},
		"total":	{
			"eth0":	1289710,
			"lo":	26806
		}
	},
	"list":	{
		"eth0":	{
			"mac":	"b8:27:eb:ef:a2:fa",
			"mtu":	1500,
			"ip":	{
				"address":	"172.23.163.15",
				"broadcast":	"172.23.163.255",
				"netmask":	"255.255.255.0"
			}
		},
		"lo":	{
			"mac":	"00:00:00:00:00:00",
			"mtu":	16436,
			"ip":	{
				"address":	"127.0.0.1",
				"broadcast":	"0.0.0.0",
				"netmask":	"255.0.0.0"
			}
		}
	}
}
```

### /api/network/packets/rx,tx,total

Number of packets transmitted (tx), received (rx) and sum (total) since boot. Values are provided for each interface separately.

### /api/network/bytes/rx,tx,total

Number of bytes transmitted (tx), received (rx) and sum (total) since boot. Values are provided for each interface separately.

## /api/network/list

Returns object of interfaces and provides their basic information.

### /api/network/list/$ifname/mac

Mac address of the interface.

### /api/network/list/$ifname/mtu

Max transmission unit of the interface.

### /api/network/list/$ifname/ip/address,broadcast,netmask

IPv4 address, broadcast address and netmask information of the interface if configured.

### /api/network/list/$ifname/ipv6

Only available if IPv6 is enabled.
Returns json array of objects. Each object is an ipv6 address configured on the interface and contains address, prefix and scope.
