import platform
import os

def config(env,args):

	debug = args.get('debug',0)
	standalone = args.get('standalone',0)
	cppcheck = args.get('cppcheck',0)
	checkheaders = args.get('checkheaders',0)
	
	if int(debug):
		env.Append(CXXFLAGS = '-g -O0')
	else:	
		env.Append(CXXFLAGS = '-O')
		env.Append(CPPDEFINES= '-DNDEBUG')

	if int(standalone):
		env.Append(STANDALONE= '1')

	if int(cppcheck):
		env.Append(CPPCHECK= '1')
	
	if int(checkheaders):
		env.Append(CHECKHEADERS= '1')
	
	env.Append(CXXFLAGS = '-Wall -Woverloaded-virtual')
	env.Append(CPPDEFINES = '-DLINUXVERS')

	cxxversion = env.subst('$CXXVERSION')

#	if float(cxxversion[0:2])>=4.0:
#			env.Append(CXXFLAGS = '-Wextra -Wno-missing-field-initializers')
	
	if float(cxxversion[0:2])>=3.0:
			env.Append(CPPDEFINES = '-DHAS_SSTREAM')
	
	env['SHLINKFLAGS'] = '$LINKFLAGS -shared'
	env['SHLIBSUFFIX'] = '.so'


#end linux6432.py
