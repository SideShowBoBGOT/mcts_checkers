#include <mcts_checkers/statistic_form.hpp>
#include <mcts_checkers/utils.hpp>


#include <fmt/format.h>

namespace mcts_checkers::statistic {

    static constexpr auto FORM_WIDTH = 400.f;

    static constexpr std::string_view detrmine_status_message(const InputMessage input_message) {
        return std::visit(utils::overloaded{
            [](const ShowNothing) {
                return "";
            },
            [](const ShowWin message) {
                return message.m_player_index == PlayerIndex::FIRST ?
                    "First player won!" : "Second player won!";
            },
            [](const ShowMakingDecision message) {
                return message.m_player_index == PlayerIndex::FIRST ?
                    "First player is making decision" : "Second player is making decision!";
            },
            [](const ShowDraw) {
                return "Draw";
            }
            }, input_message
        );
    }

    Message iter_out(const Form&, InputMessage input_message) {
        ImGui::BeginChild("StatisticForm", ImVec2(FORM_WIDTH, -1), true);
        ON_SCOPE_EXIT {
            ImGui::EndChild();
        };

        ImGui::Text("%s", detrmine_status_message(input_message).data());

        ImGui::NewLine();

        if(ImGui::Button("Replay", ImVec2(70, 30))) {
            return ReplayGame{};
        }

        return Nothing{};
    }

}
