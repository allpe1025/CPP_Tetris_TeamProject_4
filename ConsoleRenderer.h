#pragma once
#include <Windows.h>
#include "Color.h"
#include "IRenderer.h"

class Board;
class Block;
enum class GameMode;

// IRenderer 의 Windows 콘솔 구현.
// 모든 그리기는 (x, y) 절대 좌표에 gotoxy + std::cout 으로 수행된다.
class ConsoleRenderer : public IRenderer
{
private:
    int ab_x = 18;
    int ab_y = 1;

    // === 성능 캐시 ===
    HANDLE hConsole_;                       // GetStdHandle 결과 1회 캐싱 (gotoxy 마다 syscall 회피)
    Color  current_color_ = Color::WHITE;   // 직전 적용 색 — 동일 색이면 SetConsoleTextAttribute 스킵
    bool   color_cache_valid_ = false;      // clear() 직후엔 cache 무효
    int    last_next_box_level_ = -1;       // 다음 블록 박스가 그려진 레벨 (변경 감지용)
    int    last_hold_box_level_ = -1;       // 홀드 박스가 그려진 레벨
    bool   next_box_drawn_ = false;
    bool   hold_box_drawn_ = false;

    void gotoxy(int x, int y);
    void hide_cursor();
    void apply_color(Color c);              // 캐시 비교 후 필요 시에만 SetConsoleTextAttribute 호출
    void draw_next_box(int level);          // 다음 블록 박스 외곽 (레벨 변경 시 1회만)
    void draw_hold_box(int level);          // 홀드 박스 외곽 (레벨 변경 시 1회만)

public:
    ConsoleRenderer();

    void draw_board(const Board& board, int level, GameMode mode) override;            // 보드 외곽 + 고정 블록
    void draw_block(const Block& block, int x, int y) override;         // 현재 조작 중인 블록
    void erase_block(const Block& block, int x, int y) override;        // 직전 위치 지우기 (블록 모양 기준)
    void draw_next_block(const Block& next, int level) override;        // 다음 블록 미리보기
    void draw_stats(int level, int score, int lines_left) override;     // STAGE / SCORE / LINES 박스
    void animate_line_clear(int row) override;                          // 가득 찬 줄 □ 깜빡임 효과
    void draw_hold_block(const Block* hold, int level) override;        // 홀드 블록 (박스 + 블록)
    void draw_ghost_block(const Block& ghost, int x, int y) override;   // 고스트 블록 (현재 위치 보여주는 블록)
    void draw_skill_status(bool q, bool w, bool e, int q_cnt, int w_cnt, int w_tgt) override;   // 스킬 보유 여부 출력

    void draw_game_over() override;     // GAME OVER 박스
    void draw_logo() override;          // 타이틀 (타이핑 효과)
    void draw_logo_demo() override;     // 로고 아래 데모 블록 한 프레임
    void clear() override;              // 콘솔 전체 정리
};
