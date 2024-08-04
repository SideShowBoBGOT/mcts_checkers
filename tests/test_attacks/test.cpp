#include <gtest/gtest.h>

#include <mcts_checkers_test/mcts_checkers_test.hpp>

#include <filesystem>
#include <fstream>
#include <string_view>
#include <range/v3/algorithm/find_if.hpp>

void validate_unique_nodes_per_single_level(const std::vector<mcts_checkers::AttackAction>& actions) {
    for(size_t i = 0; i < actions.size(); ++i) {
        for(size_t j = i + 1; j < actions.size(); ++j) {
            EXPECT_NE(actions[i].m_board_index, actions[j].m_board_index);
        }
    }
}

void validate_actions_equal(
    const std::vector<mcts_checkers::AttackAction>& first,
    const std::vector<mcts_checkers::AttackAction>& second
) {
    EXPECT_EQ(first.size(), second.size());
    validate_unique_nodes_per_single_level(first);
    validate_unique_nodes_per_single_level(second);
    for(const auto& first_action : first) {
        const auto second_it = ranges::find_if(second, [&first_action](const mcts_checkers::AttackAction& second_action) {
            return first_action.m_board_index == second_action.m_board_index;
        });
        const auto ne = second_it != std::end(second);
        if(ne) {
            validate_actions_equal(first_action.m_child_actions, second_it->m_child_actions);
        } else {
            EXPECT_TRUE(false);
        }
    }
}


struct TestData {
    mcts_checkers::Vector<uint8_t> checker_vector;
    mcts_checkers::CheckersData board;
    std::vector<mcts_checkers::AttackAction> result;
};

namespace nlohmann {

    template<>
    struct adl_serializer<mcts_checkers::CheckersData> {
        static void from_json(const json& j, mcts_checkers::CheckersData& data) {
            for(uint8_t j_index = 1, y = 0; y < mcts_checkers::CELLS_PER_SIDE; ++j_index, ++y) {
                auto row = j[j_index].get<std::string_view>();
                const auto is_even = mcts_checkers::is_even(y);
                for(
                    uint8_t row_index = is_even ? 2 : 1,
                    x = is_even ? 1 : 0;
                    x < mcts_checkers::CELLS_PER_SIDE;
                    row_index += 2, x += 2
                ) {
                    const auto checker_index = mcts_checkers::convert_board_vector_to_checker_index({x, y});
                    const auto ch = row[row_index];
                    switch(ch) {
                        case 'o': {
                            data.m_is_in_place[checker_index] = true;
                            data.m_is_king[checker_index] = false;
                            data.m_player_index[checker_index] = false;
                            break;
                        }
                        case 'O': {
                            data.m_is_in_place[checker_index] = true;
                            data.m_is_king[checker_index] = true;
                            data.m_player_index[checker_index] = false;
                            break;
                        }
                        case 'x': {
                            data.m_is_in_place[checker_index] = true;
                            data.m_is_king[checker_index] = false;
                            data.m_player_index[checker_index] = true;
                            break;
                        }
                        default: {
                            data.m_is_in_place[checker_index] = false;
                            data.m_is_king[checker_index] = false;
                            data.m_player_index[checker_index] = false;
                            break;
                        }
                    }
                }
            }
        }
    };
    template<>
    struct adl_serializer<mcts_checkers::AttackAction> {
        static void from_json(const json& j, ::mcts_checkers::AttackAction& data) {
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
            data.result = j["result"].get<std::vector<mcts_checkers::AttackAction>>();
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
        const auto result = mcts_checkers::collect_attacks(test_data.board, test_data.checker_vector);
        validate_actions_equal(result.first, test_data.result);
    }
}
