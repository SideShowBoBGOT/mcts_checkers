#include <mcts_checkers/main_form.hpp>
#include <imgui.h>

namespace mcts_checkers {

    MainForm::MainForm()=default;

    void MainForm::iter() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Two-Pane Layout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

        m_statistic_form.iter();

        ImGui::SameLine();

        m_board_form.iter(CheckersData());

        ImGui::End();
    }
}