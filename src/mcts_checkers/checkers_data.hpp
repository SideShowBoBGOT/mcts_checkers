#pragma once
#include <strong_type/ordered.hpp>
#include <strong_type/strong_type.hpp>
#include <bitset>

namespace mcts_checkers {

    constexpr uint8_t CELLS_PER_SIDE = 10;
    constexpr uint8_t CHECKERS_PER_ROW = CELLS_PER_SIDE / 2;
    constexpr uint8_t CHECKERS_PER_COL = CELLS_PER_SIDE;
    constexpr uint8_t BOARD_CELLS_COUNT = CELLS_PER_SIDE * CELLS_PER_SIDE;
    constexpr uint8_t CHEKCERS_CELLS_COUNT = BOARD_CELLS_COUNT / 2;

    using CheckerIndex = strong::type<uint8_t, struct CheckersIndex_, strong::incrementable, strong::equality>;

    using CheckersBitset = strong::type<std::bitset<CHEKCERS_CELLS_COUNT>, struct CheckersBitset_, strong::indexed<CheckerIndex>>;
    using BoardIndex = strong::type<uint8_t, struct BoardIndex_, strong::equality, strong::hashable>;


    struct BoardVector {
        uint8_t x{};
        uint8_t y{};
    };
    constexpr bool operator==(const BoardVector first, const BoardVector second) {
        return first.x == second.x and first.y == second.y;
    }

    enum class PlayerIndex {
        FIRST,
        SECOND
    };

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