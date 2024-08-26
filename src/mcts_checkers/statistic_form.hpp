#pragma once
#include <variant>
#include <mcts_checkers/checkers_data.hpp>

namespace mcts_checkers::statistic {

    struct Nothing {};
    struct ReplayGame {};

    using Message = std::variant<Nothing, ReplayGame>;

    struct Form {};

    struct ShowNothing {};
    struct ShowWin { PlayerIndex m_player_index; };
    struct ShowMakingDecision { PlayerIndex m_player_index; };
    struct ShowDraw {};

    using InputMessage = std::variant<
        ShowNothing, ShowWin, ShowMakingDecision, ShowDraw
    >;

    Message iter_out(const Form& stat, InputMessage input_message);

}