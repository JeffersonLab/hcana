.PHONY: doxygen clean

DOXYGEN_OUTPUT_DIRECTORY ?= .
export DOXYGEN_OUTPUT_DIRECTORY
export DOXYGEN_SOURCE_DIRECTORY := ..

PODDDIR = ../podd/docs
# THISDIR is location of this directory relative to the podd docs directory
THISDIR = ../../docs

all: doxygen

doxygen: Doxyfile Doxygen_podd.tag ROOT.tag
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

