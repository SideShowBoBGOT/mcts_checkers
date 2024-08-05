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

        class Form;

        class MoveActionForm {
            public:
                MoveActionForm(std::vector<MoveAction>&& actions);
                void iter(ProtocolStateChanger<Form> state_changer, const GameData& checkers_data) const;

            private:
                std::vector<MoveAction> m_actions;

        };

        class AttackActionForm {
            public:
                void iter(ProtocolStateChanger<Form> state_changer, const GameData& checkers_data) const;
        };

        using State = std::variant<MoveActionForm, AttackActionForm>;

        class Form {
            public:
                Form(CheckerIndex checker_index, const GameData& game_data);
                void iter(ProtocolStateChanger<board::Form> state_changer, const GameData& checkers_data);
                void change_state(State&& state);

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
