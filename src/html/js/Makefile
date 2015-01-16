export JAVA_CMD=java

SUBDIRS=jquery-rpijs angular-dashboard

FILES=jquery-ui.min.js jquery.ui.touch-punch.min.js jquery-rpijs.min.js \
	ng-google-chart.min.js angular-dashboard.min.js

.PHONY=subdirs

all: subdirs javascript.js

javascript.js: $(FILES)
	cat $(FILES) > javascript.js
	
%.min.js: %.js
	yui-compressor --type js --charset utf-8 $< -o $@
	echo "" >> $@
	
clean:
	for dir in $(SUBDIRS); do \
		cd $$dir; $(MAKE) $(MAKEFLAGS) clean; cd ..; \
	done

subdirs:
	for dir in $(SUBDIRS); do \
		cd $$dir; $(MAKE) $(MAKEFLAGS); cd ..; \
	done
