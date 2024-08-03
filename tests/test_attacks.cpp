#include <gtest/gtest.h>

#include <mcts_checkers/checkers_data.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string_view>
#include <ranges>

struct TestData {
    mcts_checkers::Vector<uint8_t> checker_vector;
    mcts_checkers::CheckersData board;
    std::vector<mcts_checkers::AttackAction> result;
};

namespace nlohmann {
    template<>
        struct adl_serializer<::mcts_checkers::Vector<uint8_t>> {
        static void from_json(const json& j, ::mcts_checkers::Vector<uint8_t>& data) {
            const auto arr = j.get<std::array<uint8_t, 2>>();
            data.x = arr[0];
            data.y = arr[1];
        }
    };

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
                        default: break;
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
    auto file_stream = std::ifstream("../../tests/test_attacks_0.json");
    auto json_file = nlohmann::json();
    file_stream >> json_file;
    const auto test_data = json_file.get<TestData>();
    auto result = mcts_checkers::collect_king_attacks(test_data.board, test_data.checker_vector);
    std::cout << "fsdfdsf";
}
