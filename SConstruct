import os

env=Environment(CC='gcc')

sources=[Glob('src/*.c')]
libs=['ccore', 'X11', 'Xrandr', 'Xinerama', 'Xi', 'GL', 'GLU', 'pthread']
libpaths=['/usr/lib', '/usr/local/lib', '.']

env.Append(CCFLAGS=['-g'])
env.Append(CCFLAGS=['-Wall'])

env.Program(target='bin/testgame', source=sources, LIBS=[libs], LIBPATH=libpaths)
