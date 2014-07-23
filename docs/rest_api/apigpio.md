# /api/gpio

GPIO module provides read and write access to gpio of your Raspberry Pi. As well as pwm and tone generator (square wave) configurations.

[WiringPi](http://wiringpi.com/) library is used for accessing GPIO. WiringPi pin numbers are used, check page [Pins](http://wiringpi.com/pins/).

## Read access

Shows info about configuration and values for each pin. For read access use GET methods as for any other module.

Example:
```
{
	"0":	{
		"mode":	"undefined",
		"pull":	"off",
		"value":	0,
		"frequency":	1000,
		"range":	100
	},
	"1":	{
		"mode":	"undefined",
		"pull":	"off",
		"value":	0,
		"frequency":	1000,
		"range":	100
	},
	...
}
```

### gpio/%d/mode

Specifies pin function:
* "undefined" - the pin was not initialized through REST api
* "input" - pin is input
* "output" - pin is output
* "pwm" - pin is PWM output, on pin 1 this is hardware PWM, otherwise software PWM
* "tone" - software square wave generation up to 5kHz

### gpio/%d/pull

Defines pull resistor in "input" mode:
* "off" - no pull resistor (default)
* "down" - pull-down resistor
* "up" - pull-up resistor

### gpio/%d/value

Value can be from 0 to *range*.

Meaning depends on the mode:
* "undefined", "input" - read pin value using *digitalRead*
* "output" - zero is low, any other value is high
* "pwm" - PWM duty cycle
* "tone" - zero disables tone, any other value is enabled

### gpio/%d/frequency

Frequency in Hz for tone generation (max 5kHz) and for hardware PWM on pin 1. Default is 1kHz.

### gpio/%d/range

Sets range of values for PWM. Default is 100.

## Write access

REST api is the same as for read access, but for writing you have to use POST method. Put JSON with values in POST body.

### Write single value

POST to **gpio/%d/mode**
```
"input"
```

### Write more values for one pin with one request

POST to **gpio/%d**
```
{
    "mode": "input",
    "pull": "up"
}
```

### Write more pins at the same time

POST to **gpio**
```
{
    "0": {
        "mode": "input"
    },
    "5": {
        "mode": "output",
        "value": 1
    }
}
```
