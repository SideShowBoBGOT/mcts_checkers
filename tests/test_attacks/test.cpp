#include <gtest/gtest.h>

#include <mcts_checkers_test/mcts_checkers_test.hpp>
#include <mcts_checkers/action_collection_funcs.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>
#include <range/v3/algorithm/find_if.hpp>

using NodeList = mcts_checkers::action_collection::attack::NodeList<std::allocator>;
using Node = mcts_checkers::action_collection::attack::Node<std::allocator>;
using Output = mcts_checkers::action_collection::attack::Output<std::allocator>;

void validate_unique_nodes_per_single_level(const NodeList& actions) {

    for(auto i = std::begin(actions); i != std::end(actions); ++i) {
        for(auto j = std::next(i); j != std::end(actions); ++j) {
            ASSERT_NE(i->m_board_index, j->m_board_index);
        }
    }
}

void validate_actions_equal(const NodeList& first, const NodeList& second) {
    ASSERT_EQ(first.size(), second.size());
    validate_unique_nodes_per_single_level(first);
    validate_unique_nodes_per_single_level(second);
    for(const auto& first_action : first) {
        const auto second_it = ranges::find_if(second, [&first_action](const mcts_checkers::action_collection::attack::Node<std::allocator>& second_action) {
            return first_action.m_board_index == second_action.m_board_index;
        });
        const auto ne = second_it != std::end(second);
        if(ne) {
            validate_actions_equal(first_action.m_child_trees, second_it->m_child_trees);
        } else {
            ASSERT_TRUE(false);
        }
    }
}

struct TestData {
    mcts_checkers::BoardVector checker_vector;
    mcts_checkers::CheckersData board;
    Output result;
};

namespace nlohmann {

    template<>
    struct adl_serializer<Node> {
        static Node from_json(const json& j) {
            const auto board_index = j["board_index"].get<mcts_checkers::BoardIndex>();
            auto actions = NodeList();
            for(const auto& ch : j["children"]) {
                actions.emplace_back(from_json(ch));
            }
            return {board_index, mcts_checkers::utils::checked_move(actions)};
        }
    };

    template<>
    struct adl_serializer<Output> {
        static Output from_json(const json& j) {
            const auto depth = j["depth"].get<uint8_t>();
            const auto checker_index = j["checker_index"].get<mcts_checkers::CheckerIndex>();
            auto actions = NodeList{};
            for(const auto& ch : j["actions"]) {
                actions.emplace_back(ch.get<Node>());
            }
            return {
                mcts_checkers::utils::checked_move(actions),
                checker_index,
                depth
            };
        }
    };

    template<>
    struct adl_serializer<TestData> {
        static TestData from_json(const json& j) {
            return {
                j["checker_vector"].get<mcts_checkers::BoardVector>(),
                j["board"].get<mcts_checkers::CheckersData>(),
                j["output"].get<Output>()
            };
        }
    };
}

TEST(TestAttacks, Sanity) {
    const auto file_path = std::filesystem::path(__FILE__).parent_path().append("data.json");
    auto file_stream = std::ifstream(file_path);
    auto json_file = nlohmann::json{};
    file_stream >> json_file;
    for(const auto& json_test_data : json_file) {
        const auto test_data = json_test_data.get<TestData>();
        const auto result = mcts_checkers::action_collection::attack::collect(
            test_data.board,
            mcts_checkers::convert_board_vector_to_checker_index(test_data.checker_vector),
            std::allocator<Node>()
        );
        ASSERT_EQ(result.depth, test_data.result.depth);
        ASSERT_EQ(result.m_checker_index, test_data.result.m_checker_index);
        validate_actions_equal(result.actions, test_data.result.actions);
    }
}
