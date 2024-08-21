#include <mcts_checkers/board/form.hpp>
#include <mcts_checkers/utils.hpp>

namespace mcts_checkers::board {

    namespace {

        constexpr ImVec2 convert_board_vector_to_imvec(const BoardVector board_index) {
            return ImVec2{
                static_cast<float>(board_index.x),
                static_cast<float>(board_index.y)
            };
        }

        constexpr BoardVector convert_imvec_to_board_vector(const ImVec2 imvec) {
            return BoardVector{
                static_cast<uint8_t>(imvec.x),
                static_cast<uint8_t>(imvec.y)
            };
        }

        void draw_rects() {
            const auto draw_list = ImGui::GetWindowDrawList();
            const auto cell_size = calc_cell_size();
            auto is_white = true;
            for(uint8_t y = 0; y < CELLS_PER_SIDE; ++y) {
                for(uint8_t x = 0; x < CELLS_PER_SIDE; ++x) {
                    const auto p_min = calc_cell_top_left(BoardVector{x, y});
                    draw_list->AddRectFilled(
                        p_min, p_min + cell_size, is_white ? BOARD_CELL_ONE_COLOR : BOARD_CELL_TWO_COLOR
                    );
                    is_white = not is_white;
                }
                is_white = not is_white;
            }
        }

        void draw_checkers(const GameData& data) {
            const auto draw_list = ImGui::GetWindowDrawList();
            const auto cell_size = calc_cell_size();
            const auto half_cell_size = cell_size / 2;
            constexpr auto radius_modifier = 0.8f;
            const auto pawn_radius = half_cell_size * radius_modifier;
            const auto king_hat_radius = half_cell_size / 2;
            auto checker_index = CheckerIndex{0};
            for(uint8_t y = 0; y < CELLS_PER_SIDE; ++y) {
                for(uint8_t x = y % 2 == 0 ? 1 : 0; x < CELLS_PER_SIDE; x += 2, ++checker_index) {
                    if(not data.checkers.m_is_in_place[checker_index]) continue;
                    const auto center = calc_cell_top_left(BoardVector{x, y}) + half_cell_size;
                    const auto player_index = data.checkers.m_player_index[checker_index];
                    draw_list->AddEllipseFilled(center, pawn_radius, player_index ? PLAYER_ONE_PAWN_COLOR : PLAYER_TWO_PAWN_COLOR);
                    if(data.checkers.m_is_king[checker_index]) {
                        draw_list->AddEllipseFilled(center, king_hat_radius, KING_HAT_COLOR);
                    }
                }
            }
        }
        constexpr std::array<detail::State(*)(const GameData&), 2> STATE_FACTORIES = {
            [](const GameData&) -> detail::State { return human::Form{}; },
            // [](const GameData& game_data) -> detail::State { return ai::Form{game_data}; }
            [](const GameData&) -> detail::State { return human::Form{}; },
        };
    }

    Form::Form()
        : m_state{STATE_FACTORIES[static_cast<uint8_t>(m_game_data.m_current_player_index)](m_game_data)} {}

    OutMessage::Type iter_out(Form& form) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("BoardForm", ImVec2(0, -1), true, ImGuiWindowFlags_NoScrollWithMouse);
        ON_SCOPE_EXIT {
            ImGui::EndChild();
            ImGui::PopStyleVar();
        };

        draw_rects();
        draw_checkers(form.m_game_data);

        const auto player_message = std::visit(
            [&game_data = std::as_const(form.m_game_data)]
            (auto& state) -> PlayerMessage::Type {
                return iter(state, game_data);
            }, form.m_state
        );

        // struct PlayerMadeNoSelection {};
        // struct PlayerMadeSelection { GameData m_game_data; };
        // struct DeclareLoss { PlayerIndex m_index; };
        // struct DeclareDraw {};

        std::visit(utils::overloaded{
            [](const PlayerMessage::PlayerMadeNoSelection message) -> OutMessage::Type {
                return OutMessage::MakingDecision{};
            },
            [&form](const PlayerMessage::PlayerMadeSelection message) -> OutMessage::Type {
                form.m_game_data = message.m_game_data;
                form.m_state = STATE_FACTORIES[static_cast<uint8_t>(form.m_game_data.m_current_player_index)](form.m_game_data);
                return OutMessage::MakingDecision{};
            },
            [](const PlayerMessage::DeclareLoss message) -> OutMessage::Type {
                return OutMessage::DeclareWin{opposite_player(message.m_index)};
            },
            [](const PlayerMessage::DeclareDraw) -> OutMessage::Type {
                return OutMessage::DeclareDraw{};
            }
        }, player_message);

        // return std::visit(utils::overloaded{
        //     [&game_data=std::as_const(form.m_game_data)](const PlayerMadeNoSelection) -> OutMessage {
        //         return MakingDecision{game_data.m_current_player_index};
        //     },
        //     [](const turn_actions::DeclareLoss action) -> OutMessage {
        //         return DeclareWin{opposite_player(action.m_player_index)};
        //     },
        //     [&game_data=std::as_const(form.m_game_data)](const turn_actions::DeclareDraw) -> OutMessage {
        //         return DeclareDraw{};
        //     },
        //     [&form](const selection_confirmed::Move& action) -> OutMessage {
        //         apply_move(form.m_game_data, action.checker_index, action.data);
        //         form.m_state = STATE_FACTORIES[static_cast<uint8_t>(form.m_game_data.m_current_player_index)](form.m_game_data);
        //         return MakingDecision{form.m_game_data.m_current_player_index};
        //     },
        //     [&form](const selection_confirmed::Attack& action) -> OutMessage {
        //         apply_attack(form.m_game_data, action.data);
        //         form.m_state = STATE_FACTORIES[static_cast<uint8_t>(form.m_game_data.m_current_player_index)](form.m_game_data);
        //         return MakingDecision{form.m_game_data.m_current_player_index};
        //     }
        // }, action);

        return OutMessage::Type{};
    }

}