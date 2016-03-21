rpi-dashboard
=============

A dashboard for Raspberry Pi using Duda I/O.

Raspberry Pi Dashboard:
- is a web based dashboard for monitoring and controlling your Raspberry Pi
- shows information about CPU, memory, network and storage
- is open source, licensed under [Apache License v2.0](http://www.apache.org/licenses/LICENSE-2.0.html). All code is available in [github repository](https://github.com/matematik7/rpi-dashboard)
- uses a lightweight [Monkey](http://monkey-project.com/) web server and [Duda I/O](http://duda.io/) framework for backend and AngularJS, jQuery, Bootstrap and Google Charts for frontend
- was developed as part of [Google Summer of Code](https://www.google-melange.com/gsoc/homepage/google/gsoc2014)

Documentation with installation instructions: [gitbooks](https://matematik7.gitbooks.io/rpi-dashboard-docs/content/)

## Features:

- Simple user login configuration
- CPU, memory, network and storage metrics
- Logging of metrics for hour, day, week, month and year intervals
- REST api
- jQuery plugin library
- AngularJS one page Dashboard
- GPIO view and control (input, output, PWM, tone)
- I2C, SPI and serial support
- Shifting 8-bits in/out on two pins
- WebSocket support for GPIO and serial
- Modular main page on dashboard

## Screenshots:

### Login screen
![Login screen](screenshots/login.png)

### Main dashboard page
![Main dashboard page](screenshots/main.png)

### Page with cpu information
![Page with cpu information](screenshots/cpu.png)

### Page with information about ram and swap
![Page with information about ram and swap](screenshots/memory.png)

### Page with network information
![Page with network information](screenshots/network.png)

### Page with information about storage
![Page with information about storage](screenshots/storage.png)

### Page with GPIO control
![Page with GPIO control](screenshots/gpio.png)

### GPIO page with pinout shown
![GPIO page with pinout shown](screenshots/gpio-pinout.png)

### Page with i2c control
![Page with i2c control](screenshots/i2c.png)

### Page for SPI control
![Page for SPI control](screenshots/spi.png)

### Page for serial communication
![Page for serial communication](screenshots/serial.png)

### Page for shift control
![Page for shift control](screenshots/shift.png)


