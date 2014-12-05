src_files =  Glob('*.cpp')
src_files +=  Glob('*.c')
src_files += Glob('dep/msgpack-c/src/*.c')

include_path = ['dep/msgpack-c/include']

lib_path = []
libs = []

cc_flags = ['-g', '-std=c++11']

object_files = []
for node in src_files:
    sf = str(node)
    of = ''
    if sf.endswith('.c') == True:
        of = sf[ : sf.find('.c')] + '.o'
    else:
        of = sf[ : sf.find('.cpp')] + '.o'
    object_files += Object(target = 'objs/%s' % of, source = sf, CPPPATH = include_path, CCFLAGS = cc_flags)

Program(target = './objs/main', source = object_files, CPPPATH = include_path, LIBS = libs, LIBPATH = lib_path, CCFLAGS = cc_flags)