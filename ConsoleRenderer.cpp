#include "ConsoleRenderer.h"
#include "Board.h"
#include "Block.h"
#include "Game.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

ConsoleRenderer::ConsoleRenderer()
    : hConsole_(GetStdHandle(STD_OUTPUT_HANDLE))
{
}

void ConsoleRenderer::gotoxy(int x, int y)
{
    // GetStdHandle 은 생성자에서 1회만 호출 → 매 호출 syscall 회피.
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(hConsole_, pos);
}

void ConsoleRenderer::hide_cursor()
{
    CONSOLE_CURSOR_INFO info = { 1, FALSE };
    SetConsoleCursorInfo(hConsole_, &info);
}

void ConsoleRenderer::apply_color(Color c)
{
    // 직전 적용 색과 같으면 SetConsoleTextAttribute 스킵.
    if (color_cache_valid_ && c == current_color_) return;
    ColorUtility::apply(c);
    current_color_ = c;
    color_cache_valid_ = true;
}

void ConsoleRenderer::draw_board(const Board& board, int level, GameMode mode)
{
    hide_cursor();
    Color wall_color = static_cast<Color>((level % 6) + 1);     // 벽 색: 레벨마다 6색을 순환

    for (int y = 0; y < Board::height; y++) {
        for (int x = 0; x < Board::width; x++) {
            Cell cell_type = board.get_cell(x, y);
            gotoxy(x * 2 + ab_x, y + ab_y);
            if (cell_type == Cell::Wall) {
                apply_color(wall_color);
                cout << "▨";
            }
            else if (cell_type == Cell::Fixed) {
                apply_color(Color::GRAY);
                if (mode == GameMode::HiddenStack) {
                    cout << "  ";       // 투명 모드일 때는 쌓인 블록을 공백으로!
                }
                else {
                    cout << "■";
                }
            }
            else {
                cout << "  ";                                   // Empty: 보드 내부 공백
            }
        }
    }
}

void ConsoleRenderer::draw_block(const Block& block, int x, int y)
{
    apply_color(static_cast<Color>(block.get_color()));
    auto shape_data = block.get_shape();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] != 1) continue;
            if (i + y < 0) continue;                            // 보드 위쪽 바깥은 그리지 않음 (스폰 시 누수 방지)
            gotoxy((j + x) * 2 + ab_x, (i + y) + ab_y);
            cout << block.get_display_text();
        }
    }
}

void ConsoleRenderer::erase_block(const Block& block, int x, int y)
{
    // 블록 셀이 있던 자리에 공백을 덮어쓴다. 보드(Fixed) 위를 지우는 일은 호출 측에서 막아야 함.
    auto shape_data = block.get_shape();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] != 1) continue;
            gotoxy((j + x) * 2 + ab_x, (i + y) + ab_y);
            cout << "  ";
        }
    }
}

void ConsoleRenderer::draw_next_box(int level)
{
    // 다음 블록 박스 외곽 (6×6의 outline 20셀)만 그림. 박스 색은 "다음 스테이지의 벽 색" (원본 동작 이식).
    Color box_color = static_cast<Color>((level + 1) % 6 + 1);
    apply_color(box_color);
    for (int i = 1; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            if (i == 1 || i == 6 || j == 0 || j == 5) {
                gotoxy((j + 16) * 2 + ab_x, i + ab_y);
                cout << "■";
            }
        }
    }
}

void ConsoleRenderer::draw_next_block(const Block& next, int level)
{
    // 박스 외곽은 레벨 변경 시 + 첫 호출 시에만 그림 → 매 호출마다 36셀 redraw 회피.
    if (!next_box_drawn_ || last_next_box_level_ != level) {
        draw_next_box(level);
        next_box_drawn_ = true;
        last_next_box_level_ = level;
    }

    // 박스 안 4×4 영역 클리어 (이전 블록 흔적 제거)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy((j + 17) * 2 + ab_x, (i + 2) + ab_y);
            cout << "  ";
        }
    }

    // 새 블록 그리기 (회전 전 0 기준 각도)
    apply_color(static_cast<Color>(next.get_color()));
    auto shape_data = next.get_shape();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] != 1) continue;
            gotoxy((j + 17) * 2 + ab_x, (i + 2) + ab_y);
            cout << next.get_display_text();
        }
    }
}

void ConsoleRenderer::draw_stats(int level, int score, int lines_left)
{
    // 원본 show_gamestat 의 레이아웃 이식: 라벨과 값을 별도 줄에 배치 (박스 없음)
    apply_color(Color::GRAY);

    gotoxy(35 + ab_x, 7  + ab_y); cout << "STAGE";
    gotoxy(41 + ab_x, 7  + ab_y); cout << level;

    gotoxy(35 + ab_x, 9  + ab_y); cout << "SCORE";
    gotoxy(35 + ab_x, 10 + ab_y); cout << setw(10) << score;

    gotoxy(35 + ab_x, 12 + ab_y); cout << "LINES";
    gotoxy(35 + ab_x, 13 + ab_y); cout << setw(10) << lines_left;

    apply_color(Color::WHITE);
}

void ConsoleRenderer::animate_line_clear(int row)
{
    // 원본 check_full_line 의 시각효과: 파란 □ 로 한 칸씩 채웠다가 → 다시 비움
    apply_color(Color::BLUE);
    for (int j = Board::left_wall + 1; j < Board::right_wall; j++) {
        gotoxy(j * 2 + ab_x, row + ab_y);
        cout << "□";
        Sleep(10);
    }
    for (int j = Board::left_wall + 1; j < Board::right_wall; j++) {
        gotoxy(j * 2 + ab_x, row + ab_y);
        cout << "  ";
        Sleep(10);
    }
}

// 디버깅용
void ConsoleRenderer::draw_skill_status(bool q, bool w, bool e, int q_cnt, int w_cnt, int w_tgt)
{
    int debug_x = 32 + ab_x;
    int debug_y = 15 + ab_y;

    gotoxy(debug_x, debug_y + 1);
    std::cout << "Q : " << (q ? "O" : "X") << " (" << q_cnt << "/36)  ";

    gotoxy(debug_x, debug_y + 2);
    std::cout << "W (Type " << w_tgt << ") : " << (w ? "O" : "X") << " (" << w_cnt << "/5)   ";

    gotoxy(debug_x, debug_y + 3);
    std::cout << "E : " << (e ? "O" : "X");
}

void ConsoleRenderer::draw_game_over()
{
    apply_color(Color::RED);
    gotoxy(15 + ab_x, 8  + ab_y); cout << "┏━━━━━━━━━━━━━┓";
    gotoxy(15 + ab_x, 9  + ab_y); cout << "┃  GAME OVER  ┃";
    gotoxy(15 + ab_x, 10 + ab_y); cout << "┗━━━━━━━━━━━━━┛";
}

void ConsoleRenderer::draw_logo()
{
    // 원본 show_logo 의 6줄 TETRIS 글자 + "Please Press Any Key" 메시지 이식.
    apply_color(Color::YELLOW);
    int logo_x = 13 + ab_x;
    int logo_y = 3  + ab_y;
    const char* frames[] = {
        "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓",
        "┃  ◆ ◆ ◆   ◆ ◆ ◆   ◆ ◆ ◆   ◆ ◆ ◆    ◆   ◆ ◆ ◆   ┃",
        "┃    ◆     ◆         ◆     ◆   ◆    ◆   ◆       ┃",
        "┃    ◆     ◆ ◆ ◆     ◆     ◆ ◆      ◆     ◆     ┃",
        "┃    ◆     ◆         ◆     ◆  ◆     ◆       ◆   ┃",
        "┃    ◆     ◆ ◆ ◆     ◆     ◆    ◆   ◆   ◆ ◆ ◆   ┃",
        "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛"
    };
    for (const char* line : frames) {           // 한 줄씩 타이핑 효과
        gotoxy(logo_x, logo_y++);
        cout << line;
        Sleep(100);
    }

    gotoxy(28 + ab_x, 20 + ab_y);
    cout << "Please Press Any Key~!";
    apply_color(Color::WHITE);
}

void ConsoleRenderer::draw_logo_demo()
{
    // 원본 show_logo 의 데모 블록 효과: 로고 아래 영역을 지우고 무작위 블록 4개를 그림.
    // 호출 측(Game::wait_for_logo_key)이 주기적으로 호출 → 깜빡이며 바뀌는 인상.

    // 1) 데모 영역 5줄 지우기 (원본은 y=14~18 부근에 그렸음)
    for (int j = 0; j < 5; j++) {
        gotoxy(17 + ab_x, 14 + j + ab_y);
        cout << "                                                          ";
    }

    // 2) 무작위 4개 블록 — 모양/각도/x 위치 모두 랜덤, y는 고정
    const int demo_xs[4] = { 6, 12, 19, 24 };
    for (int x : demo_xs) {
        Block demo(rand() % 7, rand() % 4, x, 14);
        draw_block(demo, demo.get_x(), demo.get_y());
    }
}

void ConsoleRenderer::clear()
{
    system("cls");
    // cls 후 Windows 콘솔 상태가 리셋되므로 우리 캐시도 무효화.
    color_cache_valid_ = false;
    next_box_drawn_ = false;
    hold_box_drawn_ = false;
    last_next_box_level_ = -1;
    last_hold_box_level_ = -1;
    hide_cursor();      // cls 가 커서를 다시 보이게 할 수 있어 안전상 다시 숨김
}

void ConsoleRenderer::draw_hold_box(int level)
{
    // 홀드 박스 외곽 (6×6 outline) 만 그림.
    const int box_x = -7;
    const int box_y = 1;
    const int box_width = 6;
    const int box_height = 6;

    // 제목 출력
    apply_color(Color::GRAY);
    gotoxy(box_x * 2 + ab_x, (box_y - 1) + ab_y);
    cout << "HOLD";

    // 박스 외곽만 그림 (안쪽 공백은 draw_hold_block 에서 매번 클리어)
    Color box_color = static_cast<Color>((level % 6) + 1);
    apply_color(box_color);
    for (int i = 0; i < box_height; i++) {
        for (int j = 0; j < box_width; j++) {
            if (i == 0 || i == box_height - 1 || j == 0 || j == box_width - 1) {
                gotoxy((j + box_x) * 2 + ab_x, (i + box_y) + ab_y);
                cout << "■";
            }
        }
    }
}

void ConsoleRenderer::draw_hold_block(const Block* hold, int level)
{
    const int box_x = -7;
    const int box_y = 1;
    const int inner_x = box_x + 1;          // 안쪽 4×4 시작 좌표
    const int inner_y = box_y + 1;

    // 박스 외곽은 레벨 변경 시 + 첫 호출 시에만 그림.
    if (!hold_box_drawn_ || last_hold_box_level_ != level) {
        draw_hold_box(level);
        hold_box_drawn_ = true;
        last_hold_box_level_ = level;
    }

    // 안쪽 4×4 클리어 (이전 hold 블록 흔적 제거)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gotoxy((j + inner_x) * 2 + ab_x, (i + inner_y) + ab_y);
            cout << "  ";
        }
    }

    if (hold == nullptr) {
        apply_color(Color::WHITE);
        return;
    }

    apply_color(static_cast<Color>(hold->get_color()));
    auto shape_data = hold->get_shape();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] != 1) continue;
            gotoxy((j + inner_x) * 2 + ab_x, (i + inner_y) + ab_y);
            cout << hold->get_display_text();
        }
    }
    apply_color(Color::WHITE);
}

void ConsoleRenderer::draw_ghost_block(const Block& ghost, int x, int y)
{
    apply_color(Color::GRAY);
    auto shape_data = ghost.get_shape();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] != 1) continue;
            if (i + y < 0) continue;                            // 보드 위쪽 바깥은 그리지 않음 (스폰 시 누수 방지)
            gotoxy((j + x) * 2 + ab_x, (i + y) + ab_y);
            cout << "□";
        }
    }
}
