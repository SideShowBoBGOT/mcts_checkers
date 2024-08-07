#pragma once
#include <mcts_checkers/checkers_types.hpp>

namespace mcts_checkers {

    struct CheckersData {
        CheckersData();
        CheckersBitset m_player_index;
        CheckersBitset m_is_in_place;
        CheckersBitset m_is_king;
    };

    struct GameData {
        GameData()=default;
        CheckersData checkers;
        bool m_current_player_index = true;
    };

}