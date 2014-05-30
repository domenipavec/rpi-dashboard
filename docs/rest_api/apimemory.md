# /api/memory

Provides info about system memory. All values are in kB.

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

Total amount of physical RAM in kB.

### /api/memory/used

Amount of used physical RAM in kB.

### /api/memory/free

Amount of free physical RAM in kB.

### /api/memory/buffers

Amount of physical RAM in kB used for file buffers.

### /api/memory/cached

Amount of physical RAM in kB used as cache memory.

### /api/memory/swap/total

Total amount of swap in kB.

### /api/memory/swap/used

Amount of used swap in kB.

### /api/memory/swap/free

Amount of free swap in kB.
