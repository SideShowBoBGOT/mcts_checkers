#include <gtest/gtest.h>

#include <mcts_checkers_test/mcts_checkers_test.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>
#include <unordered_set>

struct TestData {
    mcts_checkers::Vector<uint8_t> checker_vector;
    mcts_checkers::CheckersData board;
    std::vector<uint8_t> result;
};

namespace nlohmann {

    template<>
    struct adl_serializer<mcts_checkers::AttackAction> {
        static void from_json(const json& j, mcts_checkers::AttackAction& data) {
            data.m_board_index = j["index"].get<uint8_t>();
            for(const auto& ch : j["children"]) {
                from_json(ch, data.m_child_actions.emplace_back());
            }
        }
    };

    template<>
    struct adl_serializer<TestData> {
        static void from_json(const json& j, TestData& data) {
            data.checker_vector = j["checker_vector"].get<mcts_checkers::Vector<uint8_t>>();
            data.board = j["board"].get<mcts_checkers::CheckersData>();
            data.result = j["result"].get<std::vector<uint8_t>>();
        }
    };

}

std::unordered_set<uint8_t> validate_unique(const std::vector<uint8_t>& v) {
    auto s = std::unordered_set(std::begin(v), std::end(v));
    EXPECT_EQ(s.size(), v.size());
    return s;
}

void validate_result(const std::vector<uint8_t>& result, const std::vector<uint8_t>& expected) {
    const auto result_set = validate_unique(result);
    const auto expected_set = validate_unique(expected);
    EXPECT_EQ(result_set, expected_set);
}

TEST(TestMoves, Sanity) {
    const auto file_path = std::filesystem::path(__FILE__).parent_path().append("data.json");
    auto file_stream = std::ifstream(file_path);
    auto json_file = nlohmann::json{};
    file_stream >> json_file;
    for(const auto& json_test_data : json_file) {
        const auto test_data = json_test_data.get<TestData>();
        const auto result = collect_moves(test_data.board, test_data.checker_vector);
        validate_result(result, test_data.result);
    }
}
