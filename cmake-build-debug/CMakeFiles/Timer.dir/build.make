# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/greg/Study/CLionProjects/Timer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/greg/Study/CLionProjects/Timer/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Timer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Timer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Timer.dir/flags.make

CMakeFiles/Timer.dir/main.cpp.o: CMakeFiles/Timer.dir/flags.make
CMakeFiles/Timer.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/greg/Study/CLionProjects/Timer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Timer.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Timer.dir/main.cpp.o -c /Users/greg/Study/CLionProjects/Timer/main.cpp

CMakeFiles/Timer.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Timer.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/greg/Study/CLionProjects/Timer/main.cpp > CMakeFiles/Timer.dir/main.cpp.i

CMakeFiles/Timer.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Timer.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/greg/Study/CLionProjects/Timer/main.cpp -o CMakeFiles/Timer.dir/main.cpp.s

# Object files for target Timer
Timer_OBJECTS = \
"CMakeFiles/Timer.dir/main.cpp.o"

# External object files for target Timer
Timer_EXTERNAL_OBJECTS =

Timer: CMakeFiles/Timer.dir/main.cpp.o
Timer: CMakeFiles/Timer.dir/build.make
Timer: CMakeFiles/Timer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/greg/Study/CLionProjects/Timer/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Timer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Timer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Timer.dir/build: Timer

.PHONY : CMakeFiles/Timer.dir/build

CMakeFiles/Timer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Timer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Timer.dir/clean

CMakeFiles/Timer.dir/depend:
	cd /Users/greg/Study/CLionProjects/Timer/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/greg/Study/CLionProjects/Timer /Users/greg/Study/CLionProjects/Timer /Users/greg/Study/CLionProjects/Timer/cmake-build-debug /Users/greg/Study/CLionProjects/Timer/cmake-build-debug /Users/greg/Study/CLionProjects/Timer/cmake-build-debug/CMakeFiles/Timer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Timer.dir/depend
