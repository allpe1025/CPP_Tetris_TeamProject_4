#pragma once

// 렌더링 백엔드 인터페이스.
// Game 은 IRenderer 만 알면 되고, 구체 구현 (ConsoleRenderer 등) 은 DI 로 주입된다.
class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual void draw_board(const class Board& board, int level) = 0;       // 보드 외곽 + 고정 블록
    virtual void draw_block(const class Block& block, int x, int y) = 0;    // 현재 조작 중인 블록
    virtual void erase_block(const class Block& block, int x, int y) = 0;   // 직전 위치 잔상 지우기
    virtual void draw_next_block(const class Block& next, int level) = 0;   // 다음 블록 미리보기 (박스 색은 다음 스테이지 색)
    virtual void draw_stats(int level, int score, int lines_left) = 0;      // STAGE / SCORE / LINES
    virtual void animate_line_clear(int row) = 0;                           // 줄 삭제 시 깜빡임 효과
	virtual void draw_hold_block(const class Block* hold, int level) = 0;          // 홀드 블록 (박스 + 블록)
	virtual void draw_ghost_block(const class Block& ghost, int x, int y) = 0;          // 블록 현재 위치 보여주는 고스트 블록

    virtual void draw_game_over() = 0;          // 게임 종료 박스
    virtual void draw_logo()      = 0;          // 시작 시 타이틀
    virtual void draw_logo_demo() = 0;          // 로고 아래 데모 블록 한 프레임 (랜덤 블록 4개)
    virtual void clear()          = 0;          // 콘솔 전체 정리
};
