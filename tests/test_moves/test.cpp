#include <gtest/gtest.h>

#include <mcts_checkers_test/mcts_checkers_test.hpp>
#include <mcts_checkers/action_collection_funcs.hpp>

#include <filesystem>
#include <fstream>
#include <unordered_set>

struct TestData {
    mcts_checkers::BoardVector checker_vector;
    mcts_checkers::CheckersData board;
    std::list<mcts_checkers::MoveAction> result;
};

namespace nlohmann {

    template<>
    struct adl_serializer<mcts_checkers::MoveAction> {
        static mcts_checkers::MoveAction from_json(const json& j) {
            return mcts_checkers::MoveAction{j.get<mcts_checkers::BoardIndex>()};
        }
    };

    template<>
    struct adl_serializer<TestData> {
        static void from_json(const json& j, TestData& data) {
            data.checker_vector = j["checker_vector"].get<mcts_checkers::BoardVector>();
            data.board = j["board"].get<mcts_checkers::CheckersData>();
            data.result = j["result"].get<std::list<mcts_checkers::MoveAction>>();
        }
    };

}

std::unordered_set<mcts_checkers::MoveAction> validate_unique(const std::list<mcts_checkers::MoveAction>& v) {
    auto s = std::unordered_set(std::begin(v), std::end(v));
    EXPECT_EQ(s.size(), v.size());
    return s;
}

void validate_result(const std::list<mcts_checkers::MoveAction>& result, const std::list<mcts_checkers::MoveAction>& expected) {
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
        const auto result = mcts_checkers::action_collection::move::collect(
            test_data.board,
            mcts_checkers::convert_board_vector_to_checker_index(test_data.checker_vector),
            std::allocator<mcts_checkers::MoveAction>()
        );
        validate_result(result.m_actions, test_data.result);
    }
}
