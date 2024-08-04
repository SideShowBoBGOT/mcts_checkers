add_executable(test_attacks test.cpp)
target_link_libraries(test_attacks mcts_checkers_test GTest::gtest GTest::gtest_main)
gtest_discover_tests(test_attacks)