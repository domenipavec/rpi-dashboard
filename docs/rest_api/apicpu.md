# /api/cpu

Provides info about central processing unit, including usage, load, uptime and temperature.

Example:
```
{
	"usage":	{
		"total":	9242512,
		"busy":	18907,
		"user":	10071,
		"nice":	0,
		"system":	8644,
		"idle":	9223054,
		"iowait":	551,
		"irq":	68,
		"softirq":	124
	},
	"loadavg":	{
		"one":	0.010000,
		"five":	0.150000,
		"ten":	0.130000
	},
	"uptime":	92891.250000,
	"temperature":	46.540000
}
```

## /api/cpu/usage

Provides time information that the processor spent performing different kinds of work. Time units are in USER_HZ or Jiffies (typically hundredths of a second).

### /api/cpu/usage/total

Total cpu time, should match uptime approximately. This is a sum of all other values except busy.

### /api/cpu/usage/busy

Total time that cpu spent doing some work. This is a sum of all other values except total, idle and iowait.

### /api/cpu/usage/user

Time that cpu was executing normal processes executing in user mode.

### /api/cpu/usage/nice

Time that cpu was executing niced processes executing in user mode.

### /api/cpu/usage/system

Time that cpu was executing processes executing in kernel mode.

### /api/cpu/usage/idle

Time that cpu was not doing anything.

### /api/cpu/usage/iowait

Time that cpu spent waiting for IO to finish.

### /api/cpu/usage/irq

Time that cpu spent servicing interrupts.

### /api/cpu/usage/softirq

Time that cpu spent servicing softirqs.

### /api/cpu/loadavg/one,five,ten

Provides values measuring CPU and IO utilization of the last one, five or ten minute periods.

### /api/cpu/temperature

Temperature of the cpu in degrees Celsius.

