#!/usr/bin/env python

import os
import sys
import subprocess

if sys.version_info < (3,):
    def decode_utf8(x):
        return x
else:
    import codecs
    def decode_utf8(x):
        return codecs.utf_8_decode(x)[0]

# Workaround for MinGW. See:
# http://www.scons.org/wiki/LongCmdLinesOnWin32
if (os.name=="nt"):
    import subprocess

    def mySubProcess(cmdline,env):
        #print "SPAWNED : " + cmdline
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        proc = subprocess.Popen(cmdline, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, startupinfo=startupinfo, shell = False, env = env)
        data, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err.decode("utf-8"))
            print("=====")
        return rv

    def mySpawn(sh, escape, cmd, args, env):

        newargs = ' '.join(args[1:])
        cmdline = cmd + " " + newargs

        rv=0
        if len(cmdline) > 32000 and cmd.endswith("ar") :
            cmdline = cmd + " " + args[1] + " " + args[2] + " "
            for i in range(3,len(args)) :
                rv = mySubProcess( cmdline + args[i], env )
                if rv :
                    break
        else:
            rv = mySubProcess( cmdline, env )

        return rv

def add_sources(sources, dir, extension):
    for f in os.listdir(dir):
        if f.endswith('.' + extension):
            sources.append(dir + '/' + f)


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

opts = Variables([], ARGUMENTS)
opts.Add(EnumVariable(
    'platform',
    'Target platform',
    host_platform,
    allowed_values=('linux', 'osx', 'windows', 'android', 'ios'),
    ignorecase=2
))
opts.Add(EnumVariable(
    'bits',
    'Target platform bits',
    'default',
    ('default', '32', '64')
))
opts.Add(BoolVariable(
    'use_llvm',
    'Use the LLVM compiler - only effective when targeting Linux',
    False
))
opts.Add(BoolVariable(
    'use_mingw',
    'Use the MinGW compiler instead of MSVC - only effective on Windows',
    False
))
# Must be the same setting as used for cpp_bindings
opts.Add(EnumVariable(
    'target',
    'Compilation target',
    'debug',
    allowed_values=('debug', 'release'),
    ignorecase=2
))
opts.Add(PathVariable(
    'headers_dir',
    'Path to the directory containing Godot headers',
    'godot_headers',
    PathVariable.PathIsDir
))
opts.Add(PathVariable(
    'custom_api_file',
    'Path to a custom JSON API file',
    None,
    PathVariable.PathIsFile
))
opts.Add(BoolVariable(
    'generate_bindings',
    'Generate GDNative API bindings',
    False
))
opts.Add(EnumVariable(
    'android_arch',
    'Target Android architecture',
    'armv7',
    ['armv7','arm64v8','x86','x86_64']
))
opts.Add(EnumVariable(
    'ios_arch',
    'Target iOS architecture',
    'arm64',
    ['armv7', 'arm64', 'x86_64']
))
opts.Add(
    'IPHONEPATH',
    'Path to iPhone toolchain',
    '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain',
)
opts.Add(
    'android_api_level',
    'Target Android API level',
    '18' if ARGUMENTS.get("android_arch", 'armv7') in ['armv7', 'x86'] else '21'
)
opts.Add(
    'ANDROID_NDK_ROOT',
    'Path to your Android NDK installation. By default, uses ANDROID_NDK_ROOT from your defined environment variables.',
    os.environ.get("ANDROID_NDK_ROOT", None)
)
opts.Add(
    'ndk_toolchain',
    'Path to your pre-made standalone NDK toolchain. If not defined, and NDK version is lower than 19, Scons will generate a temporary toolchain.',
    ''
)

env = Environment(ENV = os.environ)
opts.Update(env)
Help(opts.GenerateHelpText(env))

is64 = sys.maxsize > 2**32
if (
    env['TARGET_ARCH'] == 'amd64' or
    env['TARGET_ARCH'] == 'emt64' or
    env['TARGET_ARCH'] == 'x86_64' or
    env['TARGET_ARCH'] == 'arm64-v8a'
):
    is64 = True

if env['bits'] == 'default':
    env['bits'] = '64' if is64 else '32'

# This makes sure to keep the session environment variables on Windows.
# This way, you can run SCons in a Visual Studio 2017 prompt and it will find
# all the required tools
if host_platform == 'windows' and env['platform'] != 'android':
    if env['bits'] == '64':
        env = Environment(TARGET_ARCH='amd64')
    elif env['bits'] == '32':
        env = Environment(TARGET_ARCH='x86')

    opts.Update(env)

if env['platform'] == 'linux':
    if env['use_llvm']:
        env['CXX'] = 'clang++'

    env.Append(CCFLAGS=['-fPIC', '-g', '-std=c++14', '-Wwrite-strings'])
    env.Append(LINKFLAGS=["-Wl,-R,'$$ORIGIN'"])

    if env['target'] == 'debug':
        env.Append(CCFLAGS=['-Og'])
    elif env['target'] == 'release':
        env.Append(CCFLAGS=['-O3'])

    if env['bits'] == '64':
        env.Append(CCFLAGS=['-m64'])
        env.Append(LINKFLAGS=['-m64'])
    elif env['bits'] == '32':
        env.Append(CCFLAGS=['-m32'])
        env.Append(LINKFLAGS=['-m32'])

elif env['platform'] == 'osx':
    # Use Clang on macOS by default
    env['CXX'] = 'clang++'

    if env['bits'] == '32':
        raise ValueError(
            'Only 64-bit builds are supported for the macOS target.'
        )

    env.Append(CCFLAGS=['-g', '-std=c++14', '-arch', 'x86_64'])
    env.Append(LINKFLAGS=[
        '-arch',
        'x86_64',
        '-framework',
        'Cocoa',
        '-Wl,-undefined,dynamic_lookup',
    ])

    if env['target'] == 'debug':
        env.Append(CCFLAGS=['-Og'])
    elif env['target'] == 'release':
        env.Append(CCFLAGS=['-O3'])

elif env['platform'] == 'ios':
    if env['ios_arch'] == 'x86_64':
        sdk_name = 'iphonesimulator'
        env.Append(CCFLAGS=['-mios-simulator-version-min=10.0'])
    else:
        sdk_name = 'iphoneos'
        env.Append(CCFLAGS=['-miphoneos-version-min=10.0'])

    try:
        sdk_path = decode_utf8(subprocess.check_output(['xcrun', '--sdk', sdk_name, '--show-sdk-path']).strip())
    except (subprocess.CalledProcessError, OSError):
        raise ValueError("Failed to find SDK path while running xcrun --sdk {} --show-sdk-path.".format(sdk_name))

    compiler_path = env['IPHONEPATH'] + '/usr/bin/'
    env['ENV']['PATH'] = env['IPHONEPATH'] + "/Developer/usr/bin/:" + env['ENV']['PATH']

    env['CC'] = compiler_path + 'clang'
    env['CXX'] = compiler_path + 'clang++'
    env['AR'] = compiler_path + 'ar'
    env['RANLIB'] = compiler_path + 'ranlib'

    env.Append(CCFLAGS=['-g', '-std=c++14', '-arch', env['ios_arch'], '-isysroot', sdk_path])
    env.Append(LINKFLAGS=[
        '-arch',
        env['ios_arch'],
        '-framework',
        'Cocoa',
        '-Wl,-undefined,dynamic_lookup',
        '-isysroot', sdk_path,
        '-F' + sdk_path
    ])

    if env['target'] == 'debug':
        env.Append(CCFLAGS=['-Og'])
    elif env['target'] == 'release':
        env.Append(CCFLAGS=['-O3'])

elif env['platform'] == 'windows':
    if host_platform == 'windows' and not env['use_mingw']:
        # MSVC
        env.Append(LINKFLAGS=['/WX'])
        if env['target'] == 'debug':
            env.Append(CCFLAGS=['/Z7', '/Od', '/EHsc', '/D_DEBUG', '/MDd'])
        elif env['target'] == 'release':
            env.Append(CCFLAGS=['/O2', '/EHsc', '/DNDEBUG', '/MD'])

    elif host_platform == 'linux' or host_platform == 'osx':
        # Cross-compilation using MinGW
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
    elif host_platform == 'windows' and env['use_mingw']:
        env = env.Clone(tools=['mingw'])
        env["SPAWN"] = mySpawn

    # Native or cross-compilation using MinGW
    if host_platform == 'linux' or host_platform == 'osx' or env['use_mingw']:
        env.Append(CCFLAGS=['-g', '-O3', '-std=c++14', '-Wwrite-strings'])
        env.Append(LINKFLAGS=[
            '--static',
            '-Wl,--no-undefined',
            '-static-libgcc',
            '-static-libstdc++',
        ])
elif env['platform'] == 'android':
    if host_platform == 'windows':
        env = env.Clone(tools=['mingw'])
        env["SPAWN"] = mySpawn

    standalone = False
    # Check if standalone toolchain is present
    if env['ndk_toolchain']:
        print("Standalone toolchain defined. Ignoring other Android scons options (android_arch, android_api_level, ANDROID_NDK_ROOT)")
        toolchain = env['ndk_toolchain']
        standalone = True
    else: 
        # Verify NDK root
        if not 'ANDROID_NDK_ROOT' in env:
            raise ValueError("To build for Android, ANDROID_NDK_ROOT must be defined. Please set ANDROID_NDK_ROOT to the root folder of your Android NDK installation.")
    
        # Get NDK version
        # If it can't auto-detect the version, it will be set to the lowest version
        # supported and try to construct a toolchain as best as it can
        ndk_version = 17
        if os.path.exists(env['ANDROID_NDK_ROOT'] + "/source.properties"):
            version_line = ""
            with open(env['ANDROID_NDK_ROOT'] + "/source.properties", "r") as props:
                for line in props:
                    if line.startswith("Pkg.Revision"):
                        version_line = line
                        break
            if version_line:
                equal_pos = version_line.find("=")
                dot_pos = version_line.find(".", equal_pos)
                if equal_pos != -1:
                    if dot_pos != -1:
                        ndk_version = int(version_line[equal_pos+1:dot_pos])
                    else:
                        ndk_version = int(version_line[equal_pos+1:])
                print("Android NDK Version: " + str(ndk_version))
            else:
                print("Could not detect NDK version, assuming ndk_version <= " + str(ndk_version))
            if ndk_version < 17:
                raise ValueError("Only NDK revisions 17c and up are supported. Please download the latest NDK revision and set ANDROID_NDK_ROOT to it.")
        else:
            print("Could not detect NDK version, assuming ndk_version <= " + str(ndk_version))
        
        # Validate API level
        api_level = int(env['android_api_level'])
        if env['android_arch'] in ['x86_64', 'arm64v8'] and api_level < 21:
            print("WARN: 64-bit Android architectures require an API level of at least 21; setting android_api_level=21")
            env['android_api_level'] = '21'
            api_level = 21
        
        # Setup toolchain
        if ndk_version >= 19: # In-place toolchain usage available for revision 19 and up
            toolchain = env['ANDROID_NDK_ROOT'] + "/toolchains/llvm/prebuilt/"
            if host_platform == "windows":
                toolchain += "windows"
                import platform as pltfm
                if pltfm.machine().endswith("64"):
                    toolchain += "-x86_64"
            elif host_platform == "linux":
                toolchain += "linux-x86_64"
            elif host_platform == "osx":
                toolchain += "darwin-x86_64"
            env.PrependENVPath('PATH', toolchain + "/bin") # This does nothing half of the time, but we'll put it here anyways
        else:
            standalone = True
            toolchain = "./tempchain/" "ndk" + str(ndk_version) + "-" + env['android_arch'] + "-" + env['android_api_level']
            # Gotta run make_standalone_toolchain.py
            print("NDK version is less than 19. Constructing standalone toolchain at \"" + toolchain + "\"")
            print("Note: Compatability may be worse with older NDKs. Please upgrade as soon as possible.")
            
            import subprocess
            standalone_arch = {
                "armv7": "arm",
                "arm64v8": "arm64",
                "x86" : "x86",
                "x86_64" : "x86_64"
            }
            initial_call = []

            if not os.path.exists(toolchain):
                initial_call = ["python", env['ANDROID_NDK_ROOT'] + "/build/tools/make_standalone_toolchain.py", "--api=" + env['android_api_level'], "-v"]

                err = subprocess.call(initial_call + [
                "--arch=" + standalone_arch[env['android_arch']],
                "--stl=libc++",
                "--install-dir=" + toolchain,
                ])

                # 0 == ok
                # 1 == already exists
                if err not in [0, 1]:
                    print("ERR: Something went wrong while creating the NDK toolchain (Error code " + str(err) + ")")
                    Exit(err)
            else:
                print("Toolchain already generated.")
            

    # Get architecture info
    arch_info_table = {
        "armv7" : {
            "march":"armv7-a", "target":"armv7a-linux-androideabi", "tool_path":"arm-linux-androideabi", "compiler_path":"armv7a-linux-androideabi",
            "ccflags" : ['-mfpu=neon']
            },
        "arm64v8" : {
            "march":"armv8-a", "target":"aarch64-linux-android", "tool_path":"aarch64-linux-android", "compiler_path":"aarch64-linux-android",
            "ccflags" : []
            },
        "x86" : {
            "march":"i686", "target":"i686-linux-android", "tool_path":"i686-linux-android", "compiler_path":"i686-linux-android",
            "ccflags" : ['-mstackrealign']
            },
        "x86_64" : {"march":"x86-64", "target":"x86_64-linux-android", "tool_path":"x86_64-linux-android", "compiler_path":"x86_64-linux-android",
            "ccflags" : []
        }
    }
    arch_info = arch_info_table[env['android_arch']]

    cmd = ""
    if host_platform == "windows":
        cmd = ".cmd"

    # Setup tools
    if standalone:
        if os.path.exists(toolchain + "/bin/" + arch_info["tool_path"] + "-clang" + cmd):
            cc = "clang"
            cxx = "clang++"
            # I'd use NDK version for this, but that isn't defined when the toolchain is manually set.
        else:
            cc = "gcc"
            cxx = "g++"
        env['CC'] = '"' + toolchain + "/bin/" + arch_info["tool_path"] + '-' + cc + cmd + '"'
        env['CXX'] = '"' + toolchain + "/bin/" + arch_info["tool_path"] + '-' + cxx + cmd + '"'
    else:
        env['CC'] = '"' + toolchain + "/bin/clang" + '"'
        env['CXX'] = '"' + toolchain + "/bin/clang++" + '"'
    env['AR'] = '"' + toolchain + "/bin/" + arch_info['tool_path'] + "-ar" + '"'
    env["RANLIB"] = '"' + toolchain + "/bin/" + arch_info['tool_path'] + "-ranlib" + '"'

    if not standalone:
        env.Append(CCFLAGS=['--target=' + arch_info['target'] + env['android_api_level'], '-march=' + arch_info['march']])
    env.Append(CCFLAGS=arch_info['ccflags'] + ['-fPIC', "-std=c++11"])

env.Append(CPPPATH=[
    '.',
    env['headers_dir'],
    'include',
    'include/gen',
    'include/core',
])

# Generate bindings?
json_api_file = ''

if 'custom_api_file' in env:
    json_api_file = env['custom_api_file']
else:
    json_api_file = os.path.join(os.getcwd(), 'godot_headers', 'api.json')

if env['generate_bindings']:
    # Actually create the bindings here
    import binding_generator

    binding_generator.generate_bindings(json_api_file)

# Sources to compile
sources = []
add_sources(sources, 'src/core', 'cpp')
add_sources(sources, 'src/gen', 'cpp')

arch_suffix = env['bits']
if env['platform'] == 'android':
    arch_suffix = env['android_arch']
if env['platform'] == 'ios':
    arch_suffix = env['ios_arch']

library = env.StaticLibrary(
    target='bin/' + 'libgodot-cpp.{}.{}.{}{}'.format(
        env['platform'],
        env['target'],
        arch_suffix,
        env['LIBSUFFIX']
    ), source=sources
)
Default(library)
Clean(library, "tempchain")
