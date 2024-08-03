find_package(GTest REQUIRED CONFIG)

include(CTest)
enable_testing()
include(GoogleTest)

set(test_names
    attacks
)

foreach(test_name ${test_names})
    add_executable(test_${test_name} test_${test_name}.cpp)
    target_link_libraries(test_${test_name} mcts_checkers nlohmann_json::nlohmann_json GTest::gtest GTest::gtest_main)
    gtest_discover_tests(test_${test_name})
endforeach()

add_subdirectory(test_index_conversions)