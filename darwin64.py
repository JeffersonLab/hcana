import platform
import os

def config(env,args):

	debug = args.get('debug',0)
	standalone = args.get('standalone',0)
	if int(debug):
		env.Append(CXXFLAGS = '-g -O0')
	else:	
		env.Append(CXXFLAGS = '-O')
		env.Append(CPPDEFINES= '-DNDEBUG')

	if int(standalone):
		env.Append(STANDALONE= '1')

	#env.Append(CXXFLAGS = '-Wall -Woverloaded-virtual -pthread -rdynamic')
	env.Append(CXXFLAGS = '-Wall -Woverloaded-virtual')
	env.Append(CPPDEFINES = '-DMACVERS')

	cxxversion = env.subst('$CXXVERSION')

#	if float(cxxversion[0:2])>=4.0:
#			env.Append(CXXFLAGS = '-Wextra -Wno-missing-field-initializers')
	
	if float(cxxversion[0:2])>=3.0:
			env.Append(CPPDEFINES = '-DHAS_SSTREAM')
	
	env['SHLINKFLAGS'] = '$LINKFLAGS -shared -Wl,-undefined,dynamic_lookup'
	env['SHLIBSUFFIX'] = '.so'


#end darwin64.py
