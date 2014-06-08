# formatOptions

FormatOptions object specifies, how the value, indexed by **key**, will be formatted.

The argument to the function extends the defaults:
```javascript
    $.rpijs.formatDefaults = {
        key: [],
        rate: false,
        valueType: "none",
        decimals: 2
    };
```

You can change the default formatOptions by changing **$.rpijs.formatDefaults**.

## Available options

### key

A list of keys, that identify the value inside the returned object. If the request is for a value, this should be an empty list.

Example:
For formatting *swap/total* value in request for *memory*, that returns:
```javascript
{
	"total":	16826519552,
	"used":	4019175424,
	"free":	12807344128,
	"buffers":	135135232,
	"cached":	1279987712,
	"swap":	{
		"total":	239024992256,
		"used":	0,
		"free":	239024992256
	}
}
```
The key should be:
```javascript
["swap", "total"]
```

### rate

Specifies whether to calculate the rate of the value. For this formatOption to work, **rate** in [options](options.md) must be set to true.

The rate is calculated in this way (time is in seconds):
```
rate = (value - oldValue)/(time - oldTime)
```

This option is not available when calling [$.rpijs.parseNumber](rpijsparsenumber.md).

### valueType

Specifies how the number should be formatted. Options:
* **"none"** - number is not formatted in any way
* **"number"** - number is rounded to the number of decimals specified in **decimals**
* **"decimal"** - number is shown in decimal (1000 multiplier) byte size values or rates (B, kB, MB)
* **"binary"** - number is shown in binary (1024 multiplier) byte size values or rates (B, KiB, MiB)
* **"time"** - number of seconds is transformed to *"# days # hours # min # s"*

### decimals

Specifies number of decimal places, that the number is rounded to. Not used when **valueType** is **"none"**.
