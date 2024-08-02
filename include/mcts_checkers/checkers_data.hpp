#pragma once
#include <bitset>
#include <concepts>
#include <vector>
#include <boost/container/static_vector.hpp>

namespace mcts_checkers {
    constexpr uint8_t CELLS_PER_SIDE = 10;
    constexpr uint8_t CHECKERS_PER_ROW = CELLS_PER_SIDE / 2;
    constexpr uint8_t CHECKERS_PER_COL = CELLS_PER_SIDE;
    constexpr uint8_t BOARD_CELLS_COUNT = CELLS_PER_SIDE * CELLS_PER_SIDE;
    constexpr uint8_t CHEKCERS_CELLS_COUNT = BOARD_CELLS_COUNT / 2;

    struct CheckersData {
        CheckersData();
        std::bitset<CHEKCERS_CELLS_COUNT> m_player_index;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_in_place;
        std::bitset<CHEKCERS_CELLS_COUNT> m_is_king;
    };

    struct GameData {
        GameData()=default;
        CheckersData data;
        bool m_current_player_index = false;
    };

    struct AttackAction {
        uint8_t m_board_index{};
        std::vector<AttackAction> m_child_actions{};
    };

    template<typename T>
    struct Vector {
        T x{};
        T y{};
    };

    template<std::integral T>
    constexpr bool is_even(const T value) {
        return value % 2 == 0;
    }

    constexpr bool is_coord_not_valid(const int8_t coord) {
        return coord < 0 or coord >= static_cast<int8_t>(BOARD_CELLS_COUNT);
    }

    constexpr Vector<uint8_t> convert_board_index_to_board_vector(const uint8_t board_index) {
        return {
            static_cast<uint8_t>(board_index / CELLS_PER_SIDE),
            static_cast<uint8_t>(board_index % CELLS_PER_SIDE),
        };
    }

    constexpr uint8_t convert_board_index_to_checker_index(const uint8_t board_index) {
        return static_cast<uint8_t>((board_index - 1) / 2);
    }

    constexpr uint8_t convert_board_vector_to_checker_index(const Vector<uint8_t> board_vector) {
        return board_vector.y * (CELLS_PER_SIDE / 2) + board_vector.x / 2;
    }

    constexpr uint8_t convert_checker_index_to_board_index(const uint8_t checker_index) {
        return static_cast<uint8_t>(checker_index * 2 + 1);
    }

    constexpr Vector<uint8_t> convert_checker_index_to_board_vector(const uint8_t checker_index) {
        return convert_board_index_to_board_vector(convert_checker_index_to_board_index(checker_index));
    }

    std::vector<AttackAction> collect_king_attacks(const CheckersData& data, uint8_t checker_index);
}