#pragma once

#include <variant>
#include <mcts_checkers/board/utils.hpp>

namespace mcts_checkers::board::ai {

    struct Form {};

    PlayerMessage::Type iter(Form, const GameData& game_data);
}