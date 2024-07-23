# Install script for directory: /home/sideshowbobgot/university/mcts_checkers/external/imgui-sfml

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/sideshowbobgot/university/mcts_checkers/cmake-build-debug/external/imgui-sfml/libImGui-SFML_d.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so"
         OLD_RPATH "/home/sideshowbobgot/university/mcts_checkers/cmake-build-debug/external/SFML/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libImGui-SFML_d.so")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui-sfml/imgui-SFML.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui-sfml/imgui-SFML_export.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/imconfig.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/imgui.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/imgui_internal.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/imstb_rectpack.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/imstb_textedit.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/imstb_truetype.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui/misc/cpp/imgui_stdlib.h"
    "/home/sideshowbobgot/university/mcts_checkers/external/imgui-sfml/imconfig-SFML.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ImGui-SFML/ImGui-SFMLConfig.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ImGui-SFML/ImGui-SFMLConfig.cmake"
         "/home/sideshowbobgot/university/mcts_checkers/cmake-build-debug/external/imgui-sfml/CMakeFiles/Export/761a6a4c7704629aea6d1d08969b2ac8/ImGui-SFMLConfig.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ImGui-SFML/ImGui-SFMLConfig-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ImGui-SFML/ImGui-SFMLConfig.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ImGui-SFML" TYPE FILE FILES "/home/sideshowbobgot/university/mcts_checkers/cmake-build-debug/external/imgui-sfml/CMakeFiles/Export/761a6a4c7704629aea6d1d08969b2ac8/ImGui-SFMLConfig.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ImGui-SFML" TYPE FILE FILES "/home/sideshowbobgot/university/mcts_checkers/cmake-build-debug/external/imgui-sfml/CMakeFiles/Export/761a6a4c7704629aea6d1d08969b2ac8/ImGui-SFMLConfig-debug.cmake")
  endif()
endif()

