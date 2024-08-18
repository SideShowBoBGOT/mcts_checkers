#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <strong_type/ordered.hpp>

namespace mcts_checkers {

    struct CheckersData {
        CheckersData();
        CheckersBitset m_player_index;
        CheckersBitset m_is_in_place;
        CheckersBitset m_is_king;
    };

    constexpr PlayerIndex opposite_player(const PlayerIndex index) {
        return index == PlayerIndex::FIRST ? PlayerIndex::SECOND : PlayerIndex::FIRST;
    }

    using UninterruptedMovesCount = strong::type<uint8_t, struct MovesCount_, strong::ordered, strong::incrementable>;
    constexpr auto MAX_MOVES_COUNT = UninterruptedMovesCount{40};

    struct GameData {
        GameData()=default;
        CheckersData checkers;
        PlayerIndex m_current_player_index = PlayerIndex::FIRST;
        UninterruptedMovesCount m_moves_count = UninterruptedMovesCount{0};
    };

}