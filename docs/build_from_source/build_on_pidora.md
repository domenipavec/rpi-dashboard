# Build on Pidora

1. Install git and rrdtool
```
sudo yum install git rrdtool-devel
```
2. Clone dudac repository
```
git clone https://github.com/monkey/dudac.git
```
3. Set up monkey and duda development environment in ~/.dudac:
```
dudac/dudac -s
```
4. Clone duda-raspberry repository
```
git clone https://github.com/matematik7/duda-raspberry.git
```
5. Build duda-raspberry
```
dudac/dudac -w duda-raspberry/src
```
