# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out

# Include any dependencies generated for this target.
include coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/compiler_depend.make

# Include the progress variables for this target.
include coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/progress.make

# Include the compile flags for this target's objects.
include coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/flags.make

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/flags.make
coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj: /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server.cc
coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/third_party/toolchain-mac/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj -MF CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj.d -o CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj -c /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server.cc

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.i"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/third_party/toolchain-mac/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server.cc > CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.i

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.s"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/third_party/toolchain-mac/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server.cc -o CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.s

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/flags.make
coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj: /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server_handlers.cc
coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/third_party/toolchain-mac/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj -MF CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj.d -o CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj -c /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server_handlers.cc

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.i"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/third_party/toolchain-mac/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server_handlers.cc > CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.i

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.s"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/third_party/toolchain-mac/gcc-arm-none-eabi-9-2020-q2-update/bin/arm-none-eabi-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base/http_server_handlers.cc -o CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.s

# Object files for target libs_base-m7_http_server
libs_base__m7_http_server_OBJECTS = \
"CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj" \
"CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj"

# External object files for target libs_base-m7_http_server
libs_base__m7_http_server_EXTERNAL_OBJECTS = \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/app_callbacks.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/multicore.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/event_groups.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/list.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/portable/GCC/ARM_CM4F/port.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/queue.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/stream_buffer.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/tasks.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/timers.c.obj" \
"/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/FreeRTOS_helpers/heap_useNewlib_NXP.c.obj"

coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server.cc.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/http_server_handlers.cc.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/app_callbacks.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/multicore.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/event_groups.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/list.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/portable/GCC/ARM_CM4F/port.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/queue.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/stream_buffer.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/tasks.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/freertos_kernel/timers.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/FreeRTOS/CMakeFiles/libs_FreeRTOS.dir/__/__/third_party/FreeRTOS_helpers/heap_useNewlib_NXP.c.obj
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/build.make
coralmicro/libs/base/liblibs_base-m7_http_server.a: coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library liblibs_base-m7_http_server.a"
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && $(CMAKE_COMMAND) -P CMakeFiles/libs_base-m7_http_server.dir/cmake_clean_target.cmake
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libs_base-m7_http_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/build: coralmicro/libs/base/liblibs_base-m7_http_server.a
.PHONY : coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/build

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/clean:
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base && $(CMAKE_COMMAND) -P CMakeFiles/libs_base-m7_http_server.dir/cmake_clean.cmake
.PHONY : coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/clean

coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/depend:
	cd /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/coralmicro/libs/base /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base /Users/vnz/Desktop/TinyScrubCam/tinyscrubcam/googleCoral/out/coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : coralmicro/libs/base/CMakeFiles/libs_base-m7_http_server.dir/depend
