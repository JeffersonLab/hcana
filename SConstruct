#!/usr/bin/env python
###### Hall C Software Main SConstruct Build File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import sys
# import platform
# import commands
# import SCons
import subprocess

####### Check SCons version ##################
print('!!! Building the Hall C analyzer and libraries with SCons requires')
print('!!! SCons version 2.5.0 or newer.')
EnsureSConsVersion(2, 5, 0)

baseenv = Environment(ENV=os.environ, tools=["default", "symlink", "rootcint"],
                      toolpath=['podd/site_scons'])

####### Hall A Build Environment #############
#
baseenv.Append(HEAD_DIR=Dir('.').abspath)
baseenv.Append(HC_DIR=baseenv.subst('$HEAD_DIR'))
baseenv.Append(HC_SRC=os.path.join(baseenv.subst('$HC_DIR'), 'src'))
baseenv.Append(HA_DIR=os.path.join(baseenv.subst('$HC_DIR'), 'podd'))
baseenv.Append(MAIN_DIR=baseenv.subst('$HEAD_DIR'))
baseenv.Append(HA_Podd=os.path.join(baseenv.subst('$HA_DIR'), 'Podd'))
baseenv.Append(HA_DC=os.path.join(baseenv.subst('$HA_DIR'), 'hana_decode'))
baseenv.Append(HA_DB=os.path.join(baseenv.subst('$HA_DIR'), 'Database'))
baseenv.Append(MAJORVERSION='1')
baseenv.Append(MINORVERSION='1')
baseenv.Append(PATCH='0')
baseenv.Append(SOVERSION=baseenv.subst('$MAJORVERSION') + '.' + baseenv.subst('$MINORVERSION'))
baseenv.Append(VERSION=baseenv.subst('$SOVERSION') + '.' + baseenv.subst('$PATCH'))
baseenv.Append(EXTVERS='')
baseenv.Append(HC_VERSION=baseenv.subst('$VERSION') + baseenv.subst('$EXTVERS'))
print("Hall C Main Directory = %s" % baseenv.subst('$HC_DIR'))
print("Hall C Source Directory = %s" % baseenv.subst('$HC_SRC'))
print("Hall A Main Directory = %s" % baseenv.subst('$HA_DIR'))
print("Software Version = %s" % baseenv.subst('$VERSION'))
ivercode = 65536 * int(float(baseenv.subst('$SOVERSION'))) + 256 * int(
  10 * (float(baseenv.subst('$SOVERSION')) - int(float(baseenv.subst('$SOVERSION'))))) + int(
  float(baseenv.subst('$PATCH')))
baseenv.Append(VERCODE=ivercode)
baseenv.Append(CPPPATH=['$HC_DIR', '$HC_SRC', '$HA_Podd', '$HA_DC', '$HA_DB'])

# Installation setup
install_prefix = os.getenv('SCONS_INSTALL_PREFIX')
if not install_prefix:
    install_prefix = os.path.join(os.getenv('HOME'), '.local')
baseenv.Append(INSTALLDIR=install_prefix)
print('Will use INSTALLDIR = "%s"' % baseenv.subst('$INSTALLDIR'))
baseenv.Alias('install', baseenv.subst('$INSTALLDIR'))

sys.path.insert(0, os.path.join(baseenv.subst('$HA_DIR'), 'site_scons'))
import configure
import podd_util

# Default RPATH handling like CMake's default: always set in build location,
#    delete when installing
# If rpath=1 given, then set installation RPATH to installation libdir
if int(ARGUMENTS.get('rpath', 0)):
    baseenv.Replace(ADD_INSTALL_RPATH='1')
baseenv.AddMethod(podd_util.InstallWithRPATH)
# Scons 4.1.0 sets this to '-Wl,rpath=' which the Xcode 12 linker doesn't understand
if baseenv['PLATFORM'] == 'darwin':
    baseenv.Replace(RPATHPREFIX='-Wl,-rpath,')

######## Configure Section #######

configure.config(baseenv, ARGUMENTS)
configure.FindROOT(baseenv)
conf = Configure(baseenv)

if not (baseenv.GetOption('clean') or baseenv.GetOption('help')):

    if not conf.CheckCXX():
        print('!!! Your compiler and/or environment is not correctly configured.')
        Exit(1)
    # if not conf.CheckFunc('printf'):
    #         print('!!! Your compiler and/or environment is not correctly configured.')
    #         Exit(1)
    if conf.CheckCXXHeader('sstream'):
        conf.env.Append(CPPDEFINES='HAS_SSTREAM')

baseenv = conf.Finish()

######## cppcheck ###########################

proceed = "1" or "y" or "yes" or "Yes" or "Y"
if baseenv.subst('$CPPCHECK') == proceed:
    is_cppcheck = which('cppcheck')
    print("Path to cppcheck is %s\n" % is_cppcheck)

    if is_cppcheck == None:
        print('!!! cppcheck not found on this system.  Check if cppcheck is installed and in your PATH.')
        Exit(1)
    else:
        cppcheck_command = baseenv.Command('cppcheck_report.txt', [], "cppcheck --quiet --enable=all src/ 2> $TARGET")
        baseenv.AlwaysBuild(cppcheck_command)

Export('baseenv')

# print (baseenv.Dump())
# print ('CXXFLAGS = ', baseenv['CXXFLAGS'])
# print ('LINKFLAGS = ', baseenv['LINKFLAGS'])
# print ('SHLINKFLAGS = ', baseenv['SHLINKFLAGS'])

####### Start of main SConstruct ############

hallclib = 'HallC'
poddlib = 'Podd'
dclib = 'dc'
dblib = 'PoddDB'

baseenv.Append(LIBPATH=['$HC_DIR', '$HC_SRC', '$HA_Podd', '$HA_DC', '$HA_DB'])
baseenv.Append(RPATH=['$HC_SRC', '$HA_Podd', '$HA_DC', '$HA_DB'])
# baseenv.Replace(SHLIBSUFFIX = '.so')
# baseenv.Replace(SOSUFFIX = baseenv.subst('$SHLIBSUFFIX'))
# baseenv.Replace(SHLIBSUFFIX = '.so')
# baseenv.Append(SHLIBSUFFIX = '.'+baseenv.subst('$VERSION'))
# Scons 4.1.0 sets this to '-Wl,rpath=' which the Xcode 12 linker doesn't understand
if baseenv['PLATFORM'] == 'darwin':
    baseenv.Replace(RPATHPREFIX='-Wl,-rpath,')

pbaseenv = baseenv.Clone()
pbaseenv.Prepend(LIBS=[hallclib, poddlib, dclib, dblib])
baseenv.Prepend(LIBS=[poddlib, dclib, dblib])
Export('pbaseenv')

if baseenv['CXX'] == 'g++':
    gxxVersion = [int(i) for i in pbaseenv['CXXVERSION'].split('.')]
    if (gxxVersion[0] < 4) or (gxxVersion[0] == 4 and gxxVersion[1] < 8):
        print('Error: g++ version too old! Need at least g++ 4.8!')
        Exit(1)

# directorylist = ['./','src','podd','podd/src','podd/hana_decode']
# SConscript('podd/SConstruct')

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
    if 'install' in COMMAND_LINE_TARGETS:
        podd_command_scons += " install"
    if 'uninstall' in COMMAND_LINE_TARGETS:
        podd_command_scons += " uninstall"
    for key, value in ARGLIST:
        podd_command_scons += " %s=%s" % (key, value)

print("podd_command_scons = %s" % podd_command_scons)
os.system(podd_command_scons)

directorylist = ['.', 'src']
# macOS
pbaseenv.Append(LINKFLAGS = pbaseenv.subst('$LINKVERFLAG'))
SConscript(dirs=directorylist, name='SConscript.py', exports='pbaseenv')

if 'uninstall' in COMMAND_LINE_TARGETS:
  pbaseenv.Command("uninstall-scons-installed-files", None, Delete(FindInstalledFiles()))
  pbaseenv.Alias("uninstall", "uninstall-scons-installed-files")

#######  End of SConstruct #########

# Local Variables:
# mode: python
# End:
