export JAVA_CMD=java

FILES=login.css general.css

all: style.css

clean:
	rm parts/*.min.css

style.css: $(FILES:%.css=parts/%.min.css)
	cat $(FILES:%.css=parts/%.min.css) > style.css

%.min.css: %.css
	cssqc -i $<
	yui-compressor --type css --charset utf-8 $< -o $@
	echo "" >> $@
