###### Hall C Software Source SConscript Build File #####
###### Author:  Edward Brash (brash@jlab.org) June 2013

import os
import re
import SCons.Util
Import('pbaseenv')

list = Glob('*.cxx', exclude=['main.C'])

pbaseenv.Object('main.C')

sotarget = 'HallC'

#srclib = pbaseenv.SharedLibrary(target = sotarget, source = list+['HallCDict.so'],SHLIBVERSION=['$VERSION'],LIBS=[''])
srclib = pbaseenv.SharedLibrary(target = sotarget, source = list+['HallCDict.so'],SHLIBPREFIX='../lib',LIBS=[''])
print ('Source shared library = %s\n' % srclib)

linkbase =pbaseenv.subst('$SHLIBPREFIX')+sotarget

cleantarget = linkbase+'.so.'+pbaseenv.subst('$VERSION')
localmajorcleantarget = '../'+linkbase+'.so'

print('cleantarget = %s\n' % cleantarget)
print('localmajorcleantarget = %s\n' % localmajorcleantarget)
try:
    os.symlink(cleantarget,localmajorcleantarget)
except:
    print (" Continuing ... ")

Clean(srclib,cleantarget)
Clean(srclib,localmajorcleantarget)

#baseenv.Install('../',srclib)
#baseenv.Alias('install',['../'])
