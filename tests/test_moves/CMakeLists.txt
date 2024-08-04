add_executable(test_moves test.cpp)
target_link_libraries(test_moves mcts_checkers_test GTest::gtest GTest::gtest_main)
gtest_discover_tests(test_moves)