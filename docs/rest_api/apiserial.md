# /api/serial

Serial module provides read and write access to serial port.

You may have to [free your serial port](../system_configuration/free_serial_port.md).

### /serial/baud

Use GET to read and POST to set serial port baud rate. Baud rate is in JSON number format.

Only standard baud rates seem to work: 50, 110, 300, 600, 1200, 2400, 4800, 9600, 19 200, 38 400, 57 600, 115 200.

### /serial/port

Use GET to read any data currently in serial port buffer, and POST to write a string to serial port.
Data is in JSON string format.

## WebSocket

For accessing Serial WebSocket interface connect to **serial/ws** with [$.rpijs.websocket()](../rpijs_jquery_plugin/rpijswebsocket.md).

The server broadcasts serial input when message is received. The message is in JSON string format.

You cannot send data to serial port with WebSocket connection, use POST to **/serial/port**.

Note: You cannot use GET on **/serial/port** at the same time as WebSocket. GET request will disable interrupts on serial port and WebSocket connection will stop working.

Example message:
```
"test"
```
