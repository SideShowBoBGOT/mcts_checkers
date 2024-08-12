#include <mcts_checkers_test/mcts_checkers_test.hpp>
#include <mcts_checkers/index_converters.hpp>

namespace nlohmann {

    void adl_serializer<mcts_checkers::BoardVector>::from_json(const json& j, mcts_checkers::BoardVector& data) {
        const auto arr = j.get<std::array<uint8_t, 2>>();
        data.x = arr[0];
        data.y = arr[1];
    }

    void adl_serializer<mcts_checkers::CheckersData>::from_json(const json& j, mcts_checkers::CheckersData& data) {
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
}