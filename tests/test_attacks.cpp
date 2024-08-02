#include <gtest/gtest.h>

#include <mcts_checkers/checkers_data.hpp>
#include <filesystem>

static const auto DEFAULT_PLAYER_INDEX = std::bitset<mcts_checkers::CHEKCERS_CELLS_COUNT>{
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

static const auto DEFAULT_IS_IN_PLACE = std::bitset<mcts_checkers::CHEKCERS_CELLS_COUNT>{
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

static const auto DEFAULT_IS_KING = std::bitset<mcts_checkers::CHEKCERS_CELLS_COUNT>{
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

TEST(TestAttacks, Sanity) {

}
