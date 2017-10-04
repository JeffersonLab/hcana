###### Hall C Software Main SConscript File #####
###### Author:	Edward Brash (brash@jlab.org) June 2013

import os
import re
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
    if newbasefilename[1][:6] == 'Scaler':
        cmd1 = "echo '#pragma link C++ class Decoder::%s+;' >> src/HallC_LinkDef.h" % newbasefilename[1]
    else:
        cmd1 = "echo '#pragma link C++ class %s+;' >> src/HallC_LinkDef.h" % newbasefilename[1]
    os.system(cmd1)

cmd = "cat src/HallC_LinkDef.h_postamble >> src/HallC_LinkDef.h"
os.system(cmd)

hcheaders = Glob('src/*.h',exclude=['src/HallC_LinkDef.h'])+Glob('src/HallC_LinkDef.h')

pbaseenv.RootCint(roothcdict,hcheaders)
pbaseenv.Clean(roothcdict,re.sub(r'\.C\Z','_rdict.pcm',roothcdict))
pbaseenv.SharedObject(target = roothcobj, source = roothcdict)

#######  Start of main SConscript ###########

print ('LIBS = %s\n' % pbaseenv.subst('$LIBS'))

analyzer = pbaseenv.Program(target = 'hcana', source = 'src/main.o')
pbaseenv.Install('./bin',analyzer)
pbaseenv.Alias('install',['./bin'])
#pbaseenv.Clean(analyzer,)
