# Enable i2c

If you want to use i2c bus, you have to enable it first:

Open /etc/modules on Rasbian:
```
sudo nano /etc/modules
```

or /etc/modules-load.d/raspberrypi.conf on Arch Linux:
```
sudo nano /etc/modules-load.d/raspberrypi.conf
```

and add these two lines to the end
```
i2c-bcm2708
i2c-dev
```

Check if file /etc/modprobe.d/raspi-blacklist.conf exist and open it:
```
sudo nano /etc/modprobe.d/raspi-blacklist.conf
```

Put # in front of i2c line:
```
blacklist spi-bcm2708
#blacklist i2c-bcm2708
```

Reboot your system.
