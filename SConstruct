env = Environment(
    CC='g++',
    CCFLAGS='-std=c++11 -pedantic -Wall -Wextra -O3',
)

SConscript('lib/gust/SConscript', 'env', variant_dir='.gust', duplicate=0)
env.Append(LIBS='gust')
env.Append(LIBPATH='.gust/build')
env.Append(CPPPATH=[
    'lib/gust/lib',
    'lib/gust/src',
    'lib/gust/src/platform/desktop'
])

env.Program(target='bin/arcball', source=Glob('src/*.cpp'))
