#include <mcts_checkers/checkers_data.hpp>
#include <algorithm>

namespace mcts_checkers {

    std::string get_reversed_string(std::string&& s) {
        std::ranges::reverse(s);
        return s;
    }

    static const auto DEFAULT_PLAYER_INDEX = std::bitset<CHEKCERS_CELLS_COUNT>{
        get_reversed_string(
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
        )
    };

    // static const auto DEFAULT_IS_IN_PLACE = std::bitset<CHEKCERS_CELLS_COUNT>{
    //     get_reversed_string(
    //         "11111"
    //         "11111"
    //         "10011"
    //         "11111"
    //         "01000"
    //         "01100"
    //         "11111"
    //         "11111"
    //         "11111"
    //         "11111"
    //     )
    // };

    static const auto DEFAULT_IS_IN_PLACE = std::bitset<CHEKCERS_CELLS_COUNT>{
        get_reversed_string(
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
        )
    };

    static const auto DEFAULT_IS_KING = std::bitset<CHEKCERS_CELLS_COUNT>{
        get_reversed_string(
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
        )
    };

    CheckersData::CheckersData(
    ) : m_player_index{DEFAULT_PLAYER_INDEX},
        m_is_in_place{DEFAULT_IS_IN_PLACE},
        m_is_king{DEFAULT_IS_KING}
    {}

}