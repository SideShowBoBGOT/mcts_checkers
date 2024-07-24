#include <mcts_checkers/game/state.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <imgui-SFML.h>
#include <imgui.h>
#include <array>

namespace mcts_checkers {

    static constexpr uint32_t BOARD_CELLS_PER_SIDE_COUNT = 10;
    static constexpr uint32_t BOARD_CELLS_COUNT = BOARD_CELLS_PER_SIDE_COUNT * BOARD_CELLS_PER_SIDE_COUNT;

    // auto create_white_black_cells(const sf::Vector2f& viewport_pos, const float viewport_size)
    //     -> std::array<sf::RectangleShape, BOARD_CELLS_COUNT>
    // {
    //     const auto cell_side_size = static_cast<float>(viewport_size) / BOARD_CELLS_PER_SIDE_COUNT;
    //     const auto cell_size = sf::Vector2f{cell_side_size, cell_side_size};
    //     auto is_white = true;
    //     auto cells = std::array<sf::RectangleShape, BOARD_CELLS_COUNT>();
    //     for(uint32_t y = 0; y < BOARD_CELLS_PER_SIDE_COUNT; ++y) {
    //         for(uint32_t x = 0; x < BOARD_CELLS_PER_SIDE_COUNT; ++x) {
    //             auto& cell  = cells[y * BOARD_CELLS_PER_SIDE_COUNT + x];
    //             cell.setSize(cell_size);
    //             cell.setPosition(viewport_pos);
    //             cell.move(
    //                 static_cast<float>(x) * cell_size.x,
    //                 static_cast<float>(y) * cell_size.y
    //             );
    //             cell.setFillColor(is_white ? sf::Color::White : sf::Color::Black);
    //             is_white = not is_white;
    //         }
    //         is_white = not is_white;
    //     }
    //     return cells;
    // }
    //
    // class BoardViewport {
    //     public:
    //         explicit BoardViewport(const sf::Vector2f& position, const float size)
    //             : m_cells{create_white_black_cells(position, size)} {}
    //
    //         void draw(sf::RenderWindow& window) const {
    //             for(const auto& cell : m_cells) {
    //                 window.draw(cell);
    //             }
    //         }
    //
    //     std::array<sf::RectangleShape, BOARD_CELLS_COUNT> m_cells;
    // };

    void draw(const ImVec2& window_size) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Two-Pane Layout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

            // Calculate the width for each pane (minus some padding)
            ImGui::BeginChild("LeftPane", ImVec2(200, -1), true);
                std::printf("(%f, %f)\n", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
                const ImVec2 leftPanePos = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(
                    leftPanePos,
                    ImVec2(leftPanePos.x + 10, leftPanePos.y + 100),
                    ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 1.0f, 1.0f))
                );
            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("RightPane", ImVec2(0, -1), true);  // 0 width means "use remaining width"
                const ImVec2 rightPanePos = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(
                    rightPanePos,
                    ImVec2(rightPanePos.x + 100, rightPanePos.y + 100),
                    ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f))
                );
            ImGui::EndChild();

        ImGui::End();
    }


    static const auto WINDOW_SIZE = sf::VideoMode(1280, 720);
    static constexpr uint32_t FRAME_RATE = 60;

    void loop() {
        auto window = sf::RenderWindow(WINDOW_SIZE, "mcts_checkers");
        window.setFramerateLimit(FRAME_RATE);

        if(not ImGui::SFML::Init(window)) {
            std::printf("Failed to init window\n");
            std::terminate();
        }

        sf::Clock deltaClock;

        while(window.isOpen()) {
            ImGui::SFML::Update(window, deltaClock.restart());

            window.clear();
            draw(ImVec2(window.getSize().x, window.getSize().y));
            ImGui::SFML::Render(window);
            window.display();
        }


        ImGui::SFML::Shutdown();
    }



}