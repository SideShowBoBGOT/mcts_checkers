#pragma once
#include <bitset>
#include <concepts>

namespace mcts_checkers {
    constexpr uint8_t BOARD_SIDE_CELL_COUNT = 10;
    constexpr uint8_t BOARD_CELLS_COUNT = BOARD_SIDE_CELL_COUNT * BOARD_SIDE_CELL_COUNT;
    constexpr uint8_t CHEKCERS_CELLS_COUNT = BOARD_CELLS_COUNT / 2;

    struct CheckersData {
        CheckersData();
        std::bitset<CHEKCERS_CELLS_COUNT> m_player_index;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_in_place;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_king;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_done_first_move;
        bool m_current_player_index = false;
    };

    template<class Fn, class Ret, class... Args>
    concept invocable_r = std::is_invocable_r_v<Ret, Fn, Args...>;

    template<invocable_r<void, uint8_t, uint8_t, bool> Callable>
        void checkers_board_iterate(Callable&& obj) {

    }

}