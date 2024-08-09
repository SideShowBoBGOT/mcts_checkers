#include <mcts_checkers/action_application_funcs.hpp>
#include <mcts_checkers/index_converters.hpp>
#include <cmath>

namespace mcts_checkers {

    void apply_move(GameData& game_data, const CheckerIndex checker_index, const MoveAction move_action) {
        const auto move_action_index = convert_board_index_to_checker_index(move_action._val);
        game_data.checkers.m_is_in_place[checker_index] = false;
        game_data.checkers.m_is_in_place[move_action_index] = true;
        game_data.checkers.m_is_king[move_action_index] = game_data.checkers.m_is_king[checker_index];
        game_data.checkers.m_player_index[move_action_index] = game_data.checkers.m_player_index[checker_index];
        game_data.m_current_player_index = not game_data.m_current_player_index;
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
        return val == 0i8 ? 0i8 : (val > 0i8 ? 1i8 : -1i8);
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

    void apply_attack_step(GameData& game_data, const BoardIndex start, const BoardIndex end) {
        const auto start_vector = VectorInt8(convert_board_index_to_board_vector(start));
        const auto end_vector = VectorInt8(convert_board_index_to_board_vector(end));
        const auto dev = sign(end_vector - start_vector);
        for(auto start = start_vector; start != end_vector; start += dev) {
            const auto index = convert_board_vector_to_checker_index(convert_vectorint8_to_board_vector(start));
            game_data.checkers.m_is_in_place[index] = false;
        }
        const auto end_checker_index = convert_board_vector_to_checker_index(
            convert_vectorint8_to_board_vector(end_vector)
        );
        game_data.checkers.m_is_in_place[end_checker_index] = true;
    }

    void apply_attacks(GameData& game_data, const CheckerIndex checker_index, const std::vector<AttackAction>& attack_actions) {
        apply_attack_step(game_data, convert_checker_index_to_board_index(checker_index), attack_actions[0]._val);
        for(size_t i = 0, j = 1; j < attack_actions.size(); ++i, ++j) {
            apply_attack_step(game_data, attack_actions[i]._val, attack_actions[j]._val);
        }
        game_data.m_current_player_index = not game_data.m_current_player_index;
    }

}