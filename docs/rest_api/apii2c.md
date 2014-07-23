# /api/i2c

I2C module allows you to send and receive data over i2c bus.

If you have not enabled i2c yet, better do it now: [Enabling i2c](../system_configuration/enable_i2c.md).

You MUST specify i2c address in the query string like this:
```
?address=0x60
```
Address can be in hex or decimal format.

Use GET method for reading data and POST method for writing data. Put data in POST body in JSON format. Data is in JSON number format.

### i2c/byte?address=

Read or write a byte of data on device.

### i2c/register8/%d?address=

Read or write an 8-bit register on device.

### i2c/register16/%d?address=

Read or write a 16-bit register on device.

### i2c/register8

Write multiple 8-bit registers on device. Write only.

Example:
```
{
    "1": 30,
    "20": 40
}
```

### i2c/register16

Write multiple 16-bit registers on device. Write only.
