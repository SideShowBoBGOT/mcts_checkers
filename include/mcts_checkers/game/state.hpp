#pragma once
#include <memory>

namespace sf {
    class RenderWindow;
}

namespace mcts_checkers::game {

    class State {
        public:
            State();
            ~State();
            void loop();

        private:
            std::unique_ptr<sf::RenderWindow> m_window;
    };

}