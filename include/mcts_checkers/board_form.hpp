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
            std::vector<std::pair<CheckerIndex, CollectAttacksResult>> data{};
        };

        struct MoveForm {
            std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>> data{};
        };
    }

    struct StateUnselected {
        explicit StateUnselected(const GameData& game_data);
    };

    namespace selected {

        struct MoveForm {
            CheckerIndex m_index;
            std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>> data{};
        };

        namespace attack {
            struct Node {
                std::optional<BoardIndex> m_index;
                std::span<AttackAction> m_actions;
            };

            struct Form {
                Form(CheckerIndex index, std::vector<AttackAction>&& actions);
                CheckerIndex m_index;
                std::vector<Node> m_nodes;
                std::vector<std::pair<CheckerIndex, std::vector<MoveAction>>> data{};
            };
        }

        struct SelectionConfirmed {
            BoardVector m_board_vector;
        };

        using State = std::variant<MoveForm, attack::Form>;

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
