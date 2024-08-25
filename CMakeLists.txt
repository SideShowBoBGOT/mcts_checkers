cmake_minimum_required(VERSION 3.15)
project(mcts_checkers)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external)

find_package(Boost REQUIRED COMPONENTS system)
find_package(fmt REQUIRED)

add_library(mcts_checkers
        src/mcts_checkers/utils.hpp
        src/mcts_checkers/statistic_form.hpp
        src/mcts_checkers/statistic_form.cpp
        src/mcts_checkers/main_form.hpp
        src/mcts_checkers/main_form.cpp
        src/mcts_checkers/index_converters.hpp
        src/mcts_checkers/checkers_data.hpp
        src/mcts_checkers/checkers_data.cpp

#         src/mcts_checkers/board_form.hpp
#         src/mcts_checkers/board_form.cpp
        
        src/mcts_checkers/action_collection_funcs.hpp
        src/mcts_checkers/action_application_funcs.hpp

        src/mcts_checkers/board/utils.hpp
        src/mcts_checkers/board/utils.cpp
        src/mcts_checkers/board/form.hpp
        src/mcts_checkers/board/form.cpp

        src/mcts_checkers/board/human_form.hpp
        src/mcts_checkers/board/human_form.cpp
        src/mcts_checkers/board/ai_form.hpp
        src/mcts_checkers/board/ai_form.cpp
        src/mcts_checkers/allocators.hpp
)
target_include_directories(mcts_checkers PUBLIC src)
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

