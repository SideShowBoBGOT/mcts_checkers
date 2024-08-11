#include <mcts_checkers/action_application_funcs.hpp>
#include <mcts_checkers/index_converters.hpp>
#include <cmath>

namespace mcts_checkers {

    static constexpr std::array<uint8_t, 2> KING_Y_POSITIONS = {
        static_cast<uint8_t>(CELLS_PER_SIDE - 1),
        0
    };


    void apply_move(GameData& game_data, const CheckerIndex checker_index, const MoveAction move_action) {
        const auto move_action_index = convert_board_index_to_checker_index(move_action._val);

        game_data.checkers.m_is_in_place[checker_index] = false;
        game_data.checkers.m_is_in_place[move_action_index] = true;

        {
            const auto y_pos = convert_board_index_to_board_vector(move_action._val).y;
            const auto is_king_pos = y_pos == KING_Y_POSITIONS[static_cast<uint8_t>(game_data.m_current_player_index)];
            game_data.checkers.m_is_king[move_action_index] = is_king_pos ? true : game_data.checkers.m_is_king[checker_index];
        }

        game_data.checkers.m_player_index[move_action_index] = game_data.checkers.m_player_index[checker_index];
        game_data.m_current_player_index = opposite_player(game_data.m_current_player_index);
    }

    struct VectorInt8 {
        constexpr VectorInt8()=default;
        constexpr VectorInt8(const int8_t x, const int8_t y)
            : m_x{x}, m_y{y} {}
        explicit VectorInt8(const BoardVector v) :
            m_x{static_cast<int8_t>(v.x)},
            m_y{static_cast<int8_t>(v.y)}
        {}

        int8_t m_x{};
        int8_t m_y{};
    };

    VectorInt8 operator-(const VectorInt8 first, const VectorInt8 second) {
        return VectorInt8{
            static_cast<int8_t>(first.m_x - second.m_x),
            static_cast<int8_t>(first.m_y - second.m_y)
        };
    }

    VectorInt8 operator+(const VectorInt8 first, const VectorInt8 second) {
        return VectorInt8{
            static_cast<int8_t>(first.m_x + second.m_x),
            static_cast<int8_t>(first.m_y + second.m_y)
        };
    }

    VectorInt8& operator+=(VectorInt8& first, const VectorInt8 second) {
        first = first + second;
        return first;
    }

    bool operator==(const VectorInt8 first, const VectorInt8 second) {
        return first.m_x == second.m_x and first.m_y == second.m_y;
    }

    constexpr int8_t sign(const int8_t val) {
        return val == 0 ? static_cast<int8_t>(0) : static_cast<int8_t>(val > 0 ? 1 : -1);
    }

    VectorInt8 sign(const VectorInt8 v) {
        return {
            sign(v.m_x),
            sign(v.m_y),
        };
    }

    BoardVector convert_vectorint8_to_board_vector(const VectorInt8 v) {
        return {
            static_cast<uint8_t>(v.m_x),
            static_cast<uint8_t>(v.m_y)
        };
    }

    void apply_attack_step(CheckersData& checkers, const BoardIndex start, const BoardIndex end) {
        const auto start_vector = VectorInt8(convert_board_index_to_board_vector(start));
        const auto end_vector = VectorInt8(convert_board_index_to_board_vector(end));
        const auto dev = sign(end_vector - start_vector);
        for(auto i = start_vector; i != end_vector; i += dev) {
            const auto index = convert_board_vector_to_checker_index(convert_vectorint8_to_board_vector(i));
            checkers.m_is_in_place[index] = false;
        }
        const auto start_checker_index = convert_board_vector_to_checker_index(
            convert_vectorint8_to_board_vector(start_vector)
        );
        const auto end_checker_index = convert_board_vector_to_checker_index(
            convert_vectorint8_to_board_vector(end_vector)
        );
        checkers.m_is_in_place[end_checker_index] = true;
        checkers.m_is_king[end_checker_index] = checkers.m_is_king[start_checker_index];
        checkers.m_player_index[end_checker_index] = checkers.m_player_index[start_checker_index];;
    }

    void apply_attack(GameData& game_data, const std::vector<AttackAction>& attack_actions) {
        for(size_t i = 0, j = 1; j < attack_actions.size(); ++i, ++j) {
            apply_attack_step(game_data.checkers, attack_actions[i]._val, attack_actions[j]._val);
        }
        const auto back_board_index = attack_actions.back()._val;
        const auto back_checker_index = convert_board_index_to_checker_index(back_board_index);
        const auto y_pos = convert_board_index_to_board_vector(back_board_index).y;
        const auto is_king_pos = y_pos == KING_Y_POSITIONS[static_cast<uint8_t>(game_data.m_current_player_index)];
        game_data.checkers.m_is_king[back_checker_index] = is_king_pos ? true : game_data.checkers.m_is_king[back_checker_index];

        game_data.m_current_player_index = opposite_player(game_data.m_current_player_index);
    }

}