#include <mcts_checkers/main_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <imgui.h>

namespace mcts_checkers {

    MainForm::MainForm()=default;

    void MainForm::iter() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Two-Pane Layout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

        std::visit(utils::overloaded{
                [](const statistic::Nothing) {},
                [&board_form=m_board_form](const statistic::ReplayGame) {
                    board_form = board::Form{};
                }
            },
            statistic::iter_out(m_statistic_form)
        );

        ImGui::SameLine();

        board::iter_out(m_board_form);

        ImGui::End();
    }
}