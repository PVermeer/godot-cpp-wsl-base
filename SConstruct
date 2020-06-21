#!python
# pylint: disable=undefined-variable
import os
import sys

opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our options
opts.Add(EnumVariable('target', "Compilation target",
                      'debug', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('bits', 'Target platform bits',
                      'default', ('default', '32', '64')))
opts.Add(EnumVariable('platform', "Compilation platform",
                      '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(EnumVariable('p', "Compilation target, alias for 'platform'",
                      '', ['', 'windows', 'x11', 'linux', 'osx']))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'no'))
opts.Add(PathVariable('target_path',
                      'The path where the lib is installed.', 'bin/cpp-library/'))
opts.Add(PathVariable('target_name', 'The library name.',
                      'lib-godot-cpp', PathVariable.PathAccept))

# Try to detect the host platform automatically.
# This is used if no `platform` argument is passed
if sys.platform.startswith('linux'):
    host_platform = 'linux'
elif sys.platform == 'darwin':
    host_platform = 'osx'
elif sys.platform == 'win32' or sys.platform == 'msys':
    host_platform = 'windows'
else:
    raise ValueError(
        'Could not detect platform automatically, please specify with '
        'platform=<platform>'
    )

# Local dependency paths, adapt them to your setup
godot_headers_path = "godot-cpp-src/godot-cpp/godot_headers/"
cpp_bindings_path = "godot-cpp-src/godot-cpp/"
cpp_library = "libgodot-cpp"

# Updates the environment with the option variables.
opts.Update(env)

# Set bits
is64 = sys.maxsize > 2**32
if (
    env['TARGET_ARCH'] == 'amd64' or
    env['TARGET_ARCH'] == 'emt64' or
    env['TARGET_ARCH'] == 'x86_64' or
    env['TARGET_ARCH'] == 'arm64-v8a'
):
    is64 = True

if (env['bits'] == 'default'):
    env['bits'] = '64' if is64 else '32'
bits = env['bits']

# Process some arguments
if env['use_llvm']:
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'

if env['p'] != '':
    env['platform'] = env['p']

if env['platform'] == '':
    print("No valid target platform selected.")
    quit()

# For the reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# Check our platform specifics
if env['platform'] == "osx":
    env['target_path'] += 'osx/'
    cpp_library += '.osx'
    env.Append(CCFLAGS=['-arch', 'x86_64'])
    env.Append(CXXFLAGS=['-std=c++17'])
    env.Append(LINKFLAGS=['-arch', 'x86_64'])
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS=['-g', '-O2'])
    else:
        env.Append(CCFLAGS=['-g', '-O3'])

elif env['platform'] in ('x11', 'linux'):
    env['target_path'] += 'x11/'
    cpp_library += '.linux'
    env.Append(CCFLAGS=['-fPIC'])
    env.Append(CXXFLAGS=['-std=c++17'])
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS=['-g3', '-Og'])
    else:
        env.Append(CCFLAGS=['-g', '-O3'])

elif env['platform'] == "windows":
    if env['bits'] == '64':
        env['target_path'] += 'win64/'
    else:
        env['target_path'] += 'win32/'

    cpp_library += '.windows'
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    env.Append(ENV=os.environ)

    # Cross-compilation using MinGW (tested in WSL)
    if host_platform == 'linux' or host_platform == 'osx':
        if env['bits'] == '64':
            env['CXX'] = 'x86_64-w64-mingw32-g++'
            env['AR'] = "x86_64-w64-mingw32-ar"
            env['RANLIB'] = "x86_64-w64-mingw32-ranlib"
            env['LINK'] = "x86_64-w64-mingw32-g++"
        elif env['bits'] == '32':
            env['CXX'] = 'i686-w64-mingw32-g++'
            env['AR'] = "i686-w64-mingw32-ar"
            env['RANLIB'] = "i686-w64-mingw32-ranlib"
            env['LINK'] = "i686-w64-mingw32-g++"

        env.Append(CCFLAGS=['-g', '-O3', '-std=c++17', '-Wwrite-strings'])
        env.Append(LINKFLAGS=[
            '--static',
            '-Wl,--no-undefined',
            '-static-libgcc',
            '-static-libstdc++',
        ])

        if env['target'] in ('debug', 'd'):
            env.Append(CPPDEFINES=['_DEBUG'])
            env.Append(LINKFLAGS=['-DEBUG'])
        else:
            env.Append(CPPDEFINES=['NDEBUG'])

        # Append .dll to target_name
        env['target_name'] += '.dll'

    else:
        env.Append(CCFLAGS=['-DWIN32', '-D_WIN32', '-D_WINDOWS',
                            '-W3', '-GR', '-D_CRT_SECURE_NO_WARNINGS'])
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS=['-EHsc', '-D_DEBUG', '-MDd'])
        else:
            env.Append(CCFLAGS=['-O2', '-EHsc', '-DNDEBUG', '-MD'])

if env['target'] in ('debug', 'd'):
    cpp_library += '.debug'
else:
    cpp_library += '.release'

cpp_library += '.' + str(bits)

# make sure our binding library is properly included
env.Append(CPPPATH=['.', godot_headers_path, cpp_bindings_path + 'include/',
                    cpp_bindings_path + 'include/core/', cpp_bindings_path + 'include/gen/'])
env.Append(LIBPATH=[cpp_bindings_path + 'bin/'])
env.Append(LIBS=[cpp_library])

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=['src/cpp/'])
sources = Glob('src/cpp/*.cpp')
sources.extend(Glob('src/cpp/*/*.cpp'))
sources.extend(Glob('src/cpp/*/*/*.cpp'))
sources.extend(Glob('src/cpp/*/*/*/*.cpp'))
sources.extend(Glob('src/cpp/*/*/*/*/*.cpp'))
sources.extend(Glob('src/cpp/*/*/*/*/*/*.cpp'))
sources.extend(Glob('src/cpp/*/*/*/*/*/*/*.cpp'))

library = env.SharedLibrary(
    target=env['target_path'] + env['target_name'], source=sources)

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
