# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_SOURCE_DIR = /home/lawliet/workspace/cpp/game_engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lawliet/workspace/cpp/game_engine/build

# Include any dependencies generated for this target.
include Empty/CMakeFiles/Empty.dir/depend.make

# Include the progress variables for this target.
include Empty/CMakeFiles/Empty.dir/progress.make

# Include the compile flags for this target's objects.
include Empty/CMakeFiles/Empty.dir/flags.make

Empty/CMakeFiles/Empty.dir/EmptyApplication.cpp.o: Empty/CMakeFiles/Empty.dir/flags.make
Empty/CMakeFiles/Empty.dir/EmptyApplication.cpp.o: ../Empty/EmptyApplication.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lawliet/workspace/cpp/game_engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Empty/CMakeFiles/Empty.dir/EmptyApplication.cpp.o"
	cd /home/lawliet/workspace/cpp/game_engine/build/Empty && /usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Empty.dir/EmptyApplication.cpp.o -c /home/lawliet/workspace/cpp/game_engine/Empty/EmptyApplication.cpp

Empty/CMakeFiles/Empty.dir/EmptyApplication.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Empty.dir/EmptyApplication.cpp.i"
	cd /home/lawliet/workspace/cpp/game_engine/build/Empty && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lawliet/workspace/cpp/game_engine/Empty/EmptyApplication.cpp > CMakeFiles/Empty.dir/EmptyApplication.cpp.i

Empty/CMakeFiles/Empty.dir/EmptyApplication.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Empty.dir/EmptyApplication.cpp.s"
	cd /home/lawliet/workspace/cpp/game_engine/build/Empty && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lawliet/workspace/cpp/game_engine/Empty/EmptyApplication.cpp -o CMakeFiles/Empty.dir/EmptyApplication.cpp.s

# Object files for target Empty
Empty_OBJECTS = \
"CMakeFiles/Empty.dir/EmptyApplication.cpp.o"

# External object files for target Empty
Empty_EXTERNAL_OBJECTS =

Empty/Empty: Empty/CMakeFiles/Empty.dir/EmptyApplication.cpp.o
Empty/Empty: Empty/CMakeFiles/Empty.dir/build.make
Empty/Empty: Framework/Common/libCommon.a
Empty/Empty: Empty/CMakeFiles/Empty.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lawliet/workspace/cpp/game_engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Empty"
	cd /home/lawliet/workspace/cpp/game_engine/build/Empty && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Empty.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Empty/CMakeFiles/Empty.dir/build: Empty/Empty

.PHONY : Empty/CMakeFiles/Empty.dir/build

Empty/CMakeFiles/Empty.dir/clean:
	cd /home/lawliet/workspace/cpp/game_engine/build/Empty && $(CMAKE_COMMAND) -P CMakeFiles/Empty.dir/cmake_clean.cmake
.PHONY : Empty/CMakeFiles/Empty.dir/clean

Empty/CMakeFiles/Empty.dir/depend:
	cd /home/lawliet/workspace/cpp/game_engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lawliet/workspace/cpp/game_engine /home/lawliet/workspace/cpp/game_engine/Empty /home/lawliet/workspace/cpp/game_engine/build /home/lawliet/workspace/cpp/game_engine/build/Empty /home/lawliet/workspace/cpp/game_engine/build/Empty/CMakeFiles/Empty.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Empty/CMakeFiles/Empty.dir/depend

