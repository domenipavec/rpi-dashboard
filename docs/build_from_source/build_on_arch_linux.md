# Build on Arch Linux

1. Install git and rrdtool
```
sudo pacman -S git rrdtool python2 base-devel
```
2. Link python to python2
```
sudo ln /usr/bin/python2 /usr/bin/python
```
3. Clone dudac repository
```
git clone https://github.com/monkey/dudac.git
```
4. Set up monkey and duda development environment in ~/.dudac:
```
dudac/dudac -s
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
