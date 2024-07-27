#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers {

    static const auto DEFAULT_PLAYER_INDEX = std::bitset<CHEKCERS_CELLS_COUNT>{
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "11111"
        "11111"
        "11111"
        "11111"
    };

    static const auto DEFAULT_IS_IN_PLACE = std::bitset<CHEKCERS_CELLS_COUNT>{
        "11111"
        "11111"
        "11111"
        "11111"
        "00000"
        "00000"
        "11111"
        "11111"
        "11111"
        "11111"
    };

    static const auto DEFAULT_IS_KING = std::bitset<CHEKCERS_CELLS_COUNT>{
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
    };

    CheckersData::CheckersData(
    ) : m_player_index{DEFAULT_PLAYER_INDEX},
        m_is_in_place{DEFAULT_IS_IN_PLACE},
        m_is_king{DEFAULT_IS_KING} {}
}