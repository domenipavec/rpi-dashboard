# Free serial port

By default Raspbian configures the Raspberry Pi serial port to provide boot-up information and allows login via a connected device.

## Disable login via serial port

Edit /etc/inittab:
```
sudo nano /etc/inittab
```

Add a # character at the beginning of line:
```
#T0:23:respawn:/sbin/getty -L ttyAMA0 115200 vt100
```

Exit and save.

## Disable boot messages

Edit /boot/cmdline.txt:
```
sudo nano /boot/cmdline.txt
```

Change the line:
```
dwc_otg.lpm_enable=0 console=ttyAMA0,115200 kgdboc=ttyAMA0,115200 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
```
to:
```
dwc_otg.lpm_enable=0 console=tty1 root=/dev/mmcblk0p2 rootfstype=ext4 elevator=deadline rootwait
```

Exit, save and reboot.
