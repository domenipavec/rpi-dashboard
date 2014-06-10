# /api/memory

Provides info about system memory. All values are in bytes.

Example:
```
{
	"total":	497504,
	"used":	130204,
	"free":	367300,
	"buffers":	17152,
	"cached":	79180,
	"swap":	{
		"total":	102396,
		"used":	0,
		"free":	102396
	}
}
```

### /api/memory/total

Total amount of physical RAM.

### /api/memory/used

Amount of used physical RAM.
This is a sum of processes, buffers and cached.

### /api/memory/free

Amount of free physical RAM.

### /api/memory/processes

Amount of physical RAM used by processes.

### /api/memory/buffers

Amount of physical RAM used for file buffers.

### /api/memory/cached

Amount of physical RAM used as cache memory.

### /api/memory/swap/total

Total amount of swap.

### /api/memory/swap/used

Amount of used swap.

### /api/memory/swap/free

Amount of free swap.
