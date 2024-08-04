find_package(GTest REQUIRED CONFIG)

include(CTest)
enable_testing()
include(GoogleTest)

add_library(mcts_checkers_test
        include/mcts_checkers_test/mcts_checkers_test.hpp
        src/mcts_checkers_test.cpp
)
target_include_directories(mcts_checkers_test PUBLIC include)

target_link_libraries(mcts_checkers_test PUBLIC mcts_checkers nlohmann_json::nlohmann_json)
add_subdirectory(test_index_conversions)
add_subdirectory(test_king_attacks)