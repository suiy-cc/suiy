# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/common_api/02_demo/01_sys/test/dbus_so

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/common_api/02_demo/01_sys/test/dbus_so/build

# Include any dependencies generated for this target.
include CMakeFiles/APP_NAME-dbus.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/APP_NAME-dbus.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/APP_NAME-dbus.dir/flags.make

CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o: CMakeFiles/APP_NAME-dbus.dir/flags.make
CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o: ../core/v0_1/APP_NAME/HelloWorldStubDefault.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/common_api/02_demo/01_sys/test/dbus_so/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64  $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o -c /home/common_api/02_demo/01_sys/test/dbus_so/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp

CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.i"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -E /home/common_api/02_demo/01_sys/test/dbus_so/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp > CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.i

CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.s"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -S /home/common_api/02_demo/01_sys/test/dbus_so/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp -o CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.s

CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.requires:
.PHONY : CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.requires

CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.provides: CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.requires
	$(MAKE) -f CMakeFiles/APP_NAME-dbus.dir/build.make CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.provides.build
.PHONY : CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.provides

CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.provides.build: CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o: CMakeFiles/APP_NAME-dbus.dir/flags.make
CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o: ../dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/common_api/02_demo/01_sys/test/dbus_so/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64  $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o -c /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.i"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -E /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp > CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.i

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.s"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -S /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp -o CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.s

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.requires:
.PHONY : CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.requires

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.provides: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.requires
	$(MAKE) -f CMakeFiles/APP_NAME-dbus.dir/build.make CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.provides.build
.PHONY : CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.provides

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.provides.build: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o: CMakeFiles/APP_NAME-dbus.dir/flags.make
CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o: ../dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/common_api/02_demo/01_sys/test/dbus_so/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64  $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o -c /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.i"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -E /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp > CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.i

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.s"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -S /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp -o CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.s

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.requires:
.PHONY : CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.requires

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.provides: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.requires
	$(MAKE) -f CMakeFiles/APP_NAME-dbus.dir/build.make CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.provides.build
.PHONY : CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.provides

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.provides.build: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o: CMakeFiles/APP_NAME-dbus.dir/flags.make
CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o: ../dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/common_api/02_demo/01_sys/test/dbus_so/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64  $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o -c /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.i"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -E /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp > CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.i

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.s"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -S /home/common_api/02_demo/01_sys/test/dbus_so/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp -o CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.s

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.requires:
.PHONY : CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.requires

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.provides: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.requires
	$(MAKE) -f CMakeFiles/APP_NAME-dbus.dir/build.make CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.provides.build
.PHONY : CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.provides

CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.provides.build: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o

# Object files for target APP_NAME-dbus
APP_NAME__dbus_OBJECTS = \
"CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o" \
"CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o" \
"CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o" \
"CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o"

# External object files for target APP_NAME-dbus
APP_NAME__dbus_EXTERNAL_OBJECTS =

libAPP_NAME-dbus.so: CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o
libAPP_NAME-dbus.so: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o
libAPP_NAME-dbus.so: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o
libAPP_NAME-dbus.so: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o
libAPP_NAME-dbus.so: CMakeFiles/APP_NAME-dbus.dir/build.make
libAPP_NAME-dbus.so: /home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64/usr/lib/libCommonAPI-DBus.so.3.1.3
libAPP_NAME-dbus.so: /home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64/usr/lib/libCommonAPI.so.3.1.3
libAPP_NAME-dbus.so: CMakeFiles/APP_NAME-dbus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library libAPP_NAME-dbus.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/APP_NAME-dbus.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/APP_NAME-dbus.dir/build: libAPP_NAME-dbus.so
.PHONY : CMakeFiles/APP_NAME-dbus.dir/build

CMakeFiles/APP_NAME-dbus.dir/requires: CMakeFiles/APP_NAME-dbus.dir/core/v0_1/APP_NAME/HelloWorldStubDefault.cpp.o.requires
CMakeFiles/APP_NAME-dbus.dir/requires: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusProxy.cpp.o.requires
CMakeFiles/APP_NAME-dbus.dir/requires: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusStubAdapter.cpp.o.requires
CMakeFiles/APP_NAME-dbus.dir/requires: CMakeFiles/APP_NAME-dbus.dir/dbus/v0_1/APP_NAME/HelloWorldDBusDeployment.cpp.o.requires
.PHONY : CMakeFiles/APP_NAME-dbus.dir/requires

CMakeFiles/APP_NAME-dbus.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/APP_NAME-dbus.dir/cmake_clean.cmake
.PHONY : CMakeFiles/APP_NAME-dbus.dir/clean

CMakeFiles/APP_NAME-dbus.dir/depend:
	cd /home/common_api/02_demo/01_sys/test/dbus_so/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/common_api/02_demo/01_sys/test/dbus_so /home/common_api/02_demo/01_sys/test/dbus_so /home/common_api/02_demo/01_sys/test/dbus_so/build /home/common_api/02_demo/01_sys/test/dbus_so/build /home/common_api/02_demo/01_sys/test/dbus_so/build/CMakeFiles/APP_NAME-dbus.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/APP_NAME-dbus.dir/depend

