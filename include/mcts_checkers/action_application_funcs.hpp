#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers {

    void apply_move(GameData& game_data, CheckerIndex checker_index, MoveAction move_action);
    void apply_attack(GameData& game_data, const std::vector<AttackAction>& attack_actions);

}