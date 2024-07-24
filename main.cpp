#include <mcts_checkers/main_form.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <imgui-SFML.h>

static const auto WINDOW_SIZE = sf::VideoMode(1280, 720);
static constexpr uint32_t FRAME_RATE = 60;

int main() {
    auto window = sf::RenderWindow(WINDOW_SIZE, "mcts_checkers");
    window.setFramerateLimit(FRAME_RATE);

    if(not ImGui::SFML::Init(window)) {
        std::printf("Failed to init window\n");
        std::terminate();
    }

    auto deltaClock = sf::Clock{};
    auto main_form = mcts_checkers::MainForm{};

    while(window.isOpen()) {
        auto event = sf::Event{};
        while(window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if(event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear();
        main_form.iter();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
