.PHONY: doxygen clean

DOXYGEN_OUTPUT_DIRECTORY ?= .
export DOXYGEN_OUTPUT_DIRECTORY
export DOXYGEN_SOURCE_DIRECTORY := ..


all: doxygen

doxygen: Doxyfile ROOT.tag
	doxygen

ROOT.tag:
	wget https://root.cern.ch/doc/master/ROOT.tag

clean:
	rm -r $(DOXYGEN_OUTPUT_DIRECTORY)/html
