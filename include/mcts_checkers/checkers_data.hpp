#pragma once
#include <bitset>

namespace mcts_checkers {

    struct CheckersData {
        static constexpr uint8_t BOARD_SIDE_CELL_COUNT = 10;
        static constexpr uint8_t BOARD_CELLS_COUNT = BOARD_SIDE_CELL_COUNT * BOARD_SIDE_CELL_COUNT;
        static constexpr uint8_t DARK_SQUARES_COUNT = BOARD_CELLS_COUNT / 2;
        CheckersData();
        std::bitset<DARK_SQUARES_COUNT> m_player_index;
        std::bitset<DARK_SQUARES_COUNT> m_is_in_place;
        std::bitset<DARK_SQUARES_COUNT> m_is_king;
    };

}