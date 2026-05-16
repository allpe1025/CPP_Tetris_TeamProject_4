#pragma once
#include "Color.h"
#include "IRenderer.h"

class Board;
class Block;

class ConsoleRenderer : public IRenderer
{
private:
	int ab_x = 2;
	int ab_y = 1;

    void gotoxy(int x, int y);
public:
    void draw_board(const Board& board, int level) override;        // 게임 보드 테두리와 누적된 블록들을 화면에 출력
    void draw_block(const Block& block, int x, int y) override;     //현재 조작 중인 블록을 지정된 보드 좌표(x, y)에 출력
    void erase_block(const Block& block, int x, int y) override;    // 블록이 이동할 때 이전 위치 잔상 지움
    void draw_next_block(const class Block& next) override;         // 다음에 등장할 블록을 출력
    void draw_stats(int level, int score, int lines_left) override; // STAGE, SCORE LINES 정보를 출력
    void draw_game_over() override;                                 //게임 종료 시 GAME OVER 출력
    void draw_logo() override;                                      //게임 시작 시 타이틀 화면 출력
    void clear() override;                                          //콘솔 창 전체를 완전히 지움
};