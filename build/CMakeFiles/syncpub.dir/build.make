# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dmitry/Work_place/Work_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dmitry/Work_place/Work_project/build

# Include any dependencies generated for this target.
include CMakeFiles/syncpub.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/syncpub.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/syncpub.dir/flags.make

CMakeFiles/syncpub.dir/syncpub.cpp.o: CMakeFiles/syncpub.dir/flags.make
CMakeFiles/syncpub.dir/syncpub.cpp.o: ../syncpub.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dmitry/Work_place/Work_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/syncpub.dir/syncpub.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/syncpub.dir/syncpub.cpp.o -c /home/dmitry/Work_place/Work_project/syncpub.cpp

CMakeFiles/syncpub.dir/syncpub.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/syncpub.dir/syncpub.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dmitry/Work_place/Work_project/syncpub.cpp > CMakeFiles/syncpub.dir/syncpub.cpp.i

CMakeFiles/syncpub.dir/syncpub.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/syncpub.dir/syncpub.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dmitry/Work_place/Work_project/syncpub.cpp -o CMakeFiles/syncpub.dir/syncpub.cpp.s

# Object files for target syncpub
syncpub_OBJECTS = \
"CMakeFiles/syncpub.dir/syncpub.cpp.o"

# External object files for target syncpub
syncpub_EXTERNAL_OBJECTS =

syncpub: CMakeFiles/syncpub.dir/syncpub.cpp.o
syncpub: CMakeFiles/syncpub.dir/build.make
syncpub: /usr/local/lib/libzmq.so.5.2.5
syncpub: /usr/lib/x86_64-linux-gnu/libsodium.so
syncpub: CMakeFiles/syncpub.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dmitry/Work_place/Work_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable syncpub"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/syncpub.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/syncpub.dir/build: syncpub

.PHONY : CMakeFiles/syncpub.dir/build

CMakeFiles/syncpub.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/syncpub.dir/cmake_clean.cmake
.PHONY : CMakeFiles/syncpub.dir/clean

CMakeFiles/syncpub.dir/depend:
	cd /home/dmitry/Work_place/Work_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dmitry/Work_place/Work_project /home/dmitry/Work_place/Work_project /home/dmitry/Work_place/Work_project/build /home/dmitry/Work_place/Work_project/build /home/dmitry/Work_place/Work_project/build/CMakeFiles/syncpub.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/syncpub.dir/depend
