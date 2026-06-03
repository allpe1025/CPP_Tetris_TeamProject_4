#pragma once
#include <Windows.h>
#include "Color.h"
#include "IRenderer.h"

class Board;
class Block;
enum class GameMode;

class ConsoleRenderer : public IRenderer
{
private:
    int ab_x = 45;                         // 보드의 콘솔 기준 x 좌표
    int ab_y = 1;                          // 보드의 콘솔 기준 y 좌표

    // === 렌더링 캐시 ===
    HANDLE hConsole_;                      // 콘솔 핸들 캐시, GetStdHandle 반복 호출 방지
    Color  current_color_ = Color::WHITE;  // 현재 적용된 콘솔 색상
    bool   color_cache_valid_ = false;     // clear() 이후 색상 캐시 무효화 여부
    int    last_next_box_level_ = -1;      // 마지막으로 그린 next 박스의 레벨
    int    last_hold_box_level_ = -1;      // 마지막으로 그린 hold 박스의 레벨
    bool   next_box_drawn_ = false;        // next 박스가 이미 그려졌는지 여부
    bool   hold_box_drawn_ = false;        // hold 박스가 이미 그려졌는지 여부

    void gotoxy(int x, int y);             // 콘솔 커서를 지정한 좌표로 이동
    void hide_cursor();                    // 콘솔 커서 숨기기
    void apply_color(Color c);             // 필요한 경우에만 콘솔 색상 변경
    void draw_next_box(int level);         // next 블록 박스 테두리 그리기
    void draw_hold_box(int level);         // hold 블록 박스 테두리 그리기

public:
    ConsoleRenderer();

    void draw_board(const Board& board, int level, GameMode mode) override;            // 보드 외곽, 고정 블록, 게임 모드별 화면 요소 그리기
    void draw_block(const Block& block, int x, int y) override;                        // 현재 조작 중인 블록 그리기
    void erase_block(const Block& block, int x, int y) override;                       // 이전 위치의 블록 지우기
    void draw_next_block(const Block& next, int level) override;                       // 다음 블록 미리보기 그리기
    void draw_stats(int level, int score, int lines_left) override;                    // 스테이지, 점수, 남은 줄 수 출력
    void animate_line_clear(int row) override;                                         // 줄 삭제 애니메이션 출력
    void draw_hold_block(const Block* hold, int level) override;                       // 홀드 블록 박스와 홀드된 블록 그리기
    void draw_ghost_block(const Block& ghost, int x, int y) override;                  // 현재 블록이 떨어질 위치를 나타내는 고스트 블록 그리기
    void draw_skill_status(bool q, bool w, bool e, int q_cnt, int w_cnt, int w_tgt) override;   // 스킬 보유 상태와 사용 횟수 출력
    void draw_mode_description(GameMode mode) override;                         // 모드별 설명 출력

    void draw_game_over() override;     // 게임 오버 화면 출력
    void draw_logo() override;          // 시작 로고 화면 출력
    void draw_logo_demo() override;     // 로고 아래에 데모 블록 애니메이션 출력
    void clear() override;              // 콘솔 화면 전체 지우기
};