# Build on Raspbian

1. Install git and rrdtool
```
sudo apt-get install git librrd-dev
```
2. Clone dudac repository
```
git clone https://github.com/monkey/dudac.git
```
3. Set up monkey and duda development environment in ~/.dudac:
```
dudac/dudac -s
```
If you want HTTPS instead of HTTP access, use:
```
dudac/dudac -sS
```
4. Clone duda-raspberry repository
```
git clone https://github.com/matematik7/duda-raspberry.git
```
5. Checkout release version
```
(cd duda-raspberry && git checkout tags/v0.1.1)
```
6. Build duda-raspberry
```
dudac/dudac -w duda-raspberry/src
```
If you want HTTPS instead of HTTP access, use:
```
dudac/dudac -Sw duda-raspberry/src
```
If you want HTTPS instead of HTTP access, use:
```
dudac/dudac -Sw duda-raspberry/src
```
