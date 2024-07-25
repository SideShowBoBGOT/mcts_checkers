#pragma once
#include <bitset>

namespace mcts_checkers {

    struct CheckersData {
        static constexpr uint8_t BOARD_SIDE_CELL_COUNT = 10;
        static constexpr uint8_t BOARD_CELLS_COUNT = BOARD_SIDE_CELL_COUNT * BOARD_SIDE_CELL_COUNT;
        std::bitset<BOARD_CELLS_COUNT> m_is_current_player;
        std::bitset<BOARD_CELLS_COUNT> m_is_in_place;
        std::bitset<BOARD_CELLS_COUNT> m_is_king;
    };

}