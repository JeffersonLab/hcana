#!/usr/bin/env python
###### Hall C Software Main SConstruct Build File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import sys
import platform
import commands
import SCons
import subprocess

def rootcint(target,source,env):
	"""Executes the ROOT dictionary generator over a list of headers."""
	dictname = target[0]
	headers = ""
	cpppath = env.subst('$_CCCOMCOM')
	ccflags = env.subst('$CCFLAGS')
	rootcint = env.subst('$ROOTCINT')
	print "Doing rootcint call now ..."
	for f in source:
		headers += str(f) + " "
	command = rootcint + " -f %s -c -pthread -fPIC %s %s" % (dictname,cpppath,headers)
	print ('RootCint Command = %s\n' % command)
	ok = os.system(command)
	return ok

baseenv = Environment(ENV = os.environ)
#dict = baseenv.Dictionary()
#keys = dict.keys()
#keys.sort()
#for key in keys:
#	print "Construction variable = '%s', value = '%s'" % (key, dict[key])

####### Check SCons version ##################
print('!!! Building the Hall C analyzer and libraries with SCons requires')
print('!!! SCons version 2.1.0 or newer.')
EnsureSConsVersion(2,1,0)

####### Hall A Build Environment #############
#
baseenv.Append(HEAD_DIR= Dir('.').abspath)
baseenv.Append(HC_DIR= baseenv.subst('$HEAD_DIR'))
baseenv.Append(HC_SRC= baseenv.subst('$HC_DIR')+'/src ')
baseenv.Append(HA_DIR= baseenv.subst('$HC_DIR')+'/podd ')
baseenv.Append(MAIN_DIR= baseenv.subst('$HEAD_DIR'))
baseenv.Append(HA_SRC= baseenv.subst('$HA_DIR')+'/src ')
baseenv.Append(HA_DC= baseenv.subst('$HA_DIR')+'/hana_decode ')
baseenv.Append(MAJORVERSION = '1')
baseenv.Append(MINORVERSION = '6')
baseenv.Append(PATCH = '0')
baseenv.Append(SOVERSION = baseenv.subst('$MAJORVERSION')+'.'+baseenv.subst('$MINORVERSION'))
baseenv.Append(VERSION = baseenv.subst('$SOVERSION')+'.'+baseenv.subst('$PATCH'))
baseenv.Append(EXTVERS = '')
baseenv.Append(HA_VERSION = baseenv.subst('$VERSION')+baseenv.subst('$EXTVERS'))
print "Hall C Main Directory = %s" % baseenv.subst('$HC_DIR')
print "Hall C Source Directory = %s" % baseenv.subst('$HC_SRC')
print "Hall A Main Directory = %s" % baseenv.subst('$HA_DIR')
print "Software Version = %s" % baseenv.subst('$VERSION')
ivercode = 65536*int(float(baseenv.subst('$SOVERSION')))+ 256*int(10*(float(baseenv.subst('$SOVERSION'))-int(float(baseenv.subst('$SOVERSION')))))+ int(float(baseenv.subst('$PATCH')))
baseenv.Append(VERCODE = ivercode)
#
# evio environment
#
evio_libdir = os.getenv('EVIO_LIBDIR')
evio_incdir = os.getenv('EVIO_INCDIR')
if evio_libdir is None or evio_incdir is None:
	print "No external EVIO environment configured !!!"
	print "EVIO_LIBDIR = %s" % evio_libdir
	print "EVIO_INCDIR = %s" % evio_incdir
	print "Using local installation ... "
	evio_local = baseenv.subst('$HA_DIR')+'/evio'
	evio_version = '4.4.6'
	uname = os.uname();
	platform = uname[0];
	machine = uname[4];
	evio_name = platform + '-' + machine
	print "evio_name = %s" % evio_name
	evio_local_lib = "%s/evio-%s/%s/lib" % (evio_local,evio_version,evio_name)
	evio_local_inc = "%s/evio-%s/%s/include" % (evio_local,evio_version,evio_name)
	evio_tarfile = "%s/evio-%s.tgz" % (evio_local,evio_version)

	####### Check to see if scons -c has been called #########

	if baseenv.GetOption('clean'):
    		subprocess.call(['echo', '!!!!!!!!!!!!!! EVIO Cleaning Process !!!!!!!!!!!! '])
		if not os.path.isdir(evio_local_lib):
			if not os.path.exists(evio_tarfile):
				evio_command_scons = "rm libevio*.*; cd %s; wget --no-check-certificate https://coda.jlab.org/drupal/system/files/evio-%s.tgz; tar xvfz evio-%s.tgz; cd evio-%s/ ; scons install -c --prefix=." % (evio_local,evio_version,evio_version,evio_version)
			else:
				evio_command_scons = "rm libevio*.*; cd %s; tar xvfz evio-%s.tgz; cd evio-%s/ ; scons install -c --prefix=." % (evio_local,evio_version,evio_version)
		else:
			evio_command_scons = "rm libevio*.*; cd %s; cd evio-%s/ ; scons install -c --prefix=." % (evio_local,evio_version)
		print "evio_command_scons = %s" % evio_command_scons
		os.system(evio_command_scons)
	else:
		if not os.path.isdir(evio_local_lib):
			if not os.path.exists(evio_tarfile):
				evio_command_scons = "cd %s; wget --no-check-certificate https://coda.jlab.org/drupal/system/files/evio-%s.tgz; tar xvfz evio-%s.tgz; cd evio-%s/ ; scons install --prefix=." % (evio_local,evio_version,evio_version,evio_version)
			else:
				evio_command_scons = "cd %s; tar xvfz evio-%s.tgz; cd evio-%s/ ; scons install --prefix=." % (evio_local,evio_version,evio_version)
		else:
			evio_command_scons = "cd %s; cd evio-%s/ ; scons install --prefix=." % (evio_local,evio_version)
		print "evio_command_scons = %s" % evio_command_scons
		os.system(evio_command_scons)
		evio_local_lib_files = "%s/*.*" % (evio_local_lib)
		evio_command_libcopy = "cp %s %s" % (evio_local_lib_files,baseenv.subst('$HA_DIR'))
		print "evio_command_libcopy = %s" % evio_command_libcopy
		os.system(evio_command_libcopy)

	baseenv.Append(EVIO_LIB = evio_local_lib)
	baseenv.Append(EVIO_INC = evio_local_inc)
else:
	# evio_command_scons = "cd %s; scons install --prefix=." % evio_instdir
	# os.system(evio_command_scons)
	baseenv.Append(EVIO_LIB = os.getenv('EVIO_LIBDIR'))
	baseenv.Append(EVIO_INC = os.getenv('EVIO_INCDIR'))
print "EVIO lib Directory = %s" % baseenv.subst('$EVIO_LIB')
print "EVIO include Directory = %s" % baseenv.subst('$EVIO_INC')
baseenv.Append(CPPPATH = ['$EVIO_INC'])
#
# end evio environment
#
baseenv.Append(CPPPATH = ['$HC_SRC','$HA_SRC','$HA_DC'])

proceed = "1" or "y" or "yes" or "Yes" or "Y"
######## Configure Section #######

import configure
configure.config(baseenv,ARGUMENTS)

Export('baseenv')

conf = Configure(baseenv)

if not conf.CheckCXX():
	print('!!! Your compiler and/or environment is not correctly configured.')
	Exit(0)

if not conf.CheckFunc('printf'):
       	print('!! Your compiler and/or environment is not correctly configured.')
       	Exit(0)

if baseenv.subst('$CHECKHEADERS')==proceed:
	system_header_list = ['arpa/inet.h','errno.h','assert.h','netdb.h','netinet/in.h','pthread.h','signal.h','stddef.h','stdio.h','stdlib.h','string.h','strings.h','sys/ioctl.h','sys/socket.h','sys/time.h','sys/types.h','time.h','unistd.h','memory.h','math.h','limits.h']

	for header_file in system_header_list:
		if not conf.CheckHeader(header_file):
			print('!! Header file %s not found.' % header_file)
			Exit(0)

baseenv = conf.Finish()

######## ROOT Dictionaries #########
baseenv.Append(ROOTCONFIG = 'root-config')
baseenv.Append(ROOTCINT = 'rootcint')

try:
        baseenv.ParseConfig('$ROOTCONFIG --cflags')
        baseenv.ParseConfig('$ROOTCONFIG --libs')
        baseenv.MergeFlags('-fPIC')
except OSError:
        try:
		baseenv.Replace(ROOTCONFIG = baseenv['ENV']['ROOTSYS'] + '/bin/root-config')
		baseenv.Replace(ROOTCINT = baseenv['ENV']['ROOTSYS'] + '/bin/rootcint')
		baseenv.ParseConfig('$ROOTCONFIG --cflags')
		baseenv.ParseConfig('$ROOTCONFIG --libs')
		baseenv.MergeFlags('-fPIC')
	except KeyError:
                print('!!! Cannot find ROOT.  Check if root-config is in your PATH.')
                Exit(1)

bld = Builder(action=rootcint)
baseenv.Append(BUILDERS = {'RootCint': bld})

######## cppcheck ###########################

def which(program):
	import os
	def is_exe(fpath):
		return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

	fpath, fname = os.path.split(program)
	if fpath:
		if is_exe(program):
			return program
	else:
		for path in os.environ["PATH"].split(os.pathsep):
			path = path.strip('"')
			exe_file = os.path.join(path, program)
			if is_exe(exe_file):
				return exe_file
	return None

if baseenv.subst('$CPPCHECK')==proceed:
	is_cppcheck = which('cppcheck')
	print "Path to cppcheck is %s\n" % is_cppcheck

	if(is_cppcheck == None):
		print('!!! cppcheck not found on this system.  Check if cppcheck is installed and in your PATH.')
		Exit(1)
	else:
		cppcheck_command = baseenv.Command('cppcheck_report.txt',[],"cppcheck --quiet --enable=all src/ 2> $TARGET")
		baseenv.AlwaysBuild(cppcheck_command)

####### Start of main SConstruct ############

hallclib = 'HallC'
hallalib = 'HallA'
dclib = 'dc'
eviolib = 'evio'

baseenv.Append(LIBPATH=['$HC_DIR','$EVIO_LIB','$HA_DIR','$HC_SRC','$HA_SRC','$HA_DC'])
baseenv.Replace(SHLIBSUFFIX = '.so')
baseenv.Append(CPPDEFINES = '-DHALLC_MODS')

baseenv.Append(SHLIBSUFFIX ='.'+baseenv.subst('$VERSION'))
pbaseenv=baseenv.Clone()
pbaseenv.Prepend(LIBS=[hallclib,hallalib,dclib,eviolib])
baseenv.Prepend(LIBS=[hallalib,dclib,eviolib])
Export('pbaseenv')

if pbaseenv['CXX'] == 'g++':
	gxxVersion = [int(i) for i in pbaseenv['CXXVERSION'].split('.')]
	if (gxxVersion[0] < 4) or (gxxVersion[0] == 4 and gxxVersion[1] < 4):
		print('Error: g++ version too old! Need at least g++ 4.4!')
		Exit(1)
	elif gxxVersion[0] == 4 and 4 <= gxxVersion[1] < 7:
		if '-std=c++0x' not in pbaseenv['CXXFLAGS']:
			pbaseenv.Append(CXXFLAGS='-std=c++0x')
	else:
		if '-std=c++11' not in pbaseenv['CXXFLAGS']:
			pbaseenv.Append(CXXFLAGS='-std=c++11')

##directorylist = ['./','src','podd','podd/src','podd/hana_decode']
##SConscript('podd/SConstruct')

if baseenv.GetOption('clean'):
    subprocess.call(['echo', '!!!!!! Cleaning Podd Directory !!!!!! '])
    podd_command_scons = "cd %s; scons -c" % baseenv.subst('$HA_DIR')
else:
    subprocess.call(['echo', '!!!!!! Building Podd !!!!!! '])
    podd_command_scons = "cd %s; scons" % baseenv.subst('$HA_DIR')

print "podd_command_scons = %s" % podd_command_scons
os.system(podd_command_scons)

directorylist = ['./','src']
SConscript(dirs = directorylist,name='SConscript.py',exports='baseenv')

#######  End of SConstruct #########

# Local Variables:
# mode: python
# End:
