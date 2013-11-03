#------------------------------------------------------------------------------
# Names of source files and target libraries
# You do want to modify this section

# List all your source files here. They will be put into a shared library
# that can be loaded from a script.
# List only the implementation files (*.cxx). For every implementation file
# there must be a corresponding header file (*.h).

SRC  =  src/THcInterface.cxx src/THcParmList.cxx src/THcAnalyzer.cxx \
	src/THcHallCSpectrometer.cxx \
	src/THcDetectorMap.cxx \
	src/THcRawHit.cxx src/THcHitList.cxx \
	src/THcSignalHit.cxx \
	src/THcHodoscope.cxx src/THcScintillatorPlane.cxx \
	src/THcHodoscopeHit.cxx \
	src/THcDC.cxx src/THcDriftChamberPlane.cxx \
	src/THcDriftChamber.cxx \
	src/THcRawDCHit.cxx src/THcDCHit.cxx \
	src/THcDCWire.cxx \
	src/THcDCLookupTTDConv.cxx src/THcDCTimeToDistConv.cxx \
	src/THcSpacePoint.cxx src/THcDCTrack.cxx \
	src/THcShower.cxx src/THcShowerPlane.cxx \
	src/THcRawShowerHit.cxx \
	src/THcAerogel.cxx src/THcAerogelHit.cxx \
	src/THcFormula.cxx

# Name of your package. 
# The shared library that will be built will get the name lib$(PACKAGE).so
PACKAGE = HallC

# Name of the LinkDef file
LINKDEF = src/$(PACKAGE)_LinkDef.h

#------------------------------------------------------------------------------
# This part defines overall options and directory locations.
# Change as necessary,

# Compile debug version
export DEBUG = 1

# Architecture to compile for
ARCH          = linux
#ARCH          = solarisCC5

#------------------------------------------------------------------------------
# Directory locations. All we need to know is INCDIRS.
# INCDIRS lists the location(s) of the C++ Analyzer header (.h) files

# The following should work with both local installations and the
# Hall A counting house installation. For local installations, verify
# the setting of ANALYZER, or specify INCDIRS explicitly.
# To use this makefile without modif, you need to have the ANALYZER environment
# variable defined, pointing to the directory where the Hall A analyzer (Podd)
# that you use is located

ifndef ANALYZER
  $(error $$ANALYZER environment variable not defined)
endif

INCDIRS  = $(wildcard $(addprefix $(ANALYZER)/, include src hana_decode hana_scaler)), $(shell pwd)/src

#------------------------------------------------------------------------------
# Check that root version is new enough (>= 5.32) by requiring
# root-config --svn-revision to be >= 43166

GOODROOTVERSION := $(shell expr `root-config --svn-revision` \>= 43166)

ifneq ($(GOODROOTVERSION),1)
  $(error ROOT version 5.32 or later required)
endif

#------------------------------------------------------------------------------
# Do not change anything  below here unless you know what you are doing

ifeq ($(strip $(INCDIRS)),)
  $(error No Analyzer header files found. Check $$ANALYZER)
endif

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
ROOTBIN      := $(shell root-config --bindir)

INCLUDES      = $(ROOTCFLAGS) $(addprefix -I, $(INCDIRS) )
#INCLUDES      = $(ROOTCFLAGS) $(addprefix -I, $(INCDIRS) ) -I$(shell pwd)

USERLIB       = lib$(PACKAGE).so
USERDICT      = $(PACKAGE)Dict

LIBS          = 
GLIBS         = 

ifeq ($(ARCH),solarisCC5)
# Solaris CC 5.0
CXX           = CC
ifdef DEBUG
  CXXFLAGS    = -g
  LDFLAGS     = -g
else
  CXXFLAGS    = -O
  LDFLAGS     = -O
endif
CXXFLAGS     += -KPIC
LD            = CC
SOFLAGS       = -G
endif

ifeq ($(ARCH),linux)
# Linux with egcs (>= RedHat 5.2)
CXX           = g++
ifdef DEBUG
  CXXFLAGS    = -g -O0
  LDFLAGS     = -g -O0
else
  CXXFLAGS    = -O
  LDFLAGS     = -O
endif
CXXFLAGS     += -Wall -Woverloaded-virtual -fPIC
LD            = g++
SOFLAGS       = -shared
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
endif

CXXFLAGS     += $(INCLUDES) -DHALLC_MODS
LIBS         += $(ROOTLIBS) $(SYSLIBS)
GLIBS        += $(ROOTGLIBS) $(SYSLIBS)

MAKEDEPEND    = gcc

ifdef WITH_DEBUG
CXXFLAGS     += -DWITH_DEBUG
endif

ifdef PROFILE
CXXFLAGS     += -pg
LDFLAGS      += -pg
endif

ifndef PKG
PKG           = lib$(PACKAGE)
LOGMSG        = "$(PKG) source files"
else
LOGMSG        = "$(PKG) Software Development Kit"
endif
DISTFILE      = $(PKG).tar.gz

#------------------------------------------------------------------------------
OBJ           = $(SRC:.cxx=.o)
RCHDR	      = $(SRC:.cxx=.h) src/THcGlobals.h
HDR           = $(SRC:.cxx=.h) 
DEP           = $(SRC:.cxx=.d) src/main.d
OBJS          = $(OBJ) $(USERDICT).o
HDR_COMPILEDATA = $(ANALYZER)/src/ha_compiledata.h

all:		$(USERLIB) hcana

LIBDIR:=$(ANALYZER)
LIBHALLA     := $(LIBDIR)/libHallA.so
LIBDC        := $(LIBDIR)/libdc.so
LIBSCALER    := $(LIBDIR)/libscaler.so
HALLALIBS    := -L$(LIBDIR) -lHallA -ldc -lscaler

src/THcInterface.d:  $(HDR_COMPILEDATA)

hcana:		src/main.o $(LIBDC) $(LIBSCALER) $(LIBHALLA) $(USERLIB)
		$(LD) $(LDFLAGS) $< $(HALLALIBS) -L. -lHallC $(GLIBS) -o $@

$(USERLIB):	$(HDR) $(OBJS)
		$(LD) $(LDFLAGS) $(SOFLAGS) -o $@ $(OBJS)
		@echo "$@ done"

$(HDR_COMPILEDATA) $(LIBHALLA) $(LIBDC) $(LIBSCALER): $(ANALYZER)/Makefile
		@echo "Building Podd"		
		@cd $(ANALYZER) ; export PODD_EXTRA_DEFINES=-DHALLC_MODS ; make

$(USERDICT).cxx: $(RCHDR) $(HDR) $(LINKDEF)
	@echo "Generating dictionary $(USERDICT)..."
	$(ROOTBIN)/rootcint -f $@ -c $(INCLUDES) $^

install:	all
	cp -p $(USERLIB) $(HOME)/cue/SRC/ana

clean:
		rm -f src/*.o *~ $(USERLIB) $(USERDICT).*

realclean:	clean
		rm -f *.d NormAnaDict.* THaDecDict.* THaScallDict.* bin/hcana
		rm -f src/*.os
		rm -f bin

srcdist:
		rm -f $(DISTFILE)
		rm -rf $(PKG)
		mkdir $(PKG)
		cp -p $(SRC) $(HDR) $(LINKDEF) db*.dat README Makefile $(PKG)
		gtar czvf $(DISTFILE) --ignore-failed-read \
		 -V $(LOGMSG)" `date -I`" $(PKG)
		rm -rf $(PKG)

.PHONY: all clean realclean srcdist

.SUFFIXES:
.SUFFIXES: .c .cc .cpp .cxx .C .o .d

%.o:	%.cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<

# FIXME: this only works with gcc
%.d:	%.cxx
	@echo Creating dependencies for $<
	@$(SHELL) -ec '$(MAKEDEPEND) -MM $(INCLUDES) -c $< \
		| sed '\''s%^.*\.o%$*\.o%g'\'' \
		| sed '\''s%\($*\)\.o[ :]*%\1.o $@ : %g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

###

-include $(DEP)
