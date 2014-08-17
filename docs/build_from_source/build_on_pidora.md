# Build on Pidora

1. Install git and rrdtool
```
sudo yum install git rrdtool-devel
```
2. Install wiringPi
```
git clone git://git.drogon.net/wiringPi
(cd wiringPi && ./build)
```
3. Clone dudac repository
```
git clone https://github.com/monkey/dudac.git
```
4. Set up monkey and duda development environment in ~/.dudac:
```
dudac/dudac -s
```
If you want HTTPS instead of HTTP access, use:
```
dudac/dudac -sS
```
5. Clone duda-raspberry repository
```
git clone https://github.com/matematik7/rpi-dashboard.git
```
6. Checkout release version
```
(cd rpi-dashboard && git checkout tags/v0.2.0)
```
7. Build rpi-dashboard
```
dudac/dudac -w rpi-dashboard/src
```
If you want HTTPS instead of HTTP access, use:
```
dudac/dudac -Sw rpi-dashboard/src
```
