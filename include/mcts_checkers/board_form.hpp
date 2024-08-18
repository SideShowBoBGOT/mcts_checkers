#pragma once
#include <mcts_checkers/checkers_types.hpp>
#include <mcts_checkers/checkers_data.hpp>
#include <variant>
#include <vector>
#include <span>
#include <future>

namespace mcts_checkers {
    struct GameData;
}

namespace mcts_checkers::board {
    namespace selection_confirmed {
        struct Move {
            MoveAction data;
            CheckerIndex checker_index;
        };
        struct Attack {
            std::vector<AttackAction> data;
        };
    }
    namespace turn_actions {
        using MakeAttack = std::vector<std::pair<CheckerIndex, CollectAttacksResult>>;
        using MakeMove = std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>;
        struct DeclareLoss {};
        struct DeclareDraw {};
        using Type = std::variant<DeclareLoss, DeclareDraw, MakeAttack, MakeMove>;
    }
}


namespace mcts_checkers::board::human {

    namespace unselected {
        struct AttackForm {
            turn_actions::MakeAttack m_actions{};
        };

        struct MoveForm {
            turn_actions::MakeMove m_actions{};
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

    namespace initial {
        struct State {};
    }

    using Form = strong::type<
        std::variant<
            initial::State,
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
        selection_confirmed::Move,
        selection_confirmed::Attack,
        turn_actions::DeclareLoss,
        turn_actions::DeclareDraw
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

    struct DeclareWin { PlayerIndex m_player_index; };
    struct DeclareDraw {};
    struct MakingDecision { PlayerIndex m_player_index; };

    using OutMessage = std::variant<
        DeclareWin,
        DeclareDraw,
        MakingDecision
    >;

    OutMessage iter_out(Form& form);
}