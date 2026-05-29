#pragma once
#include <memory>
#include "Board.h"
#include "Block.h"
#include "StageManager.h"
#include "IRenderer.h"
#include "InputManager.h"

// 누적 점수 / 줄 삭제 통계. UI 표기와 레벨업 판정에 함께 사용된다.
struct GameStats
{
    int score            = 0;
    int total_cleared    = 0;       // 게임 시작 이후 전체 누적 라인 수
    int cleared_in_stage = 0;       // 현재 스테이지 내에서 지운 라인 수 (레벨업 카운터)
};

// 전체 게임 흐름을 제어하는 오케스트레이터.
// Board / StageManager / Block 은 직접 소유하고,
// Renderer / InputManager 는 참조로 주입받는다.
class Game
{
private:
    Board         board;
    StageManager  stage;
    GameStats     stats;
    std::unique_ptr<Block> current;     // 현재 조작 중인 블록
    std::unique_ptr<Block> next;        // 다음에 등장할 블록 (미리보기용)
	std::unique_ptr<Block> hold;        // 홀드한 블록
    IRenderer&    renderer;
    InputManager& input;
    bool          running      = false;
    int           gravity_tick = 0;     // 자동 낙하 카운터: stage.speed 이상이면 1칸 낙하
	bool          can_hold     = true;      // 홀드 사용 가능 여부 (착지 후에만 true로 리셋)

    // === 부분 갱신용 더티 플래그 ===
    // 데이터가 변할 때만 해당 영역을 다시 그린다 (매 프레임 전체 redraw 방지 → 깜빡임 감소).
    bool dirty_board = true;
    bool dirty_block = true;
    bool dirty_next  = true;
    bool dirty_stats = true;
	bool dirty_hold = true;
    std::unique_ptr<Block> last_drawn_block; // 직전에 그린 블록의 사본 (이전 위치/모양을 지우기 위해)

public:
    Game(IRenderer& r, InputManager& i);
    void run();                         // 메인 루프 진입점. 게임 오버까지 블로킹

private:
    void handle_input();                // 한 프레임의 입력 1회 폴링 후 분기 처리
    void tick();                        // 자동 낙하 1칸. 착지 시 on_block_landed 호출
    bool try_move(int dx, int dy);      // 이동 시도 → 충돌 시 롤백, 성공 여부 반환
    bool try_rotate();                  // 회전 시도 → 충돌 시 back_rotate로 롤백
    void hard_drop();                   // 한 번에 바닥까지 낙하
    void on_block_landed();             // merge → clear → 점수/레벨 갱신 → 다음 스폰
    void spawn_next_block();            // next → current 승격 후 새 next 생성
    void render_frame();                // 한 프레임 화면 출력 (더티 플래그 기반 부분 갱신)
    bool any_dirty() const;             // 그릴 게 있는지

    int  ask_start_level();             // 시작 시 1~8 레벨 선택을 사용자에게 받음 (원본 input_data 역할)
    void reset_state(int start_level);  // 새 판 시작용 — 보드/통계/블록 모두 초기화
    void wait_any_key();                // 콘솔에서 아무 키나 눌릴 때까지 블로킹
    void wait_for_logo_key();           // 로고 화면용: 데모 블록 깜빡임 + 키 대기
	void hold_block();                  // 홀드 → 현재 블록과 홀드 블록 교체. 홀드가 비어있으면 현재 블록을 홀드로 보냄
};
