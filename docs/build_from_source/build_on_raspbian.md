# Build on Raspbian

1. Install git and rrdtool
```
sudo apt-get install git librrd-dev
```
2. Install wiringPi
```
git clone git://git.drogon.net/wiringPi
wiringPi/build
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
git clone https://github.com/matematik7/duda-raspberry.git
```
6. Checkout release version
```
(cd duda-raspberry && git checkout tags/v0.1.1)
```
7. Build duda-raspberry
```
dudac/dudac -w duda-raspberry/src
```
If you want HTTPS instead of HTTP access, use:
```
dudac/dudac -Sw duda-raspberry/src
```
