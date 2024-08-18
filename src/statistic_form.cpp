#include <mcts_checkers/statistic_form.hpp>
#include <imgui.h>

#include <mcts_checkers/utils.hpp>

namespace mcts_checkers::statistic {

    static constexpr auto FORM_WIDTH = 400.f;

    Message iter_out(const Form& stat) {
        ImGui::BeginChild("StatisticForm", ImVec2(FORM_WIDTH, -1), true);
        ON_SCOPE_EXIT {
            ImGui::EndChild();
        };

        ImGui::NewLine();

        if(ImGui::Button("Replay", ImVec2(70, 30))) {
            return ReplayGame{};
        }

        return Nothing{};
    }

}
