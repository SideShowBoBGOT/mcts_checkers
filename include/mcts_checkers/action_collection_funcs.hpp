#pragma once
#include <mcts_checkers/checkers_types.hpp>

namespace mcts_checkers {

    struct CheckersData;

    std::vector<MoveAction> collect_moves(const CheckersData& data, CheckerIndex checker_index);
    std::vector<MoveAction> collect_moves(const CheckersData& data, BoardVector checker_board_vector);

    CollectAttacksResult collect_attacks(const CheckersData& data, BoardVector checker_board_vector);
    CollectAttacksResult collect_attacks(const CheckersData& data, CheckerIndex checker_index);
}