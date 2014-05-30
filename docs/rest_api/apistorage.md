# /api/storage

Provides info about storage devices on the system.

Example:
```
{
	"list":	[{
			"device":	"/dev/mmcblk0p6",
			"mount":	"/",
			"filesystem":	"ext4",
			"size":	6424158208,
			"used":	1909088256,
			"available":	4188712960,
			"use":	0.347975
		}, {
			"device":	"/dev/mmcblk0p5",
			"mount":	"/boot",
			"filesystem":	"vfat",
			"size":	58662912,
			"used":	18980864,
			"available":	39682048,
			"use":	0.323558
		}],
	"throughput":	{
		"mmcblk0":	{
			"total":	134906880,
			"read":	102949888,
			"written":	31956992
		},
		"mmcblk0p1":	{
			"total":	358912,
			"read":	358912,
			"written":	0
		},
		"mmcblk0p2":	{
			"total":	5120,
			"read":	5120,
			"written":	0
		},
		"mmcblk0p5":	{
			"total":	115200,
			"read":	115200,
			"written":	0
		},
		"mmcblk0p6":	{
			"total":	134108160,
			"read":	102151168,
			"written":	31956992
		}
	}
}
```

## /api/storage/list

Returns json list of all mounted devices. The following values are available:

### device
Name of the mounted device.

### mount
Mounting point in the filesystem, that the device is mounted on.

### filesystem
Filesystem used on the device.

### size
Total size of the device in bytes.

### used
Used size on the device in bytes.

### available
Available size on the device in bytes.

### use
Fraction of the device that is used. This is (size-available)/size.

## /api/storage/throughput

Returns a json object of all physical devices with throughput data since boot.

### /api/storage/throughput/$device/total

Total bytes written or read for the device.

### /api/storage/throughput/$device/read

Bytes read from the device.

### /api/storage/throughput/$device/read

Bytes written to the device.
