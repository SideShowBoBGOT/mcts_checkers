cmake_minimum_required(VERSION 3.28)
project(mcts_checkers)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ImGui-SFML::ImGui-SFML
)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
