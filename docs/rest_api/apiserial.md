# /api/serial

Serial module provides read and write access to serial port.

You may have to [free your serial port](../system_configuration/free_serial_port.md).

### /serial/baud

Use GET to read and POST to set serial port baud rate. Baud rate is in JSON number format.

Only standard baud rates seem to work: 50, 110, 300, 600, 1200, 2400, 4800, 9600, 19 200, 38 400, 57 600, 115 200.

### /serial/port

Use GET to read any data currently in serial port buffer, and POST to write a string to serial port.
Data is in JSON string format.
