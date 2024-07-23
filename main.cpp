#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <array>

static const auto WINDOW_SIZE = sf::VideoMode(1280, 720);
static constexpr uint32_t FRAME_RATE = 60;
static constexpr uint32_t CELLS_SIDE_COUNT = 10;
static constexpr uint32_t BOARD_SIZE = CELLS_SIDE_COUNT * CELLS_SIDE_COUNT;
static const auto CELL_SIZE = sf::Vector2f{
    static_cast<float>(WINDOW_SIZE.width) / CELLS_SIDE_COUNT,
    static_cast<float>(WINDOW_SIZE.height) / CELLS_SIDE_COUNT
};

auto create_white_black_cells()
    -> std::array<sf::RectangleShape, BOARD_SIZE>
{
    auto is_white = true;
    auto cells = std::array<sf::RectangleShape, BOARD_SIZE>();
    for(uint32_t y = 0; y < CELLS_SIDE_COUNT; ++y) {
        for(uint32_t x = 0; x < CELLS_SIDE_COUNT; ++x) {
            auto& cell  = cells[y * CELLS_SIDE_COUNT + x];
            cell.setSize(CELL_SIZE);
            cell.setPosition(
                static_cast<float>(x) * CELL_SIZE.x,
                static_cast<float>(y) * CELL_SIZE.y
            );
            cell.setFillColor(is_white ? sf::Color::White : sf::Color::Black);
            is_white = not is_white;
        }
        is_white = not is_white;
    }
    return cells;
}

static const auto CELLS = create_white_black_cells();


// void handle_events(sf::RenderWindow& window) {
//     auto event = sf::Event{};
//     while(window.pollEvent(event)) {
//         ImGui::SFML::ProcessEvent(window, event);
//         switch(event.type) {
//             case sf::Event::Closed: {
//                 window.close();
//                 break;
//             }
//             default: {
//                 break;
//             }
//         }
//     }
// }

class ConceptCanDraw {
    public:
        ConceptCanDraw(sf::RenderWindow& window)
            : m_window{window} {}

        void draw(const sf::Drawable& drawable) const {
            m_window.draw(drawable);
        }

    private:
        sf::RenderWindow& m_window;
};

// void draw(const ConceptCanDraw window) {
//     for(const auto& cell : CELLS) {
//         window.draw(cell);
//     }
// }


namespace state_player_turn {



    void handle_events(sf::RenderWindow& window) {
        auto event = sf::Event{};
        while(window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            switch(event.type) {
                case sf::Event::Closed: {
                    window.close();
                    break;
                }
                default: {
                    break;
                }
            }
        }
    }

}

int main() {
    sf::RenderWindow window(WINDOW_SIZE, "mcts_checkers");
    window.setFramerateLimit(FRAME_RATE);

    if(not ImGui::SFML::Init(window)) {
        std::printf("Failed to init window\n");
        std::terminate();
    }

    while(window.isOpen()) {
        // handle_events(window);

        window.clear();
        // draw(window);

        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
