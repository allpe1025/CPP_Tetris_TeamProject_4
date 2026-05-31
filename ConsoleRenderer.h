#pragma once
#include "Color.h"
#include "IRenderer.h"

class Board;
class Block;

// IRenderer 의 Windows 콘솔 구현.
// 모든 그리기는 (x, y) 절대 좌표에 gotoxy + std::cout 으로 수행된다.
class ConsoleRenderer : public IRenderer
{
private:
    int ab_x = 2;                       // 보드 좌상단의 화면 절대 좌표
    int ab_y = 1;

    void gotoxy(int x, int y);

public:
    void draw_board(const Board& board, int level) override;            // 보드 외곽 + 고정 블록
    void draw_block(const Block& block, int x, int y) override;         // 현재 조작 중인 블록
    void erase_block(const Block& block, int x, int y) override;        // 직전 위치 지우기 (블록 모양 기준)
    void draw_next_block(const Block& next, int level) override;        // 다음 블록 미리보기 (박스 색 = 다음 스테이지)
    void draw_stats(int level, int score, int lines_left) override;     // STAGE / SCORE / LINES 박스
    void animate_line_clear(int row) override;                          // 가득 찬 줄 □ 깜빡임 효과
    void draw_skill_status(bool q, bool w, bool e, int q_cnt, int w_cnt, int w_tgt) override;

    void draw_game_over() override;     // GAME OVER 박스
    void draw_logo() override;          // 타이틀 (타이핑 효과)
    void draw_logo_demo() override;     // 로고 아래 데모 블록 한 프레임
    void clear() override;              // 콘솔 전체 정리
};
