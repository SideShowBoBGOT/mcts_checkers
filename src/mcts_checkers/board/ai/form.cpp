#include <mcts_checkers/board/ai/form.hpp>

namespace mcts_checkers::board::ai {

    PlayerMessage::Type iter(Form, const GameData& game_data) {
        return PlayerMessage::DeclareDraw{};
    }

}