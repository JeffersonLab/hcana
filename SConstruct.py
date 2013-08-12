###### Hall C Software Main SConstruct Build File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import sys
import platform
import commands

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
baseenv.Append(SOVERSION = '1.5')
baseenv.Append(PATCH = '24')
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

rootsys = baseenv['ENV']['ROOTSYS']
baseenv.Append(ROOTCONFIG = rootsys+'/bin/root-config')
baseenv.Append(ROOTCINT = rootsys+'/bin/rootcint')

try:
	baseenv.ParseConfig('$ROOTCONFIG --cflags')
	baseenv.ParseConfig('$ROOTCONFIG --libs')
	#baseenv.MergeFlags('-fPIC')
except OSError:
	print "ROOT not found!!"
	exit(1)

bld = Builder(action=rootcint)
baseenv.Append(BUILDERS = {'RootCint': bld})

####### Start of main SConstruct ############

hallclib = 'HallC'
hallalib = 'HallA'
dclib = 'dc'
scalerlib = 'scaler'

baseenv.Append(LIBPATH=['$HC_SRC','$HA_SRC','$HA_DC','$HA_SCALER'])
baseenv.Append(LIBS=[hallclib,hallalib,dclib,scalerlib])
baseenv.Replace(SHLIBSUFFIX = '.so')

directorylist = ['./','src','podd','podd/src','podd/hana_decode','podd/hana_scaler']

SConscript(dirs = directorylist,name='SConscript.py',exports='baseenv')

#######  End of SConstruct #########
