add_subdirectory(SFML)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" OFF)

add_subdirectory(imgui-sfml)
add_subdirectory(expected)
add_subdirectory(optional)