#pragma once

#include <span>
#include <mcts_checkers/board/utils.hpp>

namespace mcts_checkers::board::human {

    namespace initial {
        struct Form {};
    }

    namespace unselected {
        namespace move {
            struct Form {
                action_collection::turn_actions::Output::MakeMove<std::allocator> m_actions;
            };
        }

        namespace attack {
            struct Form {
                action_collection::turn_actions::Output::MakeAttack<std::allocator> m_actions;
            };
        }
    }

    namespace selected {
        namespace move {
            struct Form {
                Form(CheckerIndex checker_index, action_collection::turn_actions::Output::MakeMove<std::allocator>&& actions);
                action_collection::turn_actions::Output::MakeMove<std::allocator> m_actions{};
                action_collection::turn_actions::Output::MakeMove<std::allocator>::const_iterator m_index_actions;
            };
        }
        namespace attack {
            struct Form {
                Form(CheckerIndex index, action_collection::turn_actions::Output::MakeAttack<std::allocator>&& actions);
                action_collection::turn_actions::Output::MakeAttack<std::allocator> m_actions{};
                decltype(m_actions)::const_iterator m_index_actions;
                std::vector<decltype(decltype(m_actions)::value_type::m_actions)::const_iterator> m_selected_actions{};

            };
        }
    }

    using State = std::variant<
        initial::Form,
        unselected::move::Form,
        unselected::attack::Form,
        selected::move::Form,
        selected::attack::Form
    >;

    struct Form {
        State m_state{};
    };


    PlayerMessage::Type iter(Form& form, const GameData& game_data);

    
}