# /api/shift

Shift module enables you to shift 8-bit data in or out of GPIO. All operations require POST method. Shifting is done at approximately 2-3Mhz.

### shift/in

Shift 8-bit value in, using data pin as input and clock pin as output. Shifting is done as "lsbfirst" or "msbfirst".

Note: You have to set clock pin as output manually using [/api/gpio](apigpio.md).

**Example:**

Shift in data from pin 1, with clock output on pin 0.

POST JSON input:
```
{
    "order": "msbfirst",
    "data": 1,
    "clock": 0
}
```

Return:
```
170
```

### shift/out

Shift 8-bit value out on data pin and using clock pin. Shifting is done as "lsbfirst" or "msbfirst".

Note: You have to set data and clock pins as output manually using [/api/gpio](apigpio.md).

**Example:**

Shift out data on pin 1, with clock output on pin 0.

POST JSON input:
```
{
    "order": "msbfirst",
    "data": 1,
    "clock": 0,
    "value": 170
}
```

Return:
```
"Successful!"
```
