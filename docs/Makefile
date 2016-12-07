.PHONY: doxygen clean

DOXYGEN_OUTPUT_DIRECTORY ?= .
export DOXYGEN_OUTPUT_DIRECTORY
export DOXYGEN_SOURCE_DIRECTORY := ..

PODDDIR = ../podd/docs
# THISDIR is absolute location of this directory
THISDIR = $(PWD)
INSTALLPATH = jlabl1:/group/hallc/www/hallcweb/html/hcana
INSTALLPATHHCANA = $(INSTALLPATH)/docs/
INSTALLPATHPODD = $(INSTALLPATH)/podddocs/

all: doxygen

doxygen html/index.html: Doxyfile Doxygen_podd.tag ROOT.tag
	doxygen

Doxyfile_podd: $(PODDDIR)/Doxyfile
	sed -e 's/^TAGFILES.*/TAGFILES = /;s|Doxygen.tag|$(THISDIR)/Doxygen_podd.tag|' $< > $@

Doxygen_podd.tag: Doxyfile_podd
	cd $(PODDDIR);doxygen $(THISDIR)/Doxyfile_podd

ROOT.tag:
	wget https://root.cern.ch/doc/master/ROOT.tag

clean:
	rm -r $(DOXYGEN_OUTPUT_DIRECTORY)/html
	rm -r $(PODDDIR)/html
	rm Doxyfile_podd
	rm Doxygen_podd.tag

# Must have permission to write to $(INSTALLPATH)
install: doxygen
	rsync -av --delete html/ $(INSTALLPATHHCANA)
	rsync -av --delete $(PODDDIR)/html/ $(INSTALLPATHPODD)
