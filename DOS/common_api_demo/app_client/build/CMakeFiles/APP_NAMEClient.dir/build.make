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
CMAKE_SOURCE_DIR = /home/common_api/02_demo/01_sys/test/app_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/common_api/02_demo/01_sys/test/app_client/build

# Include any dependencies generated for this target.
include CMakeFiles/APP_NAMEClient.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/APP_NAMEClient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/APP_NAMEClient.dir/flags.make

CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o: CMakeFiles/APP_NAMEClient.dir/flags.make
CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o: ../src/APP_NAMEClient.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/common_api/02_demo/01_sys/test/app_client/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64  $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o -c /home/common_api/02_demo/01_sys/test/app_client/src/APP_NAMEClient.cpp

CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.i"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -E /home/common_api/02_demo/01_sys/test/app_client/src/APP_NAMEClient.cpp > CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.i

CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.s"
	/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/i686-linux/usr/bin/x86_64-poky-linux/x86_64-poky-linux-g++   -m64 -march=core2 -mtune=core2 -msse3 -mfpmath=sse --sysroot=/home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64 $(CXX_DEFINES) $(CXX_FLAGS) -S /home/common_api/02_demo/01_sys/test/app_client/src/APP_NAMEClient.cpp -o CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.s

CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.requires:
.PHONY : CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.requires

CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.provides: CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.requires
	$(MAKE) -f CMakeFiles/APP_NAMEClient.dir/build.make CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.provides.build
.PHONY : CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.provides

CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.provides.build: CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o

# Object files for target APP_NAMEClient
APP_NAMEClient_OBJECTS = \
"CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o"

# External object files for target APP_NAMEClient
APP_NAMEClient_EXTERNAL_OBJECTS =

APP_NAMEClient: CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o
APP_NAMEClient: CMakeFiles/APP_NAMEClient.dir/build.make
APP_NAMEClient: /home/wangqi/workspace/GDP/genivi-demo-platform/gdp-src-build/tmp/sysroots/qemux86-64/usr/lib/libCommonAPI.so.3.1.3
APP_NAMEClient: /usr/local/lib/libAPP_NAME-dbus.so
APP_NAMEClient: CMakeFiles/APP_NAMEClient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable APP_NAMEClient"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/APP_NAMEClient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/APP_NAMEClient.dir/build: APP_NAMEClient
.PHONY : CMakeFiles/APP_NAMEClient.dir/build

CMakeFiles/APP_NAMEClient.dir/requires: CMakeFiles/APP_NAMEClient.dir/src/APP_NAMEClient.cpp.o.requires
.PHONY : CMakeFiles/APP_NAMEClient.dir/requires

CMakeFiles/APP_NAMEClient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/APP_NAMEClient.dir/cmake_clean.cmake
.PHONY : CMakeFiles/APP_NAMEClient.dir/clean

CMakeFiles/APP_NAMEClient.dir/depend:
	cd /home/common_api/02_demo/01_sys/test/app_client/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/common_api/02_demo/01_sys/test/app_client /home/common_api/02_demo/01_sys/test/app_client /home/common_api/02_demo/01_sys/test/app_client/build /home/common_api/02_demo/01_sys/test/app_client/build /home/common_api/02_demo/01_sys/test/app_client/build/CMakeFiles/APP_NAMEClient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/APP_NAMEClient.dir/depend

