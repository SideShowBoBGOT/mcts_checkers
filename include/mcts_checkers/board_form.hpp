#pragma once
#include <variant>

namespace mcts_checkers {
    struct CheckersData;
}

namespace mcts_checkers::board_form {

    class Form;

    template<typename T>
    class ProtocolStateChanger {
        public:
            ProtocolStateChanger(T& obj) : m_obj{obj} {}

            template<typename State>
            void change_state(State&& new_state) const {
                m_obj.change_state(std::forward<State>(new_state));
            }

        private:
            T& m_obj;
    };

    struct StateUnselected {
        void iter(ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data);
    };

    struct StateSelected {
        void iter(ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data);
    };

    struct StateSelectionConfirmed {
        void iter(ProtocolStateChanger<Form> state_changer, const CheckersData& checkers_data);
    };

    using State = std::variant<StateUnselected, StateSelected, StateSelectionConfirmed>;

    class Form {
        public:
            Form();
            void iter(const CheckersData& checkers_data);
            void change_state(State&& state);

        State m_state;
    };

}
