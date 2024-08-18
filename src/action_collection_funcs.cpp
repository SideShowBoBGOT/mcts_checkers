#include <mcts_checkers/action_collection_funcs.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/index_converters.hpp>

#include <array>
#include <algorithm>
#include <mcts_checkers/utils.hpp>

#include <range/v3/algorithm/max_element.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/view/move.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/transform.hpp>

namespace mcts_checkers {

static constexpr auto DEVIATIONS = std::array{
        std::array<int8_t, 2>{-1, -1},
        std::array<int8_t, 2>{CELLS_PER_SIDE, +1},
    };

    std::vector<MoveAction> collect_moves(const CheckersData& data, const BoardVector checker_board_vector) {
        return collect_moves(data, convert_board_vector_to_checker_index(checker_board_vector));
    }

    std::vector<MoveAction> collect_moves(
        const CheckersData& data,
        const CheckerIndex checker_index
    ) {
        assert(data.m_is_in_place[checker_index]);
        const auto checker_vector = convert_checker_index_to_board_vector(checker_index);
        auto actions = std::vector<MoveAction>{};
        const auto is_king = data.m_is_king[checker_index];
        const auto max_iterations = is_king ? DEVIATIONS.size() : 1;
        const size_t start_deviation_index = is_king ? 0 : data.m_player_index[checker_index];
        for(size_t dev_index = start_deviation_index, i = 0; i < max_iterations; ++dev_index, ++i) {
            const auto [bound_y, dev_y] = DEVIATIONS[dev_index];
            for(const auto [bound_x, dev_x] : DEVIATIONS) {
                auto y = static_cast<int8_t>(static_cast<int8_t>(checker_vector.y) + dev_y);
                auto x = static_cast<int8_t>(static_cast<int8_t>(checker_vector.x) + dev_x);
                while(y != bound_y and x != bound_x) {

                    const auto move_board_index = BoardVector{static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
                    const auto move_checker_index = convert_board_vector_to_checker_index(move_board_index);
                    if(data.m_is_in_place[move_checker_index]) {
                        break;
                    }
                    actions.emplace_back(convert_board_vector_to_board_index(move_board_index));

                    if(not is_king) break;
                    y = static_cast<int8_t>(y + dev_y);
                    x = static_cast<int8_t>(x + dev_x);
                }
            }
        }
        return actions;
    }

    CollectAttacksResult collect_attacks(const CheckersData& data, const BoardVector checker_board_vector) {
        return collect_attacks(data, convert_board_vector_to_checker_index(checker_board_vector));
    }

    CollectAttacksResult collect_attacks(
        const CheckersData& data,
        const CheckerIndex checker_index
    ) {
        assert(data.m_is_in_place[checker_index]);

        auto actions = std::vector<AttackTree>{};
        auto action_sizes = std::vector<uint64_t>{};
        const auto is_king = data.m_is_king[checker_index];
        const auto checker_player = data.m_player_index[checker_index];
        const auto checker_vector = convert_checker_index_to_board_vector(checker_index);
        for(const auto [bound_y, dev_y] : DEVIATIONS) {
            for(const auto [bound_x, dev_x] : DEVIATIONS) {
                auto y = static_cast<int8_t>(static_cast<int8_t>(checker_vector.y) + dev_y);
                auto x = static_cast<int8_t>(static_cast<int8_t>(checker_vector.x) + dev_x);
                while(y != bound_y and x != bound_x) {
                    const auto enemy_board_index = BoardVector{static_cast<uint8_t>(x), static_cast<uint8_t>(y)};
                    const auto enemy_checker_index = convert_board_vector_to_checker_index(enemy_board_index);
                    if(data.m_is_in_place[enemy_checker_index]) {
                        if(data.m_player_index[enemy_checker_index] != checker_player) {
                            auto block_y = static_cast<int8_t>(y + dev_y);
                            auto block_x = static_cast<int8_t>(x + dev_x);

                            while(block_y != bound_y and block_x != bound_x) {

                                const auto block_board_vector = BoardVector{static_cast<uint8_t>(block_x), static_cast<uint8_t>(block_y)};
                                const auto block_checker_index = convert_board_vector_to_checker_index(block_board_vector);

                                if(data.m_is_in_place[block_checker_index]) break;

                                auto& attack_action = actions.emplace_back(convert_checker_index_to_board_index(block_checker_index));

                                auto new_data = data;
                                new_data.m_is_in_place[enemy_checker_index] = false;
                                new_data.m_is_in_place[checker_index] = false;
                                new_data.m_is_in_place[block_checker_index] = true;
                                new_data.m_player_index[block_checker_index] = checker_player;
                                new_data.m_is_king[block_checker_index] = is_king;

                                auto [action, action_size] = collect_attacks(new_data, block_checker_index);
                                action_sizes.emplace_back(action_size + 1);
                                attack_action.m_child_trees = std::move(action);

                                if(not is_king) break;

                                block_y = static_cast<int8_t>(block_y + dev_y);
                                block_x = static_cast<int8_t>(block_x + dev_x);
                            }
                        }
                        break;
                    }
                    if(not is_king) break;

                    y = static_cast<int8_t>(y + dev_y);
                    x = static_cast<int8_t>(x + dev_x);
                }
            }
        }

        if(action_sizes.empty()) {
            return {{}, 0};
        }

        const auto max_action_size = *ranges::max_element(action_sizes);
        auto range = ranges::views::zip(ranges::views::move(action_sizes), ranges::views::move(actions))
            | ranges::views::filter([max_action_size](const auto& el) { return el.first == max_action_size; })
            | ranges::views::transform([](auto&& el) { return utils::checked_move(el.second); })
            | ranges::to_vector;

        return {utils::checked_move(range), max_action_size};
    }

}