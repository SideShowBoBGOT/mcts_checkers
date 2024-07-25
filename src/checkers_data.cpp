#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers {



    static constexpr auto DEFAULT_PLAYER_INDEX = std::bitset<CheckersData::BOARD_CELLS_COUNT>(
        "11111"
        "11111"
        "11111"
        "11111"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
        "00000"
    );

    static constexpr auto DEFAULT_IS_IN_PLACE = std::bitset<CheckersData::BOARD_CELLS_COUNT>(
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
    );

    static constexpr auto DEFAULT_IS_KING = std::bitset<CheckersData::BOARD_CELLS_COUNT>(
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
    );

    CheckersData::CheckersData() {

    }


}