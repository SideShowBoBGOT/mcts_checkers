cmake_minimum_required(VERSION 3.15)
project(mcts_checkers)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

find_package(Boost REQUIRED COMPONENTS system)
find_package(fmt REQUIRED)

add_library(mcts_checkers
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
        include/mcts_checkers/checkers_types.hpp
        include/mcts_checkers/checkers_funcs.hpp
        src/checkers_funcs.cpp
        include/mcts_checkers/action_application_funcs.hpp
        src/action_application_funcs.cpp
        include/mcts_checkers/index_converters.hpp
)
target_include_directories(mcts_checkers PUBLIC include)
target_link_libraries(mcts_checkers
        PUBLIC
        ImGui-SFML::ImGui-SFML
        tl::expected
        tl::optional
        boost_system
        range-v3::range-v3
        fmt::fmt
        strong_type::strong_type
)

target_compile_options(mcts_checkers PRIVATE -Wall -Wextra)
#target_compile_options(mcts_checkers PRIVATE -Werror)

add_executable(main_mcts_checkers main.cpp)
target_link_libraries(main_mcts_checkers PUBLIC mcts_checkers)
target_compile_options(main_mcts_checkers PRIVATE -Wall -Wextra)

add_subdirectory(tests)

