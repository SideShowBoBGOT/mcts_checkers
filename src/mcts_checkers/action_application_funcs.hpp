#pragma once
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/utils.hpp>
#include <list>

namespace mcts_checkers {

    struct MoveAction {
        MoveAction(const CheckerIndex checker_index, const BoardIndex destination)
            : m_checker_index(checker_index), m_destination(destination) {}
        CheckerIndex m_checker_index;
        BoardIndex m_destination;
    };

    struct AttackAction {
        AttackAction(const CheckerIndex checker_index, std::vector<BoardIndex>&& destination)
            : m_checker_index(checker_index), m_destinations(utils::checked_move(destination)) {}
        CheckerIndex m_checker_index;
        std::vector<BoardIndex> m_destinations;
    };

    void apply_move(GameData& game_data, MoveAction action);
    void apply_attack(GameData& game_data, const AttackAction& action);
}