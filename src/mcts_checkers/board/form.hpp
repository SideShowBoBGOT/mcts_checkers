#pragma once

#include <variant>
#include <mcts_checkers/checkers_data.hpp>
#include <mcts_checkers/board/human/form.hpp>
#include <mcts_checkers/board/ai/form.hpp>


namespace mcts_checkers::board {

    namespace detail {
        using State = std::variant<human::Form, ai::Form>;
    }

    struct Form {
        Form();
        GameData m_game_data{};
        detail::State m_state{};
    };

    namespace OutMessage {
        struct DeclareWin { PlayerIndex m_player_index; };
        struct DeclareDraw {};
        struct MakingDecision { PlayerIndex m_player_index; };

        using Type = std::variant<DeclareDraw, DeclareWin, MakingDecision>;
    }

    OutMessage::Type iter_out(Form& form);
}