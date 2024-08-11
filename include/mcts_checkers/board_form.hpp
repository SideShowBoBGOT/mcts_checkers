#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <variant>
#include <vector>
#include <span>
#include <tl/optional.hpp>
#include <future>

namespace mcts_checkers {
    struct GameData;
}


namespace mcts_checkers::board::human_ai_common {
    namespace selection_confirmed {
        struct Move {
            MoveAction data;
            CheckerIndex checker_index;
        };
        struct Attack {
            std::vector<AttackAction> data;
        };
    }
    namespace available_actions {
        using Attacks = std::vector<std::pair<CheckerIndex, CollectAttacksResult>>;
        using Moves = std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>;
        using Type = std::variant<Attacks, Moves>;
    }
}


namespace mcts_checkers::board::human {

    namespace unselected {
        struct AttackForm {
            human_ai_common::available_actions::Attacks m_actions{};
        };

        struct MoveForm {
            human_ai_common::available_actions::Moves m_actions{};
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
                BoardIndex m_index;
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

    using StrategyResult = std::variant<
        human_ai_common::selection_confirmed::Move,
        human_ai_common::selection_confirmed::Attack
    >;

    struct Form {
        Form(const GameData& game_data);
        std::future<StrategyResult> m_task;
    };

}

namespace mcts_checkers::board {
    using State = std::variant<
        human::Form,
        ai::Form
    >;

    struct Form {
        Form();
        GameData m_game_data{};
        State m_state;
    };

    void iter(Form& form);
}