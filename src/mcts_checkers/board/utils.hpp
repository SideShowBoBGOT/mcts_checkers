#pragma once

#include <mcts_checkers/checkers_data.hpp>
#include <variant>

namespace mcts_checkers {

    namespace turn_actions {
        using MakeAttack = std::vector<std::pair<CheckerIndex, CollectAttacksResult>>;
        using MakeMove = std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>;
        struct DeclareLoss { PlayerIndex m_player_index; };
        struct DeclareDraw {};
        using Type = std::variant<DeclareLoss, DeclareDraw, MakeAttack, MakeMove>;

        Type determine(const GameData& game_data);
    }

    ImVec2 calc_cell_size();
    ImVec2 calc_cell_top_left(const BoardVector board_index);
}