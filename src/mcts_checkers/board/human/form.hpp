#pragma once

#include <variant>

namespace mcts_checkers::board::human {

    namespace detail {
        
        namespace initial {
            struct State {};
            
        }

        using State = std::variant<
            initial::State
        >;
    }

    struct Form {
        detail::State m_state{};
    };

    

}