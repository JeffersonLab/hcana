import sys
import platform
import os
import subprocess

def config(env,args):

	if env['PLATFORM'] == 'posix':
      		if (platform.machine() == 'x86_64'):
        		print "Got a 64-bit processor, I can do a 64-bit build in theory..."
			for element in platform.architecture():
         			if (element == '32bit'):
             				print '32-bit Linux build'
             				env['MEMORYMODEL'] = '32bit'
             				import linux32
             				linux32.config(env, args)
					break
         			elif (element == '64bit'):
             				print '64-bit Linux build'
             				env['MEMORYMODEL'] = '64bit'
             				import linux64
             				linux64.config(env, args)
					break
         			else:
             				print 'Memory model not specified, so I\'m building 32-bit...'
             				env['MEMORYMODEL'] = '32bit'
             				import linux32
             				linux32.config(env, args)
      		else:
          		print '32-bit Linux Build.'
          		env['MEMORYMODEL'] = '32bit'
          		import linux32
          		linux32.config(env, args)
#	elif env['PLATFORM'] == 'win32':
#      		if (os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64' or (os.environ.has_key('PROCESSOR_ARCHITEW6432') and os.environ['PROCESSOR_ARCHITEW6432'] == 'AMD64')):
#         		print "Got a 64-bit processor, I can do a 64-bit build in theory..."
#         		if args.get('32bit', 0):
#            			print '32-bit Windows build.'
#            			#import win6432
#            			#win6432.config(env, args)
#         		elif args.get('64bit', 0):
#            			print '64-bit Windows build.'
#            			#import win64
#            			#win64.config(env, args)
#         		else:
#            			print 'Memory model not specified, so I\'m building 32-bit.'
#            			#import win6432
#            			#win6432.config(env, args)
#      		else:
#			print '32-bit Windows build.'
#         		#import win32
#         		#win32.config(env, args)
	elif env['PLATFORM'] == 'darwin':
      		print 'OS X Darwin is a 64-bit build.'
      		env['MEMORYMODEL'] = '64bit'
      		import darwin64
      		darwin64.config(env, args)
	else:
      		print 'ERROR! unrecognized platform.  Twonk.'

# which() utility
def which(program):
        def is_exe(fpath):
                return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

        fpath, fname = os.path.split(program)
        if fpath:
                if is_exe(program):
                        return program
        else:
                for path in os.environ["PATH"].split(os.pathsep):
                        path = path.strip('"')
                        exe_file = os.path.join(path, program)
                        if is_exe(exe_file):
                                return exe_file
        return None

####### ROOT Definitions ####################
def FindROOT(env, need_glibs = True):
        root_config = 'root-config'
        try:
                env.PrependENVPath('PATH',env['ENV']['ROOTSYS'] + '/bin')
        except KeyError:
                pass    # ROOTSYS not defined

        try:
                if need_glibs:
                        env.ParseConfig(root_config + ' --cflags --glibs')
                else:
                        env.ParseConfig(root_config + ' --cflags --libs')
                if sys.version_info >= (2, 7):
                        cmd = root_config + ' --cxx'
                        env.Replace(CXX = subprocess.check_output(cmd, shell=True).rstrip())
                        cmd = root_config + ' --version'
                        env.Replace(ROOTVERS = subprocess.check_output(cmd, shell=True).rstrip())
                else:
                        env.Replace(CXX = subprocess.Popen([root_config, '--cxx'],\
                                stdout=subprocess.PIPE).communicate()[0].rstrip())
                        env.Replace(ROOTVERS = subprocess.Popen([root_config,\
                                '--version'],stdout=subprocess.PIPE).communicate()[0].rstrip())
                if platform.system() == 'Darwin':
                        try:
                                env.Replace(LINKFLAGS = env['LINKFLAGS'].remove('-pthread'))
                        except:
                                pass #  '-pthread' was not present in LINKFLAGS

        except OSError:
                print('!!! Cannot find ROOT.  Check if root-config is in your PATH.')
                env.Exit(1)

#end configure.py
