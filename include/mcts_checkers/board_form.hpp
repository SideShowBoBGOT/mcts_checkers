#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <variant>
#include <vector>

namespace mcts_checkers {
    struct GameData;
}

namespace mcts_checkers::board {

    struct StateUnselected {};

    namespace selected {

        struct MoveActionForm {
            explicit MoveActionForm(std::vector<MoveAction>&& actions);
            std::vector<MoveAction> m_actions;
        };
        struct AttackActionForm {};
        struct SelectionConfirmed {
            BoardVector m_board_vector;
        };

        using State = std::variant<MoveActionForm, AttackActionForm>;

        struct Form {
            Form(CheckerIndex checker_index, const GameData& game_data);
            CheckerIndex m_index;
            State m_state;
        };
    }

    struct StateSelectionConfirmed {};

    using State = std::variant<StateUnselected, selected::Form, StateSelectionConfirmed>;

    class Form {
        public:
            Form();
            void iter_sss(const GameData& checkers_data);
            void change_state(State&& state);

        State m_state = StateUnselected{};
    };

}
