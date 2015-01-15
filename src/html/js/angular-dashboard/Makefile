JAVA_CMD=java

FILES=dashboard.js history.js login.js navigation.js main.js cpu.js \
	memory.js network.js storage.js gpio.js i2c.js spi.js serial.js \
	shift.js

all: angular-dashboard.min.js

clean:
	rm *.min.js

angular-dashboard.min.js: $(FILES:.js=.min.js)
	cat $(FILES:.js=.min.js) > $@
	cp $@ ../$@

%.min.js: %.js
	yui-compressor --type js --charset utf-8 $< -o $@
	echo "" >> $@
	
