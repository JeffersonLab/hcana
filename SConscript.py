###### Hall C Software Main SConscript File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import re
import sys
import subprocess
import platform
import time
import SCons.Util
Import ('pbaseenv')

######## ROOT Dictionaries #########

roothcdict = pbaseenv.subst('$HC_DIR')+'/HallCDict.C'
roothcobj = pbaseenv.subst('$HC_SRC')+'/HallCDict.so'
 
hcheadersbase = Glob('src/*.h',exclude=['src/THcGlobals.h','src/HallC_LinkDef.h'])

cmd = "cat src/HallC_LinkDef.h_preamble > src/HallC_LinkDef.h"
os.system(cmd)

for hcheaderfile in hcheadersbase:
    filename = '%s' % hcheaderfile
    basefilename = filename.rsplit('.',1)
    newbasefilename = basefilename[0].rsplit('/',1)
    # Assume filenames beginning with Scaler are decoder classes
    if newbasefilename[1] == 'hc_compiledata':
        continue
    if newbasefilename[1][:6] == 'Scaler' or newbasefilename[1] == "TIBlobModule":
        cmd1 = "echo '#pragma link C++ class Decoder::%s+;' >> src/HallC_LinkDef.h" % newbasefilename[1]
    else:
        cmd1 = "echo '#pragma link C++ class %s+;' >> src/HallC_LinkDef.h" % newbasefilename[1]
    os.system(cmd1)

cmd = "cat src/HallC_LinkDef.h_postamble >> src/HallC_LinkDef.h"
os.system(cmd)

hcheaders = Glob('src/*.h',exclude=['src/HallC_LinkDef.h','src/hc_compiledata.h'])+Glob('src/HallC_LinkDef.h')

pbaseenv.RootCint(roothcdict,hcheaders)
pbaseenv.Clean(roothcdict,re.sub(r'\.C\Z','_rdict.pcm',roothcdict))
pbaseenv.SharedObject(target = roothcobj, source = roothcdict)

#######  write src/hc_compiledata.h header file ######

if sys.version_info >= (2, 7):
    try:
        cmd = "git rev-parse HEAD 2>/dev/null"
        gitrev = subprocess.check_output(cmd, shell=True).rstrip()
    except:
        gitrev = ''
    try:
        cmd = pbaseenv.subst('$CXX') + " --version 2>/dev/null | head -1"
        cxxver = subprocess.check_output(cmd, shell=True).rstrip()
    except:
        cxxver = ''
    # subprocess gives us byte string literals in Python 3, but we'd like
    # Unicode strings
    if sys.version_info >= (3, 0):
        gitrev = gitrev.decode()
        cxxver = cxxver.decode()
else:
    FNULL = open(os.devnull, 'w')
    try:
        gitrev = subprocess.Popen(['git', 'rev-parse', 'HEAD', '2>dev/null'],\
                    stdout=subprocess.PIPE, stderr=FNULL).communicate()[0].rstrip()
    except:
        gitrev =''
    try:
        outp = subprocess.Popen([pbaseenv.subst('$CXX'), '--version'],\
                                stdout=subprocess.PIPE, stderr=FNULL).communicate()[0]
        lines = outp.splitlines()
        cxxver = lines[0]
    except:
        cxxver = ''

compiledata = 'src/hc_compiledata.h'
f=open(compiledata,'w')
f.write('#ifndef HCANA_COMPILEDATA_H\n')
f.write('#define HCANA_COMPILEDATA_H\n')
f.write('\n')
f.write('#define HC_INCLUDEPATH "%s"\n' % (pbaseenv.subst('$HC_SRC')))
f.write('#define HC_VERSION "%s"\n' % pbaseenv.subst('$HC_VERSION'))
f.write('#define HC_DATE "%s"\n' % time.strftime("%b %d %Y"))
f.write('#define HC_DATETIME "%s"\n' % time.strftime("%a %b %d %Y"))
#f.write('#define HC_DATETIME "%s"\n' % time.strftime("%a %b %d %H:%M:%S %Z %Y"))
f.write('#define HC_PLATFORM "%s"\n' % platform.platform())
f.write('#define HC_BUILDNODE "%s"\n' % platform.node())
f.write('#define HC_BUILDDIR "%s"\n' % os.getcwd())
try:
    builduser = pbaseenv['ENV']['LOGNAME']
except:
    builduser = ''
f.write('#define HC_BUILDUSER "%s"\n' % builduser)
f.write('#define HC_GITVERS "%s"\n' % gitrev[:7])
f.write('#define HC_CXXVERS "%s"\n' % cxxver)
f.write('#define HC_ROOTVERS "%s"\n' % pbaseenv.subst('$ROOTVERS'))
f.write('#define HCANA_VERSION_CODE %s\n' % pbaseenv.subst('$VERCODE'))
f.write('#define HCANA_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))\n')
f.write('\n')
f.write('#endif\n')
f.close()

#######  Start of main SConscript ###########

print ('LIBS = %s\n' % pbaseenv.subst('$LIBS'))

# SCons seems to ignore $RPATH on macOS... sigh
if pbaseenv['PLATFORM'] == 'darwin':
    try:
        for rp in pbaseenv['RPATH']:
            pbaseenv.Append(LINKFLAGS = ['-Wl,-rpath,'+rp])
    except KeyError:
        pass

analyzer = pbaseenv.Program(target = 'hcana', source = 'src/main.o')
pbaseenv.Install('./bin',analyzer)
pbaseenv.Alias('install',['./bin'])
#pbaseenv.Clean(analyzer,)
