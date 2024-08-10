#pragma once
#include <mcts_checkers/checkers_types.hpp>

namespace mcts_checkers {

    struct CheckersData {
        CheckersData();
        CheckersBitset m_player_index;
        CheckersBitset m_is_in_place;
        CheckersBitset m_is_king;
    };

    enum class PlayerIndex {
        FIRST,
        SECOND
    };

    // constexpr operator bool(const PlayerIndex index) {
        // return index == PlayerIndex::SECOND;
    // }

    constexpr PlayerIndex opposite_player(const PlayerIndex index) {
        return index == PlayerIndex::FIRST ? PlayerIndex::SECOND : PlayerIndex::FIRST;
    }

    struct GameData {
        GameData()=default;
        CheckersData checkers;
        PlayerIndex m_current_player_index = PlayerIndex::FIRST;
    };

}