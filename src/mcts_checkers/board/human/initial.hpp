#pragma once

#include <variant>

namespace mcts_checkers {
    struct GameData;
}

namespace mcts_checkers::board::human::initial {

    struct State {};
    namespace OutMessage {
        struct TransitionToUnselectedMove {};
        struct TransitionToUnselectedAttack {};
        struct DeclareLoss {};
        struct DeclareDraw {};

        using Type = std::variant<
            TransitionToUnselectedMove,
            TransitionToUnselectedAttack,
            DeclareLoss,
            DeclareDraw
        >;
    }

    void iter(State, const GameData& game_data);

}