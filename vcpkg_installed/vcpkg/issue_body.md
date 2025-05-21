Package: openal-soft:x64-windows@1.23.1

**Host Environment**

- Host: x64-windows
- Compiler: MSVC 19.41.34120.0
-    vcpkg-tool version: 2024-06-10-02590c430e4ed9215d27870138c2e579cc338772
    vcpkg-readonly: true
    vcpkg-scripts version: 6f1ddd6b6878e7e66fcc35c65ba1d8feec2e01f8

**To Reproduce**

`vcpkg install `

**Failure logs**

```
-- Using cached kcat-openal-soft-1.23.1.tar.gz.
-- Cleaning sources at C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean. Use --editable to skip cleaning for the packages you specify.
-- Extracting source C:/Users/aryma/AppData/Local/vcpkg/downloads/kcat-openal-soft-1.23.1.tar.gz
-- Applying patch c12ada68951ea67a59bef7d4fcdf22334990c12a.patch
-- Using source at C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean
-- Found external ninja('1.11.0').
-- Configuring x64-windows
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:112 (message):
    Command failed: "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v
    Working Directory: C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel/vcpkg-parallel-configure
    Error code: 1
    See logs for more information:
      C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\config-x64-windows-dbg-CMakeCache.txt.log
      C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\config-x64-windows-rel-CMakeCache.txt.log
      C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\config-x64-windows-out.log

Call Stack (most recent call first):
  C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/x64-windows/share/vcpkg-cmake/vcpkg_cmake_configure.cmake:261 (vcpkg_execute_required_process)
  C:/Users/aryma/AppData/Local/vcpkg/registries/git-trees/d23d6573247830bd439e418fe0e31321de778d85/portfile.cmake:40 (vcpkg_cmake_configure)
  scripts/ports.cmake:175 (include)



```

<details><summary>C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\config-x64-windows-out.log</summary>

```
[1/2] "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -E chdir ".." "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/pkgs/openal-soft_x64-windows" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DALSOFT_BACKEND_PIPEWIRE=OFF" "-DLIBTYPE=SHARED" "-DALSOFT_UTILS=OFF" "-DALSOFT_NO_CONFIG_UTIL=ON" "-DALSOFT_EXAMPLES=OFF" "-DALSOFT_CONFIG=OFF" "-DALSOFT_HRTF_DEFS=OFF" "-DALSOFT_AMBDEC_PRESETS=OFF" "-DALSOFT_BACKEND_ALSA=OFF" "-DALSOFT_BACKEND_OSS=OFF" "-DALSOFT_BACKEND_SOLARIS=OFF" "-DALSOFT_BACKEND_SNDIO=OFF" "-DALSOFT_BACKEND_PORTAUDIO=OFF" "-DALSOFT_BACKEND_PULSEAUDIO=OFF" "-DALSOFT_BACKEND_COREAUDIO=OFF" "-DALSOFT_BACKEND_JACK=OFF" "-DALSOFT_BACKEND_OPENSL=" "-DALSOFT_BACKEND_WAVE=ON" "-DALSOFT_BACKEND_WINMM=OFF" "-DALSOFT_BACKEND_DSOUND=OFF" "-DALSOFT_REQUIRE_WASAPI=ON" "-DALSOFT_CPUEXT_NEON=OFF" "-DCMAKE_DISABLE_FIND_PACKAGE_WindowsSDK=ON" "-DCMAKE_MAKE_PROGRAM=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
FAILED: ../CMakeCache.txt 
"C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -E chdir ".." "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/pkgs/openal-soft_x64-windows" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DALSOFT_BACKEND_PIPEWIRE=OFF" "-DLIBTYPE=SHARED" "-DALSOFT_UTILS=OFF" "-DALSOFT_NO_CONFIG_UTIL=ON" "-DALSOFT_EXAMPLES=OFF" "-DALSOFT_CONFIG=OFF" "-DALSOFT_HRTF_DEFS=OFF" "-DALSOFT_AMBDEC_PRESETS=OFF" "-DALSOFT_BACKEND_ALSA=OFF" "-DALSOFT_BACKEND_OSS=OFF" "-DALSOFT_BACKEND_SOLARIS=OFF" "-DALSOFT_BACKEND_SNDIO=OFF" "-DALSOFT_BACKEND_PORTAUDIO=OFF" "-DALSOFT_BACKEND_PULSEAUDIO=OFF" "-DALSOFT_BACKEND_COREAUDIO=OFF" "-DALSOFT_BACKEND_JACK=OFF" "-DALSOFT_BACKEND_OPENSL=" "-DALSOFT_BACKEND_WAVE=ON" "-DALSOFT_BACKEND_WINMM=OFF" "-DALSOFT_BACKEND_DSOUND=OFF" "-DALSOFT_REQUIRE_WASAPI=ON" "-DALSOFT_CPUEXT_NEON=OFF" "-DCMAKE_DISABLE_FIND_PACKAGE_WindowsSDK=ON" "-DCMAKE_MAKE_PROGRAM=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
CMake Deprecation Warning at CMakeLists.txt:3 (cmake_minimum_required):
  Compatibility with CMake < 3.5 will be removed from a future version of
  CMake.

  Update the VERSION argument <min> value or use a ...<max> suffix to tell
  CMake that the project does not need compatibility with older versions.


-- The C compiler identification is MSVC 19.41.34120.0
-- The CXX compiler identification is MSVC 19.41.34120.0
-- Detecting C compiler ABI info
CMake Warning in C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-8a8kpw/CMakeLists.txt:
  The object file directory

    C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-8a8kpw/CMakeFiles/cmTC_b4148.dir/./

  has 246 characters.  The maximum full path to an object file is 250
  characters (see CMAKE_OBJECT_PATH_MAX).  Object file

    CMakeCCompilerABI.c.obj

...
Skipped 7 lines
...
  The object file directory

    C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-jjd0xr/CMakeFiles/cmTC_20249.dir/./

  has 246 characters.  The maximum full path to an object file is 250
  characters (see CMAKE_OBJECT_PATH_MAX).  Object file

    testCCompiler.c.obj

  cannot be safely placed under this directory.  The build may not work
  correctly.


-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe - broken
CMake Error at C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeTestCCompiler.cmake:67 (message):
  The C compiler

    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe"

  is not able to compile a simple test program.

  It fails with the following output:

    Change Dir: 'C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel/CMakeFiles/CMakeScratch/TryCompile-jjd0xr'
    
    Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_20249
    [1/2] C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1441~1.341\bin\Hostx64\x64\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /MP   /MDd /Zi /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\cmTC_20249.dir\testCCompiler.c.obj /FdCMakeFiles\cmTC_20249.dir\ /FS -c "C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\x64-windows-rel\CMakeFiles\CMakeScratch\TryCompile-jjd0xr\testCCompiler.c"
    FAILED: CMakeFiles/cmTC_20249.dir/testCCompiler.c.obj 
    C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1441~1.341\bin\Hostx64\x64\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /MP   /MDd /Zi /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\cmTC_20249.dir\testCCompiler.c.obj /FdCMakeFiles\cmTC_20249.dir\ /FS -c "C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\x64-windows-rel\CMakeFiles\CMakeScratch\TryCompile-jjd0xr\testCCompiler.c"
    C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\x64-windows-rel\CMakeFiles\CMakeScratch\TryCompile-jjd0xr\testCCompiler.c : fatal error C1083: Cannot open compiler generated file: '': Invalid argument
    ninja: build stopped: subcommand failed.
    
    

  

  CMake will not be able to correctly generate this project.
Call Stack (most recent call first):
  CMakeLists.txt:65 (project)


-- Configuring incomplete, errors occurred!
[2/2] "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -E chdir "../../x64-windows-dbg" "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_INSTALL_PREFIX=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/pkgs/openal-soft_x64-windows/debug" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DALSOFT_BACKEND_PIPEWIRE=OFF" "-DLIBTYPE=SHARED" "-DALSOFT_UTILS=OFF" "-DALSOFT_NO_CONFIG_UTIL=ON" "-DALSOFT_EXAMPLES=OFF" "-DALSOFT_CONFIG=OFF" "-DALSOFT_HRTF_DEFS=OFF" "-DALSOFT_AMBDEC_PRESETS=OFF" "-DALSOFT_BACKEND_ALSA=OFF" "-DALSOFT_BACKEND_OSS=OFF" "-DALSOFT_BACKEND_SOLARIS=OFF" "-DALSOFT_BACKEND_SNDIO=OFF" "-DALSOFT_BACKEND_PORTAUDIO=OFF" "-DALSOFT_BACKEND_PULSEAUDIO=OFF" "-DALSOFT_BACKEND_COREAUDIO=OFF" "-DALSOFT_BACKEND_JACK=OFF" "-DALSOFT_BACKEND_OPENSL=" "-DALSOFT_BACKEND_WAVE=ON" "-DALSOFT_BACKEND_WINMM=OFF" "-DALSOFT_BACKEND_DSOUND=OFF" "-DALSOFT_REQUIRE_WASAPI=ON" "-DALSOFT_CPUEXT_NEON=OFF" "-DCMAKE_DISABLE_FIND_PACKAGE_WindowsSDK=ON" "-DCMAKE_MAKE_PROGRAM=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
FAILED: ../../x64-windows-dbg/CMakeCache.txt 
"C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -E chdir "../../x64-windows-dbg" "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" "C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean" "-G" "Ninja" "-DCMAKE_BUILD_TYPE=Debug" "-DCMAKE_INSTALL_PREFIX=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/pkgs/openal-soft_x64-windows/debug" "-DFETCHCONTENT_FULLY_DISCONNECTED=ON" "-DALSOFT_BACKEND_PIPEWIRE=OFF" "-DLIBTYPE=SHARED" "-DALSOFT_UTILS=OFF" "-DALSOFT_NO_CONFIG_UTIL=ON" "-DALSOFT_EXAMPLES=OFF" "-DALSOFT_CONFIG=OFF" "-DALSOFT_HRTF_DEFS=OFF" "-DALSOFT_AMBDEC_PRESETS=OFF" "-DALSOFT_BACKEND_ALSA=OFF" "-DALSOFT_BACKEND_OSS=OFF" "-DALSOFT_BACKEND_SOLARIS=OFF" "-DALSOFT_BACKEND_SNDIO=OFF" "-DALSOFT_BACKEND_PORTAUDIO=OFF" "-DALSOFT_BACKEND_PULSEAUDIO=OFF" "-DALSOFT_BACKEND_COREAUDIO=OFF" "-DALSOFT_BACKEND_JACK=OFF" "-DALSOFT_BACKEND_OPENSL=" "-DALSOFT_BACKEND_WAVE=ON" "-DALSOFT_BACKEND_WINMM=OFF" "-DALSOFT_BACKEND_DSOUND=OFF" "-DALSOFT_REQUIRE_WASAPI=ON" "-DALSOFT_CPUEXT_NEON=OFF" "-DCMAKE_DISABLE_FIND_PACKAGE_WindowsSDK=ON" "-DCMAKE_MAKE_PROGRAM=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" "-DBUILD_SHARED_LIBS=ON" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows" "-DVCPKG_SET_CHARSET_FLAG=ON" "-DVCPKG_PLATFORM_TOOLSET=v143" "-DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=ON" "-DCMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY=ON" "-DCMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP=TRUE" "-DCMAKE_VERBOSE_MAKEFILE=ON" "-DVCPKG_APPLOCAL_DEPS=OFF" "-DCMAKE_TOOLCHAIN_FILE=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION=ON" "-DVCPKG_CXX_FLAGS=" "-DVCPKG_CXX_FLAGS_RELEASE=" "-DVCPKG_CXX_FLAGS_DEBUG=" "-DVCPKG_C_FLAGS=" "-DVCPKG_C_FLAGS_RELEASE=" "-DVCPKG_C_FLAGS_DEBUG=" "-DVCPKG_CRT_LINKAGE=dynamic" "-DVCPKG_LINKER_FLAGS=" "-DVCPKG_LINKER_FLAGS_RELEASE=" "-DVCPKG_LINKER_FLAGS_DEBUG=" "-DVCPKG_TARGET_ARCHITECTURE=x64" "-DCMAKE_INSTALL_LIBDIR:STRING=lib" "-DCMAKE_INSTALL_BINDIR:STRING=bin" "-D_VCPKG_ROOT_DIR=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg" "-D_VCPKG_INSTALLED_DIR=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed" "-DVCPKG_MANIFEST_INSTALL=OFF"
CMake Deprecation Warning at CMakeLists.txt:3 (cmake_minimum_required):
  Compatibility with CMake < 3.5 will be removed from a future version of
  CMake.

  Update the VERSION argument <min> value or use a ...<max> suffix to tell
  CMake that the project does not need compatibility with older versions.


-- The C compiler identification is MSVC 19.41.34120.0
-- The CXX compiler identification is MSVC 19.41.34120.0
-- Detecting C compiler ABI info
CMake Warning in C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-43l1ro/CMakeLists.txt:
  The object file directory

    C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-43l1ro/CMakeFiles/cmTC_a4602.dir/./

  has 246 characters.  The maximum full path to an object file is 250
  characters (see CMAKE_OBJECT_PATH_MAX).  Object file

    CMakeCCompilerABI.c.obj

  cannot be safely placed under this directory.  The build may not work
  correctly.


-- Detecting C compiler ABI info - failed
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe
CMake Warning in C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-54f34m/CMakeLists.txt:
  The object file directory

    C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-54f34m/CMakeFiles/cmTC_96629.dir/./

  has 246 characters.  The maximum full path to an object file is 250
  characters (see CMAKE_OBJECT_PATH_MAX).  Object file

    testCCompiler.c.obj

  cannot be safely placed under this directory.  The build may not work
  correctly.


-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe - broken
CMake Error at C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29/Modules/CMakeTestCCompiler.cmake:67 (message):
  The C compiler

    "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe"

  is not able to compile a simple test program.

  It fails with the following output:

    Change Dir: 'C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg/CMakeFiles/CMakeScratch/TryCompile-54f34m'
    
    Run Build Command(s): "C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe" -v cmTC_96629
    [1/2] C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1441~1.341\bin\Hostx64\x64\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /MP   /MDd /Zi /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\cmTC_96629.dir\testCCompiler.c.obj /FdCMakeFiles\cmTC_96629.dir\ /FS -c "C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\x64-windows-dbg\CMakeFiles\CMakeScratch\TryCompile-54f34m\testCCompiler.c"
    FAILED: CMakeFiles/cmTC_96629.dir/testCCompiler.c.obj 
    C:\PROGRA~1\MIB055~1\2022\COMMUN~1\VC\Tools\MSVC\1441~1.341\bin\Hostx64\x64\cl.exe  /nologo   /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /MP   /MDd /Zi /Ob0 /Od /RTC1 /showIncludes /FoCMakeFiles\cmTC_96629.dir\testCCompiler.c.obj /FdCMakeFiles\cmTC_96629.dir\ /FS -c "C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\x64-windows-dbg\CMakeFiles\CMakeScratch\TryCompile-54f34m\testCCompiler.c"
    C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\x64-windows-dbg\CMakeFiles\CMakeScratch\TryCompile-54f34m\testCCompiler.c : fatal error C1083: Cannot open compiler generated file: '': Invalid argument
    ninja: build stopped: subcommand failed.
    
    

  

  CMake will not be able to correctly generate this project.
Call Stack (most recent call first):
  CMakeLists.txt:65 (project)


-- Configuring incomplete, errors occurred!
ninja: build stopped: subcommand failed.
```
</details>

<details><summary>C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\config-x64-windows-rel-CMakeCache.txt.log</summary>

```
# This is the CMakeCache file.
# For build in directory: c:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel
# It was generated by CMake: C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//No help, variable specified on the command line.
ALSOFT_AMBDEC_PRESETS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_ALSA:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_COREAUDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_DSOUND:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_JACK:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_OPENSL:UNINITIALIZED=

//No help, variable specified on the command line.
ALSOFT_BACKEND_OSS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_PIPEWIRE:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_PORTAUDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_PULSEAUDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_SNDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_SOLARIS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_WAVE:UNINITIALIZED=ON

//No help, variable specified on the command line.
ALSOFT_BACKEND_WINMM:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_CONFIG:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_CPUEXT_NEON:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_EXAMPLES:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_HRTF_DEFS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_NO_CONFIG_UTIL:UNINITIALIZED=ON

//No help, variable specified on the command line.
ALSOFT_REQUIRE_WASAPI:UNINITIALIZED=ON

//No help, variable specified on the command line.
ALSOFT_UTILS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
BUILD_SHARED_LIBS:UNINITIALIZED=ON

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/lib.exe

//Choose the type of build, options are: None Debug Release RelWithDebInfo
// MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=Release

CMAKE_CROSSCOMPILING:STRING=OFF

//CXX compiler
CMAKE_CXX_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe

CMAKE_CXX_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /GR /EHsc /MP '

CMAKE_CXX_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

CMAKE_CXX_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//C compiler
CMAKE_C_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe

CMAKE_C_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /MP '

CMAKE_C_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/MD /O1 /Ob1 /DNDEBUG

CMAKE_C_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/MD /Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//No help, variable specified on the command line.
CMAKE_DISABLE_FIND_PACKAGE_WindowsSDK:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION:UNINITIALIZED=ON

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Enable/Disable output of compile commands during generation.
CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=

//No help, variable specified on the command line.
CMAKE_EXPORT_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY:UNINITIALIZED=ON

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel/CMakeFiles/pkgRedirects

//No help, variable specified on the command line.
CMAKE_INSTALL_BINDIR:STRING=bin

//No help, variable specified on the command line.
CMAKE_INSTALL_LIBDIR:STRING=lib

//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/pkgs/openal-soft_x64-windows

//No help, variable specified on the command line.
CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP:UNINITIALIZED=TRUE

//Path to a program.
CMAKE_LINKER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/link.exe

//make program
CMAKE_MAKE_PROGRAM:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe

//Flags used by the linker during the creation of modules during
// all build types.
...
Skipped 43 lines
...

//Flags for Windows Resource Compiler during RELWITHDEBINFO builds.
CMAKE_RC_FLAGS_RELWITHDEBINFO:STRING=

//Flags used by the linker during the creation of shared libraries
// during all build types.
CMAKE_SHARED_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of shared libraries
// during DEBUG builds.
CMAKE_SHARED_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during the creation of shared libraries
// during MINSIZEREL builds.
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during the creation of shared libraries
// during RELWITHDEBINFO builds.
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//If set, runtime paths are not added when installing shared libraries,
// but are added when building.
CMAKE_SKIP_INSTALL_RPATH:BOOL=NO

//If set, runtime paths are not added when using shared libraries.
CMAKE_SKIP_RPATH:BOOL=NO

//Flags used by the linker during the creation of static libraries
// during all build types.
CMAKE_STATIC_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of static libraries
// during DEBUG builds.
CMAKE_STATIC_LINKER_FLAGS_DEBUG:STRING=/nologo

//Flags used by the linker during the creation of static libraries
// during MINSIZEREL builds.
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL:STRING=

//Flags used by the linker during the creation of static libraries
// during RELEASE builds.
CMAKE_STATIC_LINKER_FLAGS_RELEASE:STRING=/nologo

//Flags used by the linker during the creation of static libraries
// during RELWITHDEBINFO builds.
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO:STRING=

CMAKE_SYSTEM_NAME:STRING=Windows

CMAKE_SYSTEM_PROCESSOR:STRING=AMD64

CMAKE_SYSTEM_VERSION:STRING=10.0.22631

//The CMake toolchain file
CMAKE_TOOLCHAIN_FILE:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake

//If this value is on, makefiles will be generated without the
// .SILENT directive, and all commands will be echoed to the console
// during the make.  This is useful for debugging only. With Visual
// Studio IDE projects all commands are done without /nologo.
CMAKE_VERBOSE_MAKEFILE:BOOL=ON

//No help, variable specified on the command line.
FETCHCONTENT_FULLY_DISCONNECTED:UNINITIALIZED=ON

//No help, variable specified on the command line.
LIBTYPE:UNINITIALIZED=SHARED

//Value Computed by CMake
OpenAL_BINARY_DIR:STATIC=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel

//Value Computed by CMake
OpenAL_IS_TOP_LEVEL:STATIC=ON

//Value Computed by CMake
OpenAL_SOURCE_DIR:STATIC=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean

//Automatically copy dependencies into the output directory for
// executables.
VCPKG_APPLOCAL_DEPS:BOOL=OFF

//No help, variable specified on the command line.
VCPKG_CHAINLOAD_TOOLCHAIN_FILE:UNINITIALIZED=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake

//No help, variable specified on the command line.
VCPKG_CRT_LINKAGE:UNINITIALIZED=dynamic

//No help, variable specified on the command line.
VCPKG_CXX_FLAGS:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_CXX_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_CXX_FLAGS_RELEASE:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_C_FLAGS:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_C_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_C_FLAGS_RELEASE:UNINITIALIZED=

//The directory which contains the installed libraries for each
// triplet
VCPKG_INSTALLED_DIR:PATH=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS_RELEASE:UNINITIALIZED=

//The path to the vcpkg manifest directory.
VCPKG_MANIFEST_DIR:PATH=

//Use manifest mode, as opposed to classic mode.
VCPKG_MANIFEST_MODE:BOOL=OFF

//No help, variable specified on the command line.
VCPKG_PLATFORM_TOOLSET:UNINITIALIZED=v143

//Appends the vcpkg paths to CMAKE_PREFIX_PATH, CMAKE_LIBRARY_PATH
// and CMAKE_FIND_ROOT_PATH so that vcpkg libraries/packages are
// found after toolchain/system libraries/packages.
VCPKG_PREFER_SYSTEM_LIBS:BOOL=OFF

//Enable the setup of CMAKE_PROGRAM_PATH to vcpkg paths
VCPKG_SETUP_CMAKE_PROGRAM_PATH:BOOL=ON

//No help, variable specified on the command line.
VCPKG_SET_CHARSET_FLAG:UNINITIALIZED=ON

//No help, variable specified on the command line.
VCPKG_TARGET_ARCHITECTURE:UNINITIALIZED=x64

//Vcpkg target triplet (ex. x86-windows)
VCPKG_TARGET_TRIPLET:STRING=x64-windows

//Trace calls to find_package()
VCPKG_TRACE_FIND_PACKAGE:BOOL=OFF

//Enables messages from the VCPKG toolchain for debugging purposes.
VCPKG_VERBOSE:BOOL=OFF

//(experimental) Automatically copy dependencies into the install
// target directory for executables. Requires CMake 3.14.
X_VCPKG_APPLOCAL_DEPS_INSTALL:BOOL=OFF

//(experimental) Add USES_TERMINAL to VCPKG_APPLOCAL_DEPS to force
// serialization.
X_VCPKG_APPLOCAL_DEPS_SERIALIZED:BOOL=OFF

//The directory which contains the installed libraries for each
// triplet
_VCPKG_INSTALLED_DIR:PATH=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed

//No help, variable specified on the command line.
_VCPKG_ROOT_DIR:UNINITIALIZED=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=c:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-rel
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=3
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=29
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=5
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_CXX_COMPILER
CMAKE_CXX_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_COMPILER
CMAKE_C_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_COMPILE_COMMANDS
CMAKE_EXPORT_COMPILE_COMMANDS-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Ninja
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_TOOLCHAIN_FILE
CMAKE_TOOLCHAIN_FILE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1
//Install the dependencies listed in your manifest:
//\n    If this is off, you will have to manually install your dependencies.
//\n    See https://github.com/microsoft/vcpkg/tree/master/docs/specifications/manifests.md
// for more info.
//\n
VCPKG_MANIFEST_INSTALL:INTERNAL=OFF
//ADVANCED property for variable: VCPKG_VERBOSE
VCPKG_VERBOSE-ADVANCED:INTERNAL=1
//Making sure VCPKG_MANIFEST_MODE doesn't change
Z_VCPKG_CHECK_MANIFEST_MODE:INTERNAL=OFF
//Vcpkg root directory
Z_VCPKG_ROOT_DIR:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg

```
</details>

<details><summary>C:\Users\aryma\Documents\Dataingeniør\2. År\Vår\IDATT2104 - Nettverksprogrammering\NettverkProsjekt\Kode\netcode-demo\vcpkg_installed\vcpkg\blds\openal-soft\config-x64-windows-dbg-CMakeCache.txt.log</summary>

```
# This is the CMakeCache file.
# For build in directory: c:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg
# It was generated by CMake: C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
# You can edit this file to change values found and used by cmake.
# If you do not want to change any of the values, simply exit the editor.
# If you do want to change a value, simply edit, save, and exit the editor.
# The syntax for the file is as follows:
# KEY:TYPE=VALUE
# KEY is the name of a variable in the cache.
# TYPE is a hint to GUIs for the type of VALUE, DO NOT EDIT TYPE!.
# VALUE is the current value for the KEY.

########################
# EXTERNAL cache entries
########################

//No help, variable specified on the command line.
ALSOFT_AMBDEC_PRESETS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_ALSA:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_COREAUDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_DSOUND:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_JACK:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_OPENSL:UNINITIALIZED=

//No help, variable specified on the command line.
ALSOFT_BACKEND_OSS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_PIPEWIRE:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_PORTAUDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_PULSEAUDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_SNDIO:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_SOLARIS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_BACKEND_WAVE:UNINITIALIZED=ON

//No help, variable specified on the command line.
ALSOFT_BACKEND_WINMM:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_CONFIG:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_CPUEXT_NEON:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_EXAMPLES:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_HRTF_DEFS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
ALSOFT_NO_CONFIG_UTIL:UNINITIALIZED=ON

//No help, variable specified on the command line.
ALSOFT_REQUIRE_WASAPI:UNINITIALIZED=ON

//No help, variable specified on the command line.
ALSOFT_UTILS:UNINITIALIZED=OFF

//No help, variable specified on the command line.
BUILD_SHARED_LIBS:UNINITIALIZED=ON

//Path to a program.
CMAKE_AR:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/lib.exe

//Choose the type of build, options are: None Debug Release RelWithDebInfo
// MinSizeRel ...
CMAKE_BUILD_TYPE:STRING=Debug

CMAKE_CROSSCOMPILING:STRING=OFF

//CXX compiler
CMAKE_CXX_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe

CMAKE_CXX_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /GR /EHsc /MP '

CMAKE_CXX_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

CMAKE_CXX_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//C compiler
CMAKE_C_COMPILER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/cl.exe

CMAKE_C_FLAGS:STRING=' /nologo /DWIN32 /D_WINDOWS /W3 /utf-8 /MP '

CMAKE_C_FLAGS_DEBUG:STRING='/D_DEBUG /MDd /Z7 /Ob0 /Od /RTC1 '

//Flags used by the C compiler during MINSIZEREL builds.
CMAKE_C_FLAGS_MINSIZEREL:STRING=/MD /O1 /Ob1 /DNDEBUG

CMAKE_C_FLAGS_RELEASE:STRING='/MD /O2 /Oi /Gy /DNDEBUG /Z7 '

//Flags used by the C compiler during RELWITHDEBINFO builds.
CMAKE_C_FLAGS_RELWITHDEBINFO:STRING=/MD /Zi /O2 /Ob1 /DNDEBUG

//Libraries linked by default with all C applications.
CMAKE_C_STANDARD_LIBRARIES:STRING=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib

//No help, variable specified on the command line.
CMAKE_DISABLE_FIND_PACKAGE_WindowsSDK:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION:UNINITIALIZED=ON

//Flags used by the linker during all build types.
CMAKE_EXE_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during DEBUG builds.
CMAKE_EXE_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during MINSIZEREL builds.
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_EXE_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during RELWITHDEBINFO builds.
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//Enable/Disable output of compile commands during generation.
CMAKE_EXPORT_COMPILE_COMMANDS:BOOL=

//No help, variable specified on the command line.
CMAKE_EXPORT_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY:UNINITIALIZED=ON

//No help, variable specified on the command line.
CMAKE_FIND_PACKAGE_NO_SYSTEM_PACKAGE_REGISTRY:UNINITIALIZED=ON

//Value Computed by CMake.
CMAKE_FIND_PACKAGE_REDIRECTS_DIR:STATIC=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg/CMakeFiles/pkgRedirects

//No help, variable specified on the command line.
CMAKE_INSTALL_BINDIR:STRING=bin

//No help, variable specified on the command line.
CMAKE_INSTALL_LIBDIR:STRING=lib

//Install path prefix, prepended onto install directories.
CMAKE_INSTALL_PREFIX:PATH=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/pkgs/openal-soft_x64-windows/debug

//No help, variable specified on the command line.
CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP:UNINITIALIZED=TRUE

//Path to a program.
CMAKE_LINKER:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/bin/Hostx64/x64/link.exe

//make program
CMAKE_MAKE_PROGRAM:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe

//Flags used by the linker during the creation of modules during
// all build types.
...
Skipped 45 lines
...
CMAKE_RC_FLAGS_RELWITHDEBINFO:STRING=

//Flags used by the linker during the creation of shared libraries
// during all build types.
CMAKE_SHARED_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of shared libraries
// during DEBUG builds.
CMAKE_SHARED_LINKER_FLAGS_DEBUG:STRING=/nologo    /debug /INCREMENTAL

//Flags used by the linker during the creation of shared libraries
// during MINSIZEREL builds.
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL:STRING=/INCREMENTAL:NO

CMAKE_SHARED_LINKER_FLAGS_RELEASE:STRING='/nologo /DEBUG /INCREMENTAL:NO /OPT:REF /OPT:ICF  '

//Flags used by the linker during the creation of shared libraries
// during RELWITHDEBINFO builds.
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO:STRING=/debug /INCREMENTAL

//If set, runtime paths are not added when installing shared libraries,
// but are added when building.
CMAKE_SKIP_INSTALL_RPATH:BOOL=NO

//If set, runtime paths are not added when using shared libraries.
CMAKE_SKIP_RPATH:BOOL=NO

//Flags used by the linker during the creation of static libraries
// during all build types.
CMAKE_STATIC_LINKER_FLAGS:STRING=/machine:x64

//Flags used by the linker during the creation of static libraries
// during DEBUG builds.
CMAKE_STATIC_LINKER_FLAGS_DEBUG:STRING=/nologo

//Flags used by the linker during the creation of static libraries
// during MINSIZEREL builds.
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL:STRING=

//Flags used by the linker during the creation of static libraries
// during RELEASE builds.
CMAKE_STATIC_LINKER_FLAGS_RELEASE:STRING=/nologo

//Flags used by the linker during the creation of static libraries
// during RELWITHDEBINFO builds.
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO:STRING=

CMAKE_SYSTEM_NAME:STRING=Windows

CMAKE_SYSTEM_PROCESSOR:STRING=AMD64

CMAKE_SYSTEM_VERSION:STRING=10.0.22631

//The CMake toolchain file
CMAKE_TOOLCHAIN_FILE:FILEPATH=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake

//If this value is on, makefiles will be generated without the
// .SILENT directive, and all commands will be echoed to the console
// during the make.  This is useful for debugging only. With Visual
// Studio IDE projects all commands are done without /nologo.
CMAKE_VERBOSE_MAKEFILE:BOOL=ON

//No help, variable specified on the command line.
FETCHCONTENT_FULLY_DISCONNECTED:UNINITIALIZED=ON

//No help, variable specified on the command line.
LIBTYPE:UNINITIALIZED=SHARED

//Value Computed by CMake
OpenAL_BINARY_DIR:STATIC=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg

//Value Computed by CMake
OpenAL_IS_TOP_LEVEL:STATIC=ON

//Value Computed by CMake
OpenAL_SOURCE_DIR:STATIC=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean

//Automatically copy dependencies into the output directory for
// executables.
VCPKG_APPLOCAL_DEPS:BOOL=OFF

//No help, variable specified on the command line.
VCPKG_CHAINLOAD_TOOLCHAIN_FILE:UNINITIALIZED=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/toolchains/windows.cmake

//No help, variable specified on the command line.
VCPKG_CRT_LINKAGE:UNINITIALIZED=dynamic

//No help, variable specified on the command line.
VCPKG_CXX_FLAGS:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_CXX_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_CXX_FLAGS_RELEASE:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_C_FLAGS:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_C_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_C_FLAGS_RELEASE:UNINITIALIZED=

//The directory which contains the installed libraries for each
// triplet
VCPKG_INSTALLED_DIR:PATH=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS_DEBUG:UNINITIALIZED=

//No help, variable specified on the command line.
VCPKG_LINKER_FLAGS_RELEASE:UNINITIALIZED=

//The path to the vcpkg manifest directory.
VCPKG_MANIFEST_DIR:PATH=

//Use manifest mode, as opposed to classic mode.
VCPKG_MANIFEST_MODE:BOOL=OFF

//No help, variable specified on the command line.
VCPKG_PLATFORM_TOOLSET:UNINITIALIZED=v143

//Appends the vcpkg paths to CMAKE_PREFIX_PATH, CMAKE_LIBRARY_PATH
// and CMAKE_FIND_ROOT_PATH so that vcpkg libraries/packages are
// found after toolchain/system libraries/packages.
VCPKG_PREFER_SYSTEM_LIBS:BOOL=OFF

//Enable the setup of CMAKE_PROGRAM_PATH to vcpkg paths
VCPKG_SETUP_CMAKE_PROGRAM_PATH:BOOL=ON

//No help, variable specified on the command line.
VCPKG_SET_CHARSET_FLAG:UNINITIALIZED=ON

//No help, variable specified on the command line.
VCPKG_TARGET_ARCHITECTURE:UNINITIALIZED=x64

//Vcpkg target triplet (ex. x86-windows)
VCPKG_TARGET_TRIPLET:STRING=x64-windows

//Trace calls to find_package()
VCPKG_TRACE_FIND_PACKAGE:BOOL=OFF

//Enables messages from the VCPKG toolchain for debugging purposes.
VCPKG_VERBOSE:BOOL=OFF

//(experimental) Automatically copy dependencies into the install
// target directory for executables. Requires CMake 3.14.
X_VCPKG_APPLOCAL_DEPS_INSTALL:BOOL=OFF

//(experimental) Add USES_TERMINAL to VCPKG_APPLOCAL_DEPS to force
// serialization.
X_VCPKG_APPLOCAL_DEPS_SERIALIZED:BOOL=OFF

//The directory which contains the installed libraries for each
// triplet
_VCPKG_INSTALLED_DIR:PATH=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed

//No help, variable specified on the command line.
_VCPKG_ROOT_DIR:UNINITIALIZED=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg


########################
# INTERNAL cache entries
########################

//ADVANCED property for variable: CMAKE_AR
CMAKE_AR-ADVANCED:INTERNAL=1
//This is the directory where this CMakeCache.txt was created
CMAKE_CACHEFILE_DIR:INTERNAL=c:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/x64-windows-dbg
//Major version of cmake used to create the current loaded cache
CMAKE_CACHE_MAJOR_VERSION:INTERNAL=3
//Minor version of cmake used to create the current loaded cache
CMAKE_CACHE_MINOR_VERSION:INTERNAL=29
//Patch version of cmake used to create the current loaded cache
CMAKE_CACHE_PATCH_VERSION:INTERNAL=5
//Path to CMake executable.
CMAKE_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe
//Path to cpack program executable.
CMAKE_CPACK_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cpack.exe
//Path to ctest program executable.
CMAKE_CTEST_COMMAND:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe
//ADVANCED property for variable: CMAKE_CXX_COMPILER
CMAKE_CXX_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_COMPILER
CMAKE_C_COMPILER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS
CMAKE_C_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_MINSIZEREL
CMAKE_C_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELEASE
CMAKE_C_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_FLAGS_RELWITHDEBINFO
CMAKE_C_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_C_STANDARD_LIBRARIES
CMAKE_C_STANDARD_LIBRARIES-ADVANCED:INTERNAL=1
//Executable file format
CMAKE_EXECUTABLE_FORMAT:INTERNAL=Unknown
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS
CMAKE_EXE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_DEBUG
CMAKE_EXE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_MINSIZEREL
CMAKE_EXE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELEASE
CMAKE_EXE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_EXPORT_COMPILE_COMMANDS
CMAKE_EXPORT_COMPILE_COMMANDS-ADVANCED:INTERNAL=1
//Name of external makefile project generator.
CMAKE_EXTRA_GENERATOR:INTERNAL=
//Name of generator.
CMAKE_GENERATOR:INTERNAL=Ninja
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=
//Name of generator platform.
CMAKE_GENERATOR_PLATFORM:INTERNAL=
//Name of generator toolset.
CMAKE_GENERATOR_TOOLSET:INTERNAL=
//Source directory with the top level CMakeLists.txt file for this
// project
CMAKE_HOME_DIRECTORY:INTERNAL=C:/Users/aryma/Documents/Dataingeniør/2. År/Vår/IDATT2104 - Nettverksprogrammering/NettverkProsjekt/Kode/netcode-demo/vcpkg_installed/vcpkg/blds/openal-soft/src/1.23.1-180897dbaa.clean
//ADVANCED property for variable: CMAKE_LINKER
CMAKE_LINKER-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS
CMAKE_MODULE_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_DEBUG
CMAKE_MODULE_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL
CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELEASE
CMAKE_MODULE_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_MT
CMAKE_MT-ADVANCED:INTERNAL=1
//number of local generators
CMAKE_NUMBER_OF_MAKEFILES:INTERNAL=1
//Platform information initialized
CMAKE_PLATFORM_INFO_INITIALIZED:INTERNAL=1
//noop for ranlib
CMAKE_RANLIB:INTERNAL=:
//ADVANCED property for variable: CMAKE_RC_COMPILER
CMAKE_RC_COMPILER-ADVANCED:INTERNAL=1
CMAKE_RC_COMPILER_WORKS:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS
CMAKE_RC_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_DEBUG
CMAKE_RC_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_MINSIZEREL
CMAKE_RC_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELEASE
CMAKE_RC_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_RC_FLAGS_RELWITHDEBINFO
CMAKE_RC_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//Path to CMake installation.
CMAKE_ROOT:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/share/cmake-3.29
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS
CMAKE_SHARED_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_DEBUG
CMAKE_SHARED_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL
CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELEASE
CMAKE_SHARED_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_INSTALL_RPATH
CMAKE_SKIP_INSTALL_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_SKIP_RPATH
CMAKE_SKIP_RPATH-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS
CMAKE_STATIC_LINKER_FLAGS-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_DEBUG
CMAKE_STATIC_LINKER_FLAGS_DEBUG-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL
CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELEASE
CMAKE_STATIC_LINKER_FLAGS_RELEASE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO
CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_TOOLCHAIN_FILE
CMAKE_TOOLCHAIN_FILE-ADVANCED:INTERNAL=1
//ADVANCED property for variable: CMAKE_VERBOSE_MAKEFILE
CMAKE_VERBOSE_MAKEFILE-ADVANCED:INTERNAL=1
//Install the dependencies listed in your manifest:
//\n    If this is off, you will have to manually install your dependencies.
//\n    See https://github.com/microsoft/vcpkg/tree/master/docs/specifications/manifests.md
// for more info.
//\n
VCPKG_MANIFEST_INSTALL:INTERNAL=OFF
//ADVANCED property for variable: VCPKG_VERBOSE
VCPKG_VERBOSE-ADVANCED:INTERNAL=1
//Making sure VCPKG_MANIFEST_MODE doesn't change
Z_VCPKG_CHECK_MANIFEST_MODE:INTERNAL=OFF
//Vcpkg root directory
Z_VCPKG_ROOT_DIR:INTERNAL=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg

```
</details>

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "dependencies": [
    "sfml"
  ]
}

```
</details>
