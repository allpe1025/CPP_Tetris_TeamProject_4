#pragma once
enum class GameMode;

// ?�더�?백엔???�터?�이??
// Game ?� IRenderer �??�면 ?�고, 구체 구현 (ConsoleRenderer ?? ?� DI �?주입?�다.
class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual void draw_board(const class Board& board, int level, GameMode mode) = 0;       // 보드 ?�곽 + 고정 블록
    virtual void draw_block(const class Block& block, int x, int y) = 0;    // ?�재 조작 중인 블록
    virtual void erase_block(const class Block& block, int x, int y) = 0;   // 직전 ?�치 ?�상 지?�기
    virtual void draw_next_block(const class Block& next, int level) = 0;   // ?�음 블록 미리보기 (박스 ?��? ?�음 ?�테?��? ??
    virtual void draw_stats(int level, int score, int lines_left) = 0;      // STAGE / SCORE / LINES
    virtual void animate_line_clear(int row) = 0;                           // �???�� ??깜빡???�과
    virtual void draw_hold_block(const class Block* hold, int level) = 0;          // ?�??블록 (박스 + 블록)
    virtual void draw_ghost_block(const class Block& ghost, int x, int y) = 0;          // 블록 ?�재 ?�치 보여주는 고스??블록
    virtual void draw_skill_status(bool q, bool w, bool e, int q_cnt, int w_cnt, int w_tgt) = 0;    // ?�킬 보유 ?��? 출력
    virtual void draw_mode_description(GameMode mode) = 0;

    virtual void draw_game_over() = 0;          // 게임 종료 박스
    virtual void draw_logo()      = 0;          // ?�작 ???�?��?
    virtual void draw_logo_demo() = 0;          // 로고 ?�래 ?�모 블록 ???�레??(?�덤 블록 4�?
    virtual void clear()          = 0;          // 콘솔 ?�체 ?�리
};
