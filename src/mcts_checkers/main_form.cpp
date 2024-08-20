#include <mcts_checkers/main_form.hpp>
#include <mcts_checkers/utils.hpp>
#include <imgui.h>

namespace mcts_checkers {

    MainForm::MainForm()=default;

    static constexpr auto convert_board_output_message_to_statistic_input_message(
        const board::OutMessage out_message
    ) -> statistic::InputMessage {
        return std::visit(utils::overloaded{
            [](const board::DeclareDraw) -> statistic::InputMessage {
                return statistic::ShowDraw{};
            },
            [](const board::DeclareWin message) -> statistic::InputMessage {
                return statistic::ShowWin{message.m_player_index};
            },
            [](const board::MakingDecision message) -> statistic::InputMessage {
                return statistic::ShowMakingDecision{message.m_player_index};
            }
        }, out_message);
    }

    void MainForm::iter() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Two-Pane Layout", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
        ON_SCOPE_EXIT {
            ImGui::End();
        };

        std::visit(utils::overloaded{
                [](const statistic::Nothing) {},
                [&board_form=m_board_form](const statistic::ReplayGame) {
                    board_form = board::Form{};
                }
            },
            statistic::iter_out(m_statistic_form, m_statistic_input_message)
        );

        ImGui::SameLine();

        m_statistic_input_message = convert_board_output_message_to_statistic_input_message(
            board::iter_out(m_board_form)
        );
    }
}