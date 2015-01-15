SUBDIRS=jquery-rpijs angular-dashboard

.PHONY=subdirs

all: subdirs

subdirs:
	for dir in $(SUBDIRS); do \
	  cd $$dir; $(MAKE) $(MAKEFLAGS); cd ..; \
	done
