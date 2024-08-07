#pragma once
#include <mcts_checkers/checkers_types.hpp>

namespace mcts_checkers {

    struct CheckersData;

    template<std::integral T>
    constexpr bool is_even(const T value) {
        return value % 2 == 0;
    }

    constexpr bool is_coord_not_valid(const int8_t coord) {
        return coord < 0 or coord >= static_cast<int8_t>(BOARD_CELLS_COUNT);
    }

    constexpr BoardVector convert_board_index_to_board_vector(const BoardIndex board_index) {
        return {
            static_cast<uint8_t>(board_index._val % CELLS_PER_SIDE),
            static_cast<uint8_t>(board_index._val / CELLS_PER_SIDE),
        };
    }

    constexpr BoardIndex convert_board_vector_to_board_index(const BoardVector board_vector) {
        return BoardIndex{board_vector.y * CELLS_PER_SIDE + board_vector.x};
    }

    constexpr CheckerIndex convert_board_vector_to_checker_index(const BoardVector board_vector) {
        return CheckerIndex{static_cast<uint8_t>(board_vector.y * CHECKERS_PER_ROW + board_vector.x / 2)};
    }

    constexpr CheckerIndex convert_board_index_to_checker_index(const BoardIndex board_index) {
        const auto board_vector = convert_board_index_to_board_vector(board_index);
        return convert_board_vector_to_checker_index(board_vector);
    }

    constexpr BoardIndex convert_checker_index_to_board_index(const CheckerIndex checker_index) {
        const auto y = static_cast<uint8_t>(checker_index._val / CHECKERS_PER_ROW);
        const auto x = static_cast<uint8_t>(checker_index._val - y * CHECKERS_PER_ROW);
        return BoardIndex{static_cast<uint8_t>(y * CELLS_PER_SIDE + x * 2 + is_even(y))};
    }

    constexpr BoardVector convert_checker_index_to_board_vector(const CheckerIndex checker_index) {
        const auto board_index = convert_checker_index_to_board_index(checker_index);
        return convert_board_index_to_board_vector(board_index);
    }

    std::vector<MoveAction> collect_moves(const CheckersData& data, CheckerIndex checker_index);
    std::vector<MoveAction> collect_moves(const CheckersData& data, BoardVector checker_board_vector);
    std::pair<std::vector<AttackAction>, uint64_t> collect_attacks(const CheckersData& data, BoardVector checker_board_vector);
    std::pair<std::vector<AttackAction>, uint64_t> collect_attacks(const CheckersData& data, CheckerIndex checker_index);
}