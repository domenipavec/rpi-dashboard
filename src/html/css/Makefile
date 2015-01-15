FILES=login.css general.css

all: style.css

style.css: $(FILES:%.css=parts/%.min.css)
	cat $(FILES:%.css=parts/%.min.css) > style.css

%.min.css: %.css
	cssqc -i $<
	yui-compressor --type css --charset utf-8 $< -o $@
	echo "" >> $@
