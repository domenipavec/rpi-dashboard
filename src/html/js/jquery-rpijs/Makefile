JAVA_CMD=java

all: jquery-rpijs.min.js

%.min.js: %.js
	yui-compressor --type js --charset utf-8 $< -o $@
	cp $@ ../$@
	
