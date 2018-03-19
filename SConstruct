#!/usr/bin/env python
###### Hall C Software Main SConstruct Build File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import sys
#import platform
#import commands
import SCons
import subprocess

####### Check SCons version ##################
print('!!! Building the Hall C analyzer and libraries with SCons requires')
print('!!! SCons version 2.5.0 or newer.')
EnsureSConsVersion(2,5,0)

baseenv = Environment(ENV = os.environ,tools=["default"],toolpath=['podd/site_scons'])

####### Hall A Build Environment #############
#
baseenv.Append(HEAD_DIR= Dir('.').abspath)
baseenv.Append(HC_DIR= baseenv.subst('$HEAD_DIR'))
baseenv.Append(HC_SRC= baseenv.subst('$HC_DIR')+'/src')
baseenv.Append(HA_DIR= baseenv.subst('$HC_DIR')+'/podd')
baseenv.Append(MAIN_DIR= baseenv.subst('$HEAD_DIR'))
baseenv.Append(HA_SRC= baseenv.subst('$HA_DIR')+'/src')
baseenv.Append(HA_DC= baseenv.subst('$HA_DIR')+'/hana_decode')
baseenv.Append(MAJORVERSION = '0')
baseenv.Append(MINORVERSION = '90')
baseenv.Append(PATCH = '0')
baseenv.Append(SOVERSION = baseenv.subst('$MAJORVERSION')+'.'+baseenv.subst('$MINORVERSION'))
baseenv.Append(VERSION = baseenv.subst('$SOVERSION')+'.'+baseenv.subst('$PATCH'))
baseenv.Append(EXTVERS = '')
baseenv.Append(HC_VERSION = baseenv.subst('$VERSION')+baseenv.subst('$EXTVERS'))
print ("Hall C Main Directory = %s" % baseenv.subst('$HC_DIR'))
print ("Hall C Source Directory = %s" % baseenv.subst('$HC_SRC'))
print ("Hall A Main Directory = %s" % baseenv.subst('$HA_DIR'))
print ("Software Version = %s" % baseenv.subst('$VERSION'))
ivercode = 65536*int(float(baseenv.subst('$SOVERSION')))+ 256*int(10*(float(baseenv.subst('$SOVERSION'))-int(float(baseenv.subst('$SOVERSION')))))+ int(float(baseenv.subst('$PATCH')))
baseenv.Append(VERCODE = ivercode)
baseenv.Append(CPPPATH = ['$HC_SRC','$HA_SRC','$HA_DC'])

sys.path.insert(1,baseenv.subst('$HA_DIR'+'/site_scons'))
import configure
from rootcint import rootcint

configure.FindROOT(baseenv)
# If EVIO is set up, use it. Otherwise the Podd submodule will build it
# and we will pick it up from there
configure.FindEVIO(baseenv, build_it = False, fail_if_missing = False)

bld = Builder(action=rootcint)
baseenv.Append(BUILDERS = {'RootCint': bld})

######## cppcheck ###########################

proceed = "1" or "y" or "yes" or "Yes" or "Y"
if baseenv.subst('$CPPCHECK')==proceed:
    is_cppcheck = which('cppcheck')
    print ("Path to cppcheck is %s\n" % is_cppcheck)

    if(is_cppcheck == None):
        print('!!! cppcheck not found on this system.  Check if cppcheck is installed and in your PATH.')
        Exit(1)
    else:
        cppcheck_command = baseenv.Command('cppcheck_report.txt',[],"cppcheck --quiet --enable=all src/ 2> $TARGET")
        baseenv.AlwaysBuild(cppcheck_command)

######## Configure Section #######

if not (baseenv.GetOption('clean') or baseenv.GetOption('help')):

    configure.config(baseenv,ARGUMENTS)

    conf = Configure(baseenv)
    if not conf.CheckCXX():
        print('!!! Your compiler and/or environment is not correctly configured.')
        Exit(1)
    # if not conf.CheckFunc('printf'):
    #         print('!!! Your compiler and/or environment is not correctly configured.')
    #         Exit(1)
    if conf.CheckCXXHeader('sstream'):
        conf.env.Append(CPPDEFINES = 'HAS_SSTREAM')
    baseenv = conf.Finish()

Export('baseenv')

#print (baseenv.Dump())
#print ('CXXFLAGS = ', baseenv['CXXFLAGS'])
#print ('LINKFLAGS = ', baseenv['LINKFLAGS'])
#print ('SHLINKFLAGS = ', baseenv['SHLINKFLAGS'])

####### Start of main SConstruct ############

hallclib = 'HallC'
hallalib = 'HallA'
dclib = 'dc'
eviolib = 'evio'

baseenv.Append(LIBPATH=['$HC_DIR','$EVIO_LIB','$HA_DIR','$HC_SRC','$HA_SRC','$HA_DC'])
baseenv.Replace(SHLIBSUFFIX = '.so')
baseenv.Replace(SOSUFFIX = baseenv.subst('$SHLIBSUFFIX'))
#baseenv.Replace(SHLIBSUFFIX = '.so')
baseenv.Append(SHLIBSUFFIX = '.'+baseenv.subst('$VERSION'))

pbaseenv=baseenv.Clone()
pbaseenv.Prepend(LIBS=[hallclib,hallalib,dclib,eviolib])
baseenv.Prepend(LIBS=[hallalib,dclib,eviolib])
Export('pbaseenv')

if pbaseenv['CXX'] == 'g++':
    gxxVersion = [int(i) for i in pbaseenv['CXXVERSION'].split('.')]
    if (gxxVersion[0] < 4) or (gxxVersion[0] == 4 and gxxVersion[1] < 4):
        print('Error: g++ version too old! Need at least g++ 4.4!')
        Exit(1)

##directorylist = ['./','src','podd','podd/src','podd/hana_decode']
##SConscript('podd/SConstruct')

if baseenv.GetOption('clean'):
    subprocess.call(['echo', '!!!!!! Cleaning Podd Directory !!!!!! '])
    podd_command_scons = "cd %s; scons -c" % baseenv.subst('$HA_DIR')
else:
    subprocess.call(['echo', '!!!!!! Building Podd !!!!!! '])
    podd_command_scons = "cd %s; scons" % baseenv.subst('$HA_DIR')
    if baseenv.GetOption('num_jobs'):
        podd_command_scons += " -j%s" % (GetOption('num_jobs')) 
    if baseenv.GetOption('silent'):
        podd_command_scons += " -s"
    for key,value in ARGLIST:
        podd_command_scons += " %s=%s" % (key,value)

print ("podd_command_scons = %s" % podd_command_scons)

os.system(podd_command_scons)

directorylist = ['./','src']
SConscript(dirs = directorylist,name='SConscript.py',exports='baseenv')

#######  End of SConstruct #########

# Local Variables:
# mode: python
# End:
