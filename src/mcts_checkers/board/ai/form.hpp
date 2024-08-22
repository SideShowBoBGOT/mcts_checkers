#pragma once

#include <variant>
#include <mcts_checkers/board/utils.hpp>
#include <future>

namespace mcts_checkers::board::ai {

    struct Initial {};
    struct Active {
        Active(const GameData& game_data);
        std::future<PlayerMessage::Type> m_task;
    };

    using State = std::variant<
        Initial, Active
    >;

    struct Form {
        State m_state;
    };

    PlayerMessage::Type iter(Form& form, const GameData& game_data);
}