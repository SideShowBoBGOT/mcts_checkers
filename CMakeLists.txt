cmake_minimum_required(VERSION 3.28)
project(mcts_checkers)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

add_executable(${PROJECT_NAME} main.cpp
        include/mcts_checkers/utils.hpp
        include/mcts_checkers/main_form.hpp
        src/main_form.cpp
        include/mcts_checkers/board_form.hpp
        include/mcts_checkers/statistic_form.hpp
        src/board_form.cpp
        src/statistic_form.cpp
        include/mcts_checkers/checkers_data.hpp
        src/checkers_data.cpp
        include/mcts_checkers/mcts.hpp
        src/mcts.cpp
        tests/calc_max_jumps.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ImGui-SFML::ImGui-SFML
        tl::expected
        tl::optional
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
#target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

