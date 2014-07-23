# /api/spi

SPI module allows sending and receiving data on SPI bus.

If you have not enabled SPI yet, better do it now: [Enabling SPI](../system_configuration/enable_spi.md).

Raspberry Pi has 2 SPI channels, use number (0 or 1) in spi/%d to select channel.

### /spi/%d/frequency

Use GET to read and POST to set spi frequency. Frequency is in JSON number format.

### POST to /spi/%d

Perform simultaneous read and write. JSON String format in body will be written to SPI bus, and the string of the same length will be read and returned.

Example:
```
"test"
```
```
""
```

**test** was written to the SPI bus, and all zeros were read, so empty string was returned.
