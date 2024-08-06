#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <variant>
#include <vector>

namespace mcts_checkers {
    struct GameData;
}

namespace mcts_checkers::board {

    class Form;

    template<typename T>
    class ProtocolStateChanger {
        public:
            ProtocolStateChanger(T& obj) : m_obj{obj} {}

            template<typename StateType>
            void change_state(StateType&& new_state) const {
                m_obj.change_state(std::forward<StateType>(new_state));
            }

        private:
            T& m_obj;
    };

    struct StateUnselected {
        void iter(ProtocolStateChanger<Form> state_changer, const GameData& game_data);
    };

    namespace selected {

        struct MoveActionForm {
            explicit MoveActionForm(std::vector<MoveAction>&& actions);
            std::vector<MoveAction> m_actions;
        };

        struct AttackActionForm {
        };

        class SelectionConfirmed {
            public:
                SelectionConfirmed(BoardVector board_vector);
                void iter(ProtocolStateChanger<Form> state_changer, const GameData& checkers_data) const;

            private:
                BoardVector m_board_vector;
        };

        using State = std::variant<MoveActionForm, AttackActionForm>;

        class Form {
            public:
                Form(CheckerIndex checker_index, const GameData& game_data);
                void iter(ProtocolStateChanger<board::Form> state_changer, const GameData& checkers_data);

            private:
                CheckerIndex m_index;
                State m_state;
        };
    }

    struct StateSelectionConfirmed {
        void iter(ProtocolStateChanger<Form> state_changer, const GameData& checkers_data);
    };

    using State = std::variant<StateUnselected, selected::Form, StateSelectionConfirmed>;

    class Form {
        public:
            Form();
            void iter(const GameData& checkers_data);
            void change_state(State&& state);

        State m_state = StateUnselected{};
    };

}
