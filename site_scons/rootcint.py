# rootcint.py

import os

def rootcint(target,source,env):
        """Executes the ROOT dictionary generator over a list of headers."""
        dictname = target[0]
        cpppath = env.subst('$_CCCOMCOM')
#        ccflags = env.subst('$CCFLAGS')
#        print ("Doing rootcint call now ...")
        headers = ""
        for f in source:
                headers += str(f) + " "
        command = "rootcint -f %s -c %s %s" % (dictname,cpppath,headers)
#	print ('RootCint Command = %s\n' % command)
        ok = os.system(command)
        return ok

