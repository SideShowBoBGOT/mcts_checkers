#pragma once
#include <bitset>
#include <concepts>
#include <array>
#include <vector>
#include <boost/container/static_vector.hpp>

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
        uint8_t m_board_index{};
        std::vector<AttackAction> m_child_actions{};
    };

    template<typename T>
    struct Vector {
        T x{};
        T y{};
    };

    template<std::integral T>
    constexpr bool is_even(const T value) {
        return value % 2 == 0;
    }

    constexpr bool is_coord_not_valid(const int8_t coord) {
        return coord < 0 or coord >= static_cast<int8_t>(BOARD_CELLS_COUNT);
    }

    constexpr Vector<uint8_t> convert_board_index_to_board_vector(const uint8_t board_index) {
        return {
            static_cast<uint8_t>(board_index / CELLS_PER_SIDE),
            static_cast<uint8_t>(board_index % CELLS_PER_SIDE),
        };
    }

    constexpr uint8_t convert_board_vector_to_checker_index(const Vector<uint8_t> board_index) {
        return board_index.y * (CELLS_PER_SIDE / 2) + board_index.x / 2;
    }

    constexpr uint8_t convert_checker_index_to_board_index(const uint8_t checker_index) {
        return static_cast<uint8_t>(checker_index * 2 + 1);
    }

    constexpr Vector<uint8_t> convert_checker_index_to_board_vector(const uint8_t checker_index) {
        return convert_board_index_to_board_vector(convert_checker_index_to_board_index(checker_index));
    }

    namespace king_attacks {
        std::vector<AttackAction> collect_king_attacks(const CheckersData& data, const uint8_t checker_index) {
            constexpr auto deviations = std::array{
                std::array<int8_t, 2>{-1, -1},
                std::array<int8_t, 2>{CELLS_PER_SIDE, +1}
            };
            auto attack_actions = std::vector<AttackAction>{};

            const auto checker_player = data.m_player_index[checker_index];
            const auto checker_vector = convert_checker_index_to_board_vector(checker_index);
            for(const auto [bound_y, dev_y] : deviations) {
                for(const auto [bound_x, dev_x] : deviations) {
                    auto y = static_cast<int8_t>(static_cast<int8_t>(checker_vector.y) + dev_y);
                    auto x = static_cast<int8_t>(static_cast<int8_t>(checker_vector.x) + dev_x);
                    while(y != bound_y and x != bound_x) {
                        const auto enemy_board_index = Vector{
                            static_cast<uint8_t>(x),
                            static_cast<uint8_t>(y)
                        };
                        const auto enemy_checker_index = convert_board_vector_to_checker_index(enemy_board_index);
                        if(
                            data.m_is_in_place[enemy_checker_index]
                            and data.m_player_index[enemy_checker_index] != checker_player
                        ) {
                            auto block_y = static_cast<int8_t>(y + dev_y);
                            auto block_x = static_cast<int8_t>(x + dev_x);

                            while(block_y != bound_y and block_x != bound_x) {

                                const auto block_board_vector = Vector{
                                    static_cast<uint8_t>(block_x),
                                    static_cast<uint8_t>(block_y)
                                };

                                const auto block_checker_index = convert_board_vector_to_checker_index(block_board_vector);

                                if(data.m_is_in_place[block_checker_index]) {
                                    break;
                                }

                                auto& attack_action = attack_actions.emplace_back(convert_checker_index_to_board_index(block_checker_index));

                                auto new_data = data;
                                new_data.m_is_in_place.set(enemy_checker_index, false);
                                new_data.m_is_in_place.set(checker_index, false);

                                new_data.m_is_in_place.set(block_checker_index, true);
                                new_data.m_player_index.set(block_checker_index, checker_player);
                                new_data.m_is_king.set(block_checker_index, true);

                                attack_action.m_child_actions = collect_king_attacks(new_data, block_checker_index);

                                block_y = static_cast<int8_t>(block_y + dev_y);
                                block_x = static_cast<int8_t>(block_x + dev_x);
                            }
                            break;
                        }
                        y = static_cast<int8_t>(y + dev_y);
                        x = static_cast<int8_t>(x + dev_x);
                    }
                }
            }
        }
    }






}