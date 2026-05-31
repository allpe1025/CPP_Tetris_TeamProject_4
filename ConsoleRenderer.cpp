#include "ConsoleRenderer.h"
#include "Board.h"
#include "Block.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

void ConsoleRenderer::gotoxy(int x, int y)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(hConsole, pos);
}

void ConsoleRenderer::draw_board(const Board& board, int level)
{
    Color wall_color = static_cast<Color>((level % 6) + 1);     // 벽 색: 레벨마다 6색을 순환

    for (int y = 0; y < Board::height; y++) {
        for (int x = 0; x < Board::width; x++) {
            Cell cell_type = board.get_cell(x, y);
            gotoxy(x * 2 + ab_x, y + ab_y);
            if (cell_type == Cell::Wall) {
                ColorUtility::apply(wall_color);
                cout << "▨";
            }
            else if (cell_type == Cell::Fixed) {
                ColorUtility::apply(Color::GRAY);
                cout << "■";
            }
            else {
                cout << "  ";                                   // Empty: 보드 내부 공백
            }
        }
    }
}

void ConsoleRenderer::draw_block(const Block& block, int x, int y)
{
    ColorUtility::apply(static_cast<Color>(block.get_color()));
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

void ConsoleRenderer::draw_next_block(const Block& next, int level)
{
    // 1) 박스 외곽 — 색은 "다음 스테이지의 벽 색" (원본 동작 이식)
    Color box_color = static_cast<Color>((level + 1) % 6 + 1);
    ColorUtility::apply(box_color);
    for (int i = 1; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            gotoxy((j + 16) * 2 + ab_x, i + ab_y);
            if (i == 1 || i == 6 || j == 0 || j == 5) cout << "■";
            else                                      cout << "  ";
        }
    }

    // 2) 박스 안에 블록 (각도는 회전 전 0 기준 그대로)
    ColorUtility::apply(static_cast<Color>(next.get_color()));
    auto shape_data = next.get_shape();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] != 1) continue;
            gotoxy((j + 17) * 2 + ab_x, (i + 2) + ab_y);
            cout << next.get_display_text();        // 수정(전세하) : 다음 블록도 get_display_text()로 출력
        }
    }
}

void ConsoleRenderer::draw_stats(int level, int score, int lines_left)
{
    // 원본 show_gamestat 의 레이아웃 이식: 라벨과 값을 별도 줄에 배치 (박스 없음)
    ColorUtility::apply(Color::GRAY);

    gotoxy(35 + ab_x, 7  + ab_y); cout << "STAGE";
    gotoxy(41 + ab_x, 7  + ab_y); cout << level;

    gotoxy(35 + ab_x, 9  + ab_y); cout << "SCORE";
    gotoxy(35 + ab_x, 10 + ab_y); cout << setw(10) << score;

    gotoxy(35 + ab_x, 12 + ab_y); cout << "LINES";
    gotoxy(35 + ab_x, 13 + ab_y); cout << setw(10) << lines_left;

    ColorUtility::apply(Color::WHITE);
}

void ConsoleRenderer::animate_line_clear(int row)
{
    // 원본 check_full_line 의 시각효과: 파란 □ 로 한 칸씩 채웠다가 → 다시 비움
    ColorUtility::apply(Color::BLUE);
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

    gotoxy(debug_x, debug_y);

    gotoxy(debug_x, debug_y + 1);
    std::cout << "Q : " << (q ? "O" : "X") << " (" << q_cnt << "/36)  ";

    gotoxy(debug_x, debug_y + 2);
    std::cout << "W (Type " << w_tgt << ") : " << (w ? "O" : "X") << " (" << w_cnt << "/5)   ";

    gotoxy(debug_x, debug_y + 3);
    std::cout << "E : " << (e ? "O" : "X");    
}

void ConsoleRenderer::draw_game_over()
{
    ColorUtility::apply(Color::RED);
    gotoxy(15 + ab_x, 8  + ab_y); cout << "┏━━━━━━━━━━━━━┓";
    gotoxy(15 + ab_x, 9  + ab_y); cout << "┃  GAME OVER  ┃";
    gotoxy(15 + ab_x, 10 + ab_y); cout << "┗━━━━━━━━━━━━━┛";
}

void ConsoleRenderer::draw_logo()
{
    // 원본 show_logo 의 6줄 TETRIS 글자 + "Please Press Any Key" 메시지 이식.
    ColorUtility::apply(Color::YELLOW);
    int logo_x = 13 + ab_x;
    int logo_y = 3  + ab_y;
    const char* frames[] = {
        "┏━━━━━━━━━━━━━━━━━━━━━━━┓",
        "┃◆◆◆  ◆◆◆  ◆◆◆   ◆◆     ◆   ◆◆◆ ┃",
        "┃  ◆    ◆        ◆     ◆ ◆    ◆   ◆     ┃",
        "┃  ◆    ◆◆◆    ◆     ◆◆     ◆     ◆   ┃",
        "┃  ◆    ◆        ◆     ◆ ◆    ◆       ◆ ┃",
        "┃  ◆    ◆◆◆    ◆     ◆  ◆   ◆   ◆◆◆ ┃",
        "┗━━━━━━━━━━━━━━━━━━━━━━━┛"
    };
    for (const char* line : frames) {           // 한 줄씩 타이핑 효과
        gotoxy(logo_x, logo_y++);
        cout << line;
        Sleep(100);
    }

    gotoxy(28 + ab_x, 20 + ab_y);
    cout << "Please Press Any Key~!";
    ColorUtility::apply(Color::WHITE);
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
}
