#include <mcts_checkers/statistic_form.hpp>
#include <imgui.h>

namespace mcts_checkers {

    static constexpr auto FORM_WIDTH = 400.f;

    StatisticForm::StatisticForm()=default;

    void StatisticForm::iter() {
        ImGui::BeginChild("StatisticForm", ImVec2(FORM_WIDTH, -1), true);
        ImGui::EndChild();
    }

}