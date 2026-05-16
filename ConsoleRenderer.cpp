#include "ConsoleRenderer.h"
#include "Block.h"
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

    for (int i = 0; i < 21; i++) {              //board 21행 14열
        for (int j = 0; j < 14; j++) {
            gotoxy(j * 2 + ab_x, i + ab_y);
            if (j == 0 || j == 13 || i == 20) {
                ColorUtility::apply(wall_color);
                cout << "■";
            }
            else if (/*Board.get_Cell(j, i) != 0*/ false) {
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
    //auto shape_data = block.get_shape();    //auto를 반환하는 함수를 정의되기 전에 사용할 수 없습니다
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
    //auto shape_data = block.get_shape();    
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
    //auto shape_data = next.get_shape();

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