# Init.d script

To start monkey server automatically at boot, we set up init.d script (does not work on Arch Linux).

Copy monkey.init to /etc/init.d:
```
sudo cp duda-raspberry/tools/monkey.init /etc/init.d/monkey
```

Modify /etc/init.d/monkey:
```
sudo nano /etc/init.d/monkey
```
Change the line:
```
DUDACPATH=/home/pi/.dudac
```
To the correct file path to your ~/.dudac folder.

Set monkey for start on boot on Raspbian:
```
sudo update-rc.d monkey defaults
```

Set monkey to start on boot on Pidora:
```
sudo systemctl enable monkey
```
