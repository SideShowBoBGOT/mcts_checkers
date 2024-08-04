#pragma once
#include <bitset>
#include <vector>

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
            static_cast<uint8_t>(board_index % CELLS_PER_SIDE),
            static_cast<uint8_t>(board_index / CELLS_PER_SIDE),
        };
    }

    constexpr uint8_t convert_board_vector_to_board_index(const Vector<uint8_t> board_vector) {
        return board_vector.y * CELLS_PER_SIDE + board_vector.x;
    }

    constexpr uint8_t convert_board_vector_to_checker_index(const Vector<uint8_t> board_vector) {
        return static_cast<uint8_t>(board_vector.y * CHECKERS_PER_ROW + board_vector.x / 2);
    }

    constexpr uint8_t convert_board_index_to_checker_index(const uint8_t board_index) {
        const auto board_vector = convert_board_index_to_board_vector(board_index);
        return convert_board_vector_to_checker_index(board_vector);
    }

    constexpr uint8_t convert_checker_index_to_board_index(const uint8_t checker_index) {
        const auto y = static_cast<uint8_t>(checker_index / CHECKERS_PER_ROW);
        const auto x = static_cast<uint8_t>(checker_index - y * CHECKERS_PER_ROW);
        return y * CELLS_PER_SIDE + x * 2 + is_even(y);
    }

    constexpr Vector<uint8_t> convert_checker_index_to_board_vector(const uint8_t checker_index) {
        const auto board_index = convert_checker_index_to_board_index(checker_index);
        return convert_board_index_to_board_vector(board_index);
    }

    std::vector<uint8_t> collect_moves(const CheckersData& data, uint8_t checker_index);
    std::vector<uint8_t> collect_moves(const CheckersData& data, Vector<uint8_t> checker_index);
    std::pair<std::vector<AttackAction>, uint64_t> collect_attacks(const CheckersData& data, Vector<uint8_t> checker_board_vector);
    std::pair<std::vector<AttackAction>, uint64_t> collect_attacks(const CheckersData& data, uint8_t checker_index);
}