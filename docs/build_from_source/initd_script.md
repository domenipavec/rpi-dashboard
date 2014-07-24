# Init.d script

To start monkey server automatically at boot, we set up init.d script (does not work on Arch Linux).

Copy monkey.init to /etc/init.d:
```
sudo cp rpi-dashboard/tools/monkey.init /etc/init.d/monkey
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

## Raspbian
Set monkey for start on boot on Raspbian:
```
sudo update-rc.d monkey defaults
```

Start monkey server manually:
```
sudo /etc/init.d/monkey start
```

Stop monkey server manually:
```
sudo /etc/init.d/monkey stop
```

## Pidora
Set monkey to start on boot on Pidora:
```
sudo systemctl enable monkey
```

Start monkey server manually:
```
sudo systemctl start monkey
```

Stop monkey server manually:
```
sudo systemctl stop monkey
```
