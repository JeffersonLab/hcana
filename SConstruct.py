###### Hall C Software Main SConstruct Build File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import sys
import platform
import commands
import SCons

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
#print('!!! You should be using the local version of SCons, invoked with:')
#print('!!! ./podd/scons/scons.py')
print('!!! Building the Hall C analyzer and libraries with SCons requires')
print('!!! SCons version 2.1.0 or newer.')
EnsureSConsVersion(2,1,0)

####### Hall A Build Environment #############
#
# Edit for the location of your root installation here ...
#
#baseenv.Append(ROOTSYS = '/usr/local/root')
#
baseenv.Append(MAIN_DIR = Dir('.').abspath)
baseenv.Append(HC_DIR = baseenv.subst('$MAIN_DIR'))
baseenv.Append(HC_SRC = baseenv.subst('$HC_DIR')+'/src ') 
baseenv.Append(HA_DIR = baseenv.subst('$HC_DIR')+'/podd ')
baseenv.Append(HA_SRC = baseenv.subst('$HA_DIR')+'/src ') 
baseenv.Append(HA_DC = baseenv.subst('$HA_DIR')+'/hana_decode ') 
baseenv.Append(HA_SCALER = baseenv.subst('$HA_DIR')+'/hana_scaler ') 
baseenv.Append(MAJORVERSION = '1')
baseenv.Append(MINORVERSION = '5')
baseenv.Append(PATCH = '25')
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
baseenv.Append(CPPPATH = ['$HC_SRC','$HA_SRC','$HA_DC','$HA_SCALER'])

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

proceed = "1" or "y" or "yes" or "Yes" or "Y"
if baseenv.subst('$CPPCHECK')==proceed:
	try:	
		cppcheck_command = baseenv.Command('cppcheck_report.txt',[],"cppcheck --quiet --enable=all src/ 2> $TARGET")
		baseenv.AlwaysBuild(cppcheck_command)
	except OSError:
		print('!!! cppcheck not found on this system.  Check if cppcheck is in your PATH.')
		Exit(1)

####### Start of main SConstruct ############

hallclib = 'HallC'
hallalib = 'HallA'
dclib = 'dc'
scalerlib = 'scaler'

baseenv.Append(LIBPATH=['$HC_DIR','$HA_DIR','$HC_SRC','$HA_SRC','$HA_DC','$HA_SCALER'])
baseenv.Replace(SHLIBSUFFIX = '.so')
baseenv.Append(CPPDEFINES = '-DHALLC_MODS')

directorylist = ['./','src','podd','podd/src','podd/hana_decode','podd/hana_scaler']

baseenv.Append(SHLIBSUFFIX ='.'+baseenv.subst('$VERSION'))
pbaseenv=baseenv.Clone()
pbaseenv.Append(LIBS=[hallclib,hallalib,dclib,scalerlib])
baseenv.Append(LIBS=[hallalib,dclib,scalerlib])
Export('pbaseenv')

SConscript(dirs = directorylist,name='SConscript.py',exports='baseenv')

#######  End of SConstruct #########
