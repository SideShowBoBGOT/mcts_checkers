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

    namespace unselected {
        struct AttackForm {
            std::vector<std::pair<CheckerIndex, CollectAttacksResult>> m_actions{};
        };

        struct MoveForm {
            std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>> m_actions{};
        };
    }

    namespace selected {

        struct MoveForm {
            MoveForm(CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>&& actions);
            CheckerIndex m_index;
            std::span<const MoveAction> m_index_actions;
            std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>> m_actions{};
        };

        namespace attack {
            struct Node {
                std::optional<BoardIndex> m_index;
                std::span<const AttackAction> m_actions;
            };

            struct Form {
                Form(CheckerIndex index, std::vector<std::pair<CheckerIndex, CollectAttacksResult>>&& actions);
                CheckerIndex m_index;
                std::vector<Node> m_index_nodes;
                std::vector<std::pair<CheckerIndex, CollectAttacksResult>> m_actions{};
            };
        }
    }

    struct InitialState {};
    struct SelectionConfirmed {};

    using State = std::variant<
        InitialState,
        unselected::AttackForm, unselected::MoveForm,
        selected::MoveForm, selected::attack::Form
    >;

    class Form {
        public:
            Form();
            void iter_sss(const GameData& game_data);
            void change_state(State&& state);

        State m_state = InitialState{};
    };

}
