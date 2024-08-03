#include <gtest/gtest.h>
#include <mcts_checkers/checkers_data.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

struct TestData {
    uint8_t board_index{};
    uint8_t checker_index{};
    mcts_checkers::Vector<uint8_t> board_vector{};
};

namespace nlohmann {
    template<>
        struct adl_serializer<::mcts_checkers::Vector<uint8_t>> {
        static void from_json(const json& j, mcts_checkers::Vector<uint8_t>& data) {
            const auto arr = j.get<std::array<uint8_t, 2>>();
            data.x = arr[0];
            data.y = arr[1];
        }
    };

    template<>
    struct adl_serializer<TestData> {
        static void from_json(const json& j, TestData& data) {
            data.board_vector = j["board_vector"].get<mcts_checkers::Vector<uint8_t>>();
            data.board_index = j["board_index"].get<uint8_t>();
            data.checker_index = j["checker_index"].get<uint8_t>();
        }
    };
}
namespace mcts_checkers {
    template<typename T>
    bool operator==(const mcts_checkers::Vector<T>& first, const mcts_checkers::Vector<T> second) {
        return first.x == second.x and first.y == second.y;
    }
}


TEST(IndexConversions, Sanity) {
    const auto file_path = std::filesystem::path(__FILE__).parent_path().append("data.json");
    auto file_stream = std::ifstream(file_path);
    auto json_file = nlohmann::json{};
    file_stream >> json_file;
    for(const auto& json_test_data : json_file) {
        const auto test_data = json_test_data.get<TestData>();
        EXPECT_EQ(test_data.checker_index, mcts_checkers::convert_board_vector_to_checker_index(test_data.board_vector));
        EXPECT_EQ(test_data.checker_index, mcts_checkers::convert_board_index_to_checker_index(test_data.board_index));

        EXPECT_EQ(test_data.board_vector, mcts_checkers::convert_board_index_to_board_vector(test_data.board_index));
        EXPECT_EQ(test_data.board_vector, mcts_checkers::convert_checker_index_to_board_vector(test_data.checker_index));

        EXPECT_EQ(test_data.board_index, mcts_checkers::convert_board_vector_to_board_index(test_data.board_vector));
        EXPECT_EQ(test_data.board_index, mcts_checkers::convert_checker_index_to_board_index(test_data.checker_index));
    }
}