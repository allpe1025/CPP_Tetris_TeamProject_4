#include "ConsoleRenderer.h"
#include<iostream>
#include <iomanip>

using namespace std;
void ConsoleRenderer::gotoxy(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(hConsole, pos);
}
void ConsoleRenderer::draw_board(const Board& board, int level){
    Color wall_color = static_cast<Color>((level % 6) + 1);
    Color fixed_block_color = Color::GRAY;
   
    for (int y = 0; y < Board::height; y++) {
        for (int x = 0; x < Board::width; x++) {
            Cell cell_type = board.get_cell(x, y);
            gotoxy(x * 2 + ab_x, y + ab_y);
            if (cell_type == Cell::Wall ) {
                ColorUtility::apply(wall_color);
                cout << "▨";
            }
            else if (cell_type == Cell::Fixed) {
                ColorUtility::apply(Color::GRAY);
                cout << "■";
            }
            else {
                cout << "  ";
            }
        }
    }
}
void ConsoleRenderer::draw_block(const Block& block, int x, int y){
    ColorUtility::apply(static_cast<Color>(block.get_color()));
    auto shape_data = block.get_shape();    //get_shape함수를 Block.h 파일에 body에 구현 필요
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] == 1) {
                gotoxy((j + x) * 2 + ab_x, (i + y) + ab_y);
                cout << block.get_display_text();
            }
        }
    }
}
void ConsoleRenderer::erase_block(const Block& block, int x, int y){
    auto shape_data = block.get_shape();    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] == 1) {
                gotoxy((j + x) * 2 + ab_x, (i + y) + ab_y);
                cout << "  ";
            }
        }
    }
}
void ConsoleRenderer::draw_next_block(const class Block& next){
    Color box_color = Color::SKY_BLUE;
    ColorUtility::apply(box_color);
    for (int i = 1; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            if (i == 1 || i == 6 || j == 0 || j == 5) {
                gotoxy((j + 16) * 2 + ab_x, i + ab_y);
                std::cout << "■";
            }
            else {
                gotoxy((j + 16) * 2 + ab_x, i + ab_y);
                std::cout << "  ";
            }
        }
    }
    ColorUtility::apply(static_cast<Color>(next.get_color()));
    auto shape_data = next.get_shape();

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (shape_data[i][j] == 1) {
                
                gotoxy((j + 17) * 2 + ab_x, (i + 2) + ab_y);
                std::cout << "■";
            }
        }
    }
}
void ConsoleRenderer::draw_stats(int level, int score, int lines_left){
    ColorUtility::apply(Color::GRAY);

    // 2. STAGE 정보 출력
    gotoxy(35 + ab_x, 8 + ab_y);
    std::cout << "┏━━━━━━━━━━━━━━━━━━┓";
    gotoxy(35 + ab_x, 9 + ab_y);
    std::cout << "┃  STAGE : " << setw(8) << level << "  ┃"; 

    // 3. SCORE 정보 출력
    gotoxy(35 + ab_x, 10 + ab_y);
    std::cout << "┃  SCORE : " << setw(8) << score << "  ┃";

    // 4. LINES 정보 출력
    gotoxy(35 + ab_x, 11 + ab_y);
    std::cout << "┃  LINES : " << setw(8) << lines_left << "  ┃";
    gotoxy(35 + ab_x, 12 + ab_y);
    std::cout << "┗━━━━━━━━━━━━━━━━━━┛";

    ColorUtility::apply(Color::WHITE);
}
void ConsoleRenderer::draw_game_over(){
    ColorUtility::apply(Color::RED); 
    gotoxy(15 + ab_x, 8 + ab_y);
    std::cout << "┏━━━━━━━━━━━━━┓";
    gotoxy(15 + ab_x, 9 + ab_y);
    std::cout << "┃  GAME OVER  ┃";
    gotoxy(15 + ab_x, 10 + ab_y);
    std::cout << "┗━━━━━━━━━━━━━┛";
}
void ConsoleRenderer::draw_logo() {
    ColorUtility::apply(Color::YELLOW);
    int logo_x = 13 + ab_x, logo_y = 3 + ab_y;
    const char* frames[] = {
        "┏━━━━━━━━━━━━━━━━━━━━━━━┓",
        "┃◆◆◆ ◆◆◆ ◆◆◆ ◆◆  ◆ ◆◆◆ ┃",
        "┃  ◆   ◆     ◆   ◆  ◆ ◆ ◆     ┃",
        "┗━━━━━━━━━━━━━━━━━━━━━━━┛"
    };
    for (const char* line : frames) {
        gotoxy(logo_x, logo_y++);
        std::cout << line;
        Sleep(100); 
    }
    ColorUtility::apply(Color::WHITE);
}
void ConsoleRenderer::clear(){
    system("cls");
}