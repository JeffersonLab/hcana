.PHONY: doxygen clean

DOXYGEN_OUTPUT_DIRECTORY ?= .
export DOXYGEN_OUTPUT_DIRECTORY
export DOXYGEN_SOURCE_DIRECTORY := ..

PODDDIR = ../podd/docs
# THISDIR is absolute location of this directory
THISDIR = $(PWD)
export THISDIR
INSTALLPATH = /u/group/hallc/www/hallcweb/html/hcana
INSTALLPATHHCANA = $(INSTALLPATH)/docs/
INSTALLPATHPODD = $(INSTALLPATH)/podddocs/

all: doxygen $(DOXYGEN_OUTPUT_DIRECTORY)/html/Doxygen_hcana.tag \
	$(DOXYGEN_OUTPUT_DIRECTORY)/html/Doxygen_podd.tag

doxygen html/index.html Doxygen.tag: Doxyfile Doxygen_podd.tag ROOT.tag DoxygenLayout.xml
	doxygen $(THISDIR)/Doxyfile

#Doxyfile_podd: $(PODDDIR)/Doxyfile
#	sed -e 's/^TAGFILES.*/TAGFILES = /;s|Doxygen.tag|$(THISDIR)/Doxygen_podd.tag|' $< > $@
Doxyfile_podd: Doxyfile_podd_tmp
	cp Doxyfile_podd_tmp Doxyfile_podd

Doxygen_podd.tag: Doxyfile_podd
	cp ROOT.tag $(PODDDIR)
	cd $(PODDDIR);doxygen $(THISDIR)/Doxyfile_podd

ROOT.tag:
	wget https://root.cern.ch/doc/master/ROOT.tag.gz
	gunzip ROOT.tag.gz

$(DOXYGEN_OUTPUT_DIRECTORY)/html/Doxygen_hcana.tag: Doxygen.tag
	cp -p Doxygen.tag $(DOXYGEN_OUTPUT_DIRECTORY)/html/Doxygen_hcana.tag

$(DOXYGEN_OUTPUT_DIRECTORY)/html/Doxygen_podd.tag: Doxygen_podd.tag
	cp -p Doxygen_podd.tag $(DOXYGEN_OUTPUT_DIRECTORY)/html/

clean:
	rm -r $(DOXYGEN_OUTPUT_DIRECTORY)/html
	rm -r $(PODDDIR)/html
	rm Doxyfile_podd
	rm Doxygen_podd.tag

# Must have permission to write to $(INSTALLPATH)
install: doxygen
	rsync -av --delete html/ $(INSTALLPATHHCANA)
	rsync -av --delete $(PODDDIR)/html/ $(INSTALLPATHPODD)
