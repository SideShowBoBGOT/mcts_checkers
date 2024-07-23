#include <mcts_checkers/game/state.hpp>

int main() {
    auto game = mcts_checkers::game::State{};
    game.loop();
    return 0;
}
