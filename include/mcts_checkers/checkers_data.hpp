#pragma once
#include <bitset>
#include <concepts>
#include <array>
#include <vector>

namespace mcts_checkers {
    constexpr uint8_t CELLS_PER_SIDE = 10;
    constexpr uint8_t CHECKERS_PER_ROW = CELLS_PER_SIDE / 2;
    constexpr uint8_t CHECKERS_PER_COL = CELLS_PER_SIDE;
    constexpr uint8_t BOARD_CELLS_COUNT = CELLS_PER_SIDE * CELLS_PER_SIDE;
    constexpr uint8_t CHEKCERS_CELLS_COUNT = BOARD_CELLS_COUNT / 2;

    struct CheckersData {
        CheckersData();
        std::bitset<CHEKCERS_CELLS_COUNT> m_player_index;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_in_place;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_king;
    };

    struct GameData {
        GameData();
        CheckersData data;
        bool m_current_player_index = false;
    };

    struct AttackAction {
        uint8_t m_cell_index{};
        std::vector<AttackAction> m_child_actions{};
    };

    struct MoveAction {
        std::vector<uint8_t> m_cell_indexes;
    };

    template<typename T>
    struct Vector {
        T x{};
        T y{};
    };

    template<std::integral T>
    static constexpr bool is_even(const T value) {
        return value % 2 == 0;
    }

    static constexpr bool is_coord_not_valid(const int8_t coord) {
        return coord < 0 or coord >= static_cast<int8_t>(BOARD_CELLS_COUNT);
    }

    constexpr Vector<uint8_t> checker_index_to_vector(const uint8_t checker_index) {
        return {
            static_cast<uint8_t>(checker_index / CELLS_PER_SIDE),
            static_cast<uint8_t>(checker_index % CHECKERS_PER_ROW)
        };
    }

    constexpr uint8_t checker_vector_to_index(const Vector<uint8_t> checker_vector) {
        return static_cast<uint8_t>(checker_vector.y * CHECKERS_PER_COL + checker_vector.x);
    }

    // Provided that a pawn ends the whole attack chain on enemy`s first row, it must become a king
    std::vector<AttackAction> collect_attacks(const CheckersData& data, const uint8_t checker_index) {
        constexpr std::array<int8_t, 2> even_deviations_x = {0, 1};
        constexpr std::array<int8_t, 2> uneven_deviations_x = {0, -1};
        constexpr auto deviation_cases = std::array{
            std::array{even_deviations_x, uneven_deviations_x},
            std::array{uneven_deviations_x, even_deviations_x}
        };
        const auto checker_pos = checker_index_to_vector(checker_index);
        const auto deviations_x = deviation_cases[is_even(checker_pos.y)];
        const auto checker_player = data.m_player_index[checker_index];
        auto nodes = std::vector<AttackAction>{};
        for(const int8_t dev_x : deviations_x[0]) {
            const auto next_x = static_cast<int8_t>(dev_x + checker_pos.x);
            if(is_coord_not_valid(next_x)) continue;
            const auto jump_x = static_cast<int8_t>(next_x + deviations_x[1][dev_x != deviations_x[0][0]]);
            if(is_coord_not_valid(jump_x)) continue;
            for(const int8_t dev_y : {-1, 1}) {
                const auto next_y = static_cast<int8_t>(checker_pos.y + dev_y);
                if(is_coord_not_valid(next_y)) continue;
                const auto jump_y = static_cast<int8_t>(next_y + dev_y);
                if(is_coord_not_valid(jump_y)) continue;
                const auto next_checker_vector = Vector{
                    static_cast<uint8_t>(next_x),
                    static_cast<uint8_t>(next_y)
                };

                const auto next_checker_index = checker_vector_to_index(next_checker_vector);
                const auto next_checker_player = data.m_player_index[next_checker_index];

                if(next_checker_player == checker_player or not data.m_is_in_place[next_checker_index]) continue;

                const auto jump_checker_vector = Vector{
                    static_cast<uint8_t>(jump_x),
                    static_cast<uint8_t>(jump_y)
                };
                const auto jump_checker_index = checker_vector_to_index(jump_checker_vector);
                if(data.m_is_in_place[jump_checker_index]) continue;

                auto new_data = data;
                new_data.m_is_in_place[checker_index] = false;
                new_data.m_is_in_place[next_checker_index] = false;
                new_data.m_is_in_place[jump_checker_index] = true;
                new_data.m_player_index[jump_checker_index] = checker_player;
                new_data.m_is_king[jump_checker_index] = data.m_is_king[checker_index];

                auto& new_action = nodes.emplace_back(next_checker_index);
                new_action.m_child_actions = collect_attacks(new_data, jump_checker_index);
            }
        }
        return nodes;
    }



    void calculate_pawn_actions(const bool player_index, const CheckersData& data, const uint8_t checker_index) {

        const auto x = static_cast<int8_t>(checker_index % CELLS_PER_SIDE);
        const auto y = static_cast<int8_t>(checker_index / CELLS_PER_SIDE);

        for(const auto y_dev : {-1, 1}) {
            const auto y_shifted = y + y_dev;
            if(y_shifted < 0 or y_shifted >= CELLS_PER_SIDE) {
                continue;
            }
            for(const auto x_dev : {0, 1}) {
                const auto x_shifted = static_cast<int8_t>(x + x_dev);
                if(x_shifted < 0 or x_shifted >= CELLS_PER_SIDE) {
                    continue;
                }
                const auto cell_index = static_cast<uint8_t>(y_shifted * CELLS_PER_SIDE + x_shifted);

                if(data.m_is_in_place[cell_index]) {

                } else {

                    moves.push_back(cell_index);
                }

            }
        }

    }

    void calculate_king_actions(const bool player_index, const CheckersData& data, const uint8_t checker_index) {

    }



}