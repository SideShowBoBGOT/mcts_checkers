#pragma once
#include <bitset>
#include <strong_type/type.hpp>
#include <strong_type/indexed.hpp>
#include <strong_type/incrementable.hpp>
#include <strong_type/equality.hpp>
#include <strong_type/hashable.hpp>

namespace mcts_checkers {
    constexpr uint8_t CELLS_PER_SIDE = 10;
    constexpr uint8_t CHECKERS_PER_ROW = CELLS_PER_SIDE / 2;
    constexpr uint8_t CHECKERS_PER_COL = CELLS_PER_SIDE;
    constexpr uint8_t BOARD_CELLS_COUNT = CELLS_PER_SIDE * CELLS_PER_SIDE;
    constexpr uint8_t CHEKCERS_CELLS_COUNT = BOARD_CELLS_COUNT / 2;

    using CheckerIndex = strong::type<uint8_t, struct CheckersIndex_, strong::incrementable, strong::equality>;
    using CheckersBitset = strong::type<std::bitset<CHEKCERS_CELLS_COUNT>, struct CheckersBitset_, strong::indexed<CheckerIndex>>;
    using BoardIndex = strong::type<uint8_t, struct BoardIndex_, strong::equality, strong::hashable>;
    using MoveAction = strong::type<BoardIndex, struct MoveAction_>;
}