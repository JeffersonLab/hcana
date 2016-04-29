.PHONY: doxygen clean

DOXYGEN_OUTPUT_DIRECTORY ?= .
export DOXYGEN_OUTPUT_DIRECTORY
export DOXYGEN_SOURCE_DIRECTORY := ..


all: doxygen

doxygen:
	doxygen

clean:
	rm -r $(DOXYGEN_OUTPUT_DIRECTORY)/html
