#include <mcts_checkers/checkers_data.hpp>
#include <array>
#include <algorithm>
#include <mcts_checkers/utils.hpp>

#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/to_container.hpp>
#include <range/v3/view/transform.hpp>

namespace mcts_checkers {

    static const auto DEFAULT_PLAYER_INDEX = std::bitset<CHEKCERS_CELLS_COUNT>{
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "11111"
        "11111"
        "11111"
        "11111"
    };

    static const auto DEFAULT_IS_IN_PLACE = std::bitset<CHEKCERS_CELLS_COUNT>{
        "11111"
        "11111"
        "11111"
        "11111"
        "00000"
        "00000"
        "11111"
        "11111"
        "11111"
        "11111"
    };

    static const auto DEFAULT_IS_KING = std::bitset<CHEKCERS_CELLS_COUNT>{
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
    };

    CheckersData::CheckersData(
    ) : m_player_index{DEFAULT_PLAYER_INDEX},
        m_is_in_place{DEFAULT_IS_IN_PLACE},
        m_is_king{DEFAULT_IS_KING}
    {}

    std::pair<std::vector<AttackAction>, uint64_t> collect_king_attacks(const CheckersData& data, Vector<uint8_t> checker_board_vector) {
        return collect_king_attacks(data, convert_board_vector_to_checker_index(checker_board_vector));
    }

    std::pair<std::vector<AttackAction>, uint64_t> collect_king_attacks(const CheckersData& data, const uint8_t checker_index) {
        constexpr auto deviations = std::array{
            std::array<int8_t, 2>{-1, -1},
            std::array<int8_t, 2>{CELLS_PER_SIDE, +1}
        };
        auto actions = std::vector<AttackAction>{};
        auto action_sizes = std::vector<uint64_t>{};
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

                            auto& attack_action = actions.emplace_back(convert_checker_index_to_board_index(block_checker_index));

                            auto new_data = data;
                            new_data.m_is_in_place.set(enemy_checker_index, false);
                            new_data.m_is_in_place.set(checker_index, false);

                            new_data.m_is_in_place.set(block_checker_index, true);
                            new_data.m_player_index.set(block_checker_index, checker_player);
                            new_data.m_is_king.set(block_checker_index, true);

                            {
                                auto [action, action_size] = collect_king_attacks(new_data, block_checker_index);
                                action_sizes.emplace_back(action_size + 1);
                                attack_action.m_child_actions = std::move(action);
                            }

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

        const auto max_action_size = *ranges::max_element(action_sizes);
        auto range = ranges::views::zip(ranges::views::move(action_sizes), ranges::views::move(actions))
            | ranges::views::filter([max_action_size](const auto& el) { return el.first == max_action_size; })
            | ranges::views::transform([](auto&& el) { return utils::checked_move(el.second); })
            | ranges::to_vector;

        return {utils::checked_move(range), max_action_size};
    }

}