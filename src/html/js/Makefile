SUBDIRS=jquery-rpijs angular-dashboard

FILES=jquery-rpijs.min.js jquery.ui.touch-punch.min.js \
	ng-google-chart.js angular-dashboard.min.js

.PHONY=subdirs

all: subdirs javascript.js

javascript.js: $(FILES)
	cat $(FILES) > javascript.js

clean:
	for dir in $(SUBDIRS); do \
	  cd $$dir; $(MAKE) $(MAKEFLAGS) clean; cd ..; \
	done

subdirs:
	for dir in $(SUBDIRS); do \
	  cd $$dir; $(MAKE) $(MAKEFLAGS); cd ..; \
	done
