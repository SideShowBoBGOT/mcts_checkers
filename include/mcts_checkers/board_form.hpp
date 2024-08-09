#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <variant>
#include <vector>
#include <span>
#include <tl/optional.hpp>

namespace mcts_checkers {
    struct GameData;
}

namespace mcts_checkers::board::human {

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
                tl::optional<BoardIndex> m_index;
                std::span<const AttackTree> m_actions;
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

    using Form = strong::type<
        std::variant<
            InitialState,
            unselected::AttackForm,
            unselected::MoveForm,
            selected::MoveForm,
            selected::attack::Form
        >,
        class Form_,
        strong::default_constructible
    >;
}

namespace mcts_checkers::board::ai {

    struct Form {};

}

namespace mcts_checkers::board {
    using State = std::variant<
        human::Form,
        ai::Form
    >;

    struct Form {
        State m_state{};
        GameData m_game_data{};
    };

    void iter(Form& form);
}