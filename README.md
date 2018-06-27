hcana - Hall C ROOT/C++ analyzer
============================================

hcana is an under-development tool to analyze data from the HMS, SHMS and
SOS spectrometers in
[Hall C](http://www.jlab.org/Hall-C/) at [JLab](http://www.jlab.org/).
It is being developed to replace
the historical Hall C analyzer, ENGINE, which was written in Fortran.
hcana will be the package used to analyze Hall C date in the 12 GeV era.
hcana is being written in C++, using the CERN ROOT framework.  hcana is
an extension to the Hall A analyzer, PODD.

NOTE: In the process of retrieving the hcana source code, a copy of
the Hall A PODD package will be downloaded.  The version of PODD included
has been slightly modified for use with hcana.  For an official version
of PODD, see the [ROOT/C++ Analyzer for Hall A](http://hallaweb.jlab.org/podd/) page.

Downloading
-----------

Instructions for downloading hcana can be found in the
[Hall C Wiki](https://hallcweb.jlab.org/wiki/index.php/ROOT_Analyzer/Git).

Compiling
---------

CMake is the preferred build tool. See below for builds with scons/make which 
are slightly different.

### Compiling with CMAKE

CMake build will do a **proper** build and install.
Here we are using the install prefix `$HOME/my_exp_soft` which is like the 
standard `/usr/local`. To use it make sure you your environment is setup (e.g., 
towards the end of your `.bashrc`):
```
export PATH=$HOME/my_exp_soft/bin:$PATH
export LD_LIBRARY_PATH=$HOME/my_exp_soft/lib:$HOME/my_exp_soft/lib64:$LD_LIBRARY_PATH
```

**Do not install into the source directories**. (ノಠ益ಠ)ノ彡┻━┻

First you should build evio followed by podd. Here are all the steps:

#### Build EVIO with cmake

```
git clone https://github.com/whit2333/hallac_evio.git
cd hallac_evio
mkdir build && cd build
cmake ../. -DCMAKE_INSTALL_PREFIX=$HOME/my_exp_soft
make -j4 install
```

#### Build analyzer (PODD)

```
git clone https://github.com/whit2333/analyzer.git
cd analyzer
mkdir build && cd build
cmake ../. -DCMAKE_INSTALL_PREFIX=$HOME/my_exp_soft
make -j4 install
```

#### Build hcana

```
git clone https://github.com/whit2333/hcana.git
cd hcana
mkdir build && cd build
cmake ../. -DCMAKE_INSTALL_PREFIX=$HOME/my_exp_soft
make -j4 install
```

All done.  Now you can run `hc_analyzer` and you're off to analyze.

### Other builds

hcana may be compiled with either make or scons.  Switching between these
two build systems make require some cleanup of dependency files, binary files
and other autogenerated files.

Before compiling, type
`source setup.sh` or `source setup.csh`
depending on whether your shell is bash or csh.

### Compiling with make
    make

### Compiling with scons
    scons

### Additional SCons features 
To do the equivalent of "make clean", do
`scons -c`
To compile with debug capabilities, do
`scons debug=1`
To compile the standalone codes the are part of podd, do
`scons standalone=1`
To run cppcheck (if installed) on the Hall C src diretory, do
`scons cppcheck=1`

Running
-------
Basic instructions on how to run hcana are in the
[Hall C Wiki](https://hallcweb.jlab.org/wiki/index.php/ROOT_Analyzer/Running).

Contributing
------------
To participate in hcana code development, contact Mark Jones or Stephen Wood.


