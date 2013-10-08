import platform
import os

def config(env,args):

	if env['PLATFORM'] == 'posix':
      		if (platform.machine() == 'x86_64'):
        		print "Got a 64-bit processor, I can do a 64-bit build in theory..."
         		if args.get('32bit', 0):
             			print '32-bit Linux build'
             			env['MEMORYMODEL'] = '32bit'
             			import linux6432
             			linux6432.config(env, args)
         		elif args.get('64bit', 0):
             			env['MEMORYMODEL'] = '64bit'
             			import linux64
             			linux64.config(env, args)
         		else:
             			print 'Memory model not specified, so I\'m building 32-bit...'
             			env['MEMORYMODEL'] = '32bit'
             			import linux6432
             			linux6432.config(env, args)
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

#end configure.py
