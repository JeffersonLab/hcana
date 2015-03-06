hcana with externally built evio libraries
============================================

As of PODD version 1.6, EVIO is built as an external library from the official JLAB
DAQ group source code.  As of this writing, the official release is version 4.4.5.
In order to compile hcana with PODD version 1.6, one must download, install, and define
appropriate environment variables for the evio libraries.

Downloading
-----------

On your local system, retrieve and unpack the EVIO source distribution:

	$ wget --no-check-certificate https://coda.jlab.org/drupal/system/files/coda/evio/evio-4.4/evio-4.4.5.tgz
	$ tar -xvzf evio-4.4.5.tgz
	$ cd evio-4.4.5

This will unpack the EVIO source code in a directory called evio-4.4.5, and take you
into that new directory.

Compiling
---------

You can now compile the EVIO libraries with:

	$ scons
	$ scons install --prefix=.

This will install the libaries in a subdirectory of the current source directory according to the operating system that you are using.

Environment
-----------

The final step is to define the environment variables necessary to tell hcana where your EVIO libraries are installed.  I include here excerpts from a my .cshrc file on the JLab systems (where the SHELL is tcsh), and also from my .bash_profile file on a Mac OS X system (where the SHELL is bash).  This should give you enough of a guide to be able to modify your environment variable definition appropriately.

.cshrc

	setenv HCANALYZER /home/brash/analysis/hcana
	setenv ANALYZER /home/brash/analysis/hcana/podd
	setenv INSTALL_DIR /home/brash/evio-4.4.5
	setenv KERNEL_NAME `uname -s`
	setenv MACHINE_NAME `uname -m`
	setenv EVIO_SUBDIR $KERNEL_NAME-$MACHINE_NAME
	setenv EVIO_BINDIR $INSTALL_DIR/$EVIO_SUBDIR/bin
	setenv EVIO_LIBDIR $INSTALL_DIR/$EVIO_SUBDIR/lib
	setenv EVIO_INCDIR $INSTALL_DIR/$EVIO_SUBDIR/include
	setenv LD_LIBRARY_PATH "${HCANALYZER}:${ANALYZER}/src:${ANALYZER}/hana_decode:${ANALYZER}/hana_scaler:${EVIO_LIBDIR}:${ANALYZER}"
	setenv DB_DIR $ANALYZER/DB
	set path = ( $ANALYZER/bin $HCANALYZER/bin $EVIO_BINDIR $path )

.bash_profile

	export HCANALYZER=/Users/brash/Dropbox/Research/analysis/hcana
	export ANALYZER=/Users/brash/Dropbox/Research/analysis/hcana/podd
	export INSTALL_DIR=/Users/brash/Dropbox/Research/analyzer-evio/evio-4.4.5
	export KERNEL_NAME=`uname -s`
	export MACHINE_NAME=`uname -m`
	export EVIO_SUBDIR=$KERNEL_NAME-$MACHINE_NAME
	export EVIO_BINDIR=$INSTALL_DIR/$EVIO_SUBDIR/bin
	export EVIO_LIBDIR=$INSTALL_DIR/$EVIO_SUBDIR/lib
	export EVIO_INCDIR=$INSTALL_DIR/$EVIO_SUBDIR/include
	export PATH=$HCANALYZER/bin:$ANALYZER/bin:$EVIO_BINDIR:$PATH
	export DYLD_LIBRARY_PATH=$HCANALYZER:$ANALYZER:$EVIO_LIBDIR:$DYLD_LIBRARY_PATH
	export LD_LIBRARY_PATH=$HCANALYZER:$ANALYZER:$EVIO_LIBDIR:$LD_LIBRARY_PATH



