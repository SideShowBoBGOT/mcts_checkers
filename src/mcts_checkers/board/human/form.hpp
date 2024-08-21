#pragma once

#include <variant>
#include <span>
#include <mcts_checkers/board/utils.hpp>


namespace mcts_checkers::board::human {

    namespace initial {
        struct Form {};
    }

    namespace unselected {
        namespace move {
            struct Form { turn_actions::MakeMove m_actions; };
        }

        namespace attack {
            struct Form { turn_actions::MakeAttack m_actions; };
        }
    }

    namespace selected {
        namespace move {
            struct Form {
                Form(CheckerIndex checker_index, std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>>&& actions);
                CheckerIndex m_index;
                std::span<const MoveAction> m_index_actions;
                std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>> m_actions{};
            };
        }
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

    using State = std::variant<
        initial::Form,
        unselected::move::Form,
        unselected::attack::Form,
        selected::move::Form,
        selected::attack::Form,
    >;

    struct Form {
        State m_state{};
    };

    namespace OutMessage {
        struct PlayerMadeNoSelection {};
        struct PlayerMadeSelection { GameData m_game_data; };
        struct DeclareLoss { PlayerIndex m_index; };
        struct DeclareDraw {};
        using Type = std::variant<
            PlayerMadeNoSelection,
            PlayerMadeSelection,
            DeclareLoss,
            DeclareDraw
        >;
    }

    OutMessage::Type iter(Form& form, const GameData& game_data);

    
}