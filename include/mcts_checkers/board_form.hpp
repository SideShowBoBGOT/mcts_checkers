#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <variant>
#include <vector>
#include <optional>
#include <span>

namespace mcts_checkers {
    struct GameData;
}

namespace mcts_checkers::board {

    struct StateUnselected {};

    namespace selected {

        struct MoveActionForm {
            CheckerIndex m_index;
            std::vector<MoveAction> m_actions;
        };

        namespace attack {
            struct Node {
                std::optional<CheckerIndex> m_index;
                std::span<AttackAction> m_actions;
            };

            struct Form {
                Form(std::vector<AttackAction>&& actions);
                std::vector<Node> m_nodes;
                std::vector<AttackAction> m_actions;
            };
        }

        struct SelectionConfirmed {
            BoardVector m_board_vector;
        };

        using State = std::variant<MoveActionForm, attack::Form>;

        struct Form {
            Form(CheckerIndex checker_index, const GameData& game_data);
            CheckerIndex m_index;
            State m_state;
        };
    }

    struct StateSelectionConfirmed { BoardVector m_board_vector; };

    using State = std::variant<StateUnselected, selected::Form, StateSelectionConfirmed>;

    class Form {
        public:
            Form();
            void iter_sss(const GameData& checkers_data);
            void change_state(State&& state);

        State m_state = StateUnselected{};
    };

}
