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

// 게임 모드 (시작 시 사용자 선택). 1~5 입력값과 그대로 매핑된다.
// === 팀원 작업 위치 ===
//   Basic        — 본인(신정우). 기본 동작. 별도 분기 없음.
//   Inverted     — 좌우/회전 키 반대로. → Game::handle_input 분기.
//   HiddenStack  — 쌓인 블록을 숨김. → ConsoleRenderer::draw_board 분기.
//   SpecialBlock — 일정 확률로 특수 효과 블록 등장. → SpecialBlockGame 서브클래스
//                  (Game::spawn_next_block 오버라이드). 효과 적용은 Block::execute_effect.
//   Quest        — 줄 N개 클리어 시 스킬 보상. → SkillGame 서브클래스
//                  (Game::handle_input / on_block_landed 오버라이드).
enum class GameMode
{
    Basic        = 1,
    Inverted     = 2,
    HiddenStack  = 3,
    SpecialBlock = 4,
    Quest        = 5,
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
    GameMode      mode         = GameMode::Basic;   // ask_game_mode() 또는 외부 set_mode()로 결정
    bool          mode_preset  = false;             // main에서 set_mode를 통해 미리 정해졌는지
    bool          can_hold     = true;      // 홀드 사용 가능 여부 (착지 후에만 true로 리셋)

    // === 부분 갱신용 더티 플래그 ===
    bool dirty_board = true;
    bool dirty_block = true;
    bool dirty_next  = true;
    bool dirty_stats = true;
    bool dirty_hold  = true;
    std::unique_ptr<Block> last_drawn_block; // 직전에 그린 블록의 사본 (이전 위치/모양을 지우기 위해)
    std::unique_ptr<Block> last_drawn_ghost; // 직전에 그린 고스트 블록 사본

public:
    Game(IRenderer& r, InputManager& i);
    virtual ~Game() = default;                          // 다형성 소멸을 위해 virtual
    void run();                                         // 메인 루프 진입점. 게임 오버까지 블로킹
    void set_mode(GameMode m) { mode = m; mode_preset = true; }   // 외부에서 모드 지정 (run에서 ask 건너뜀)

private:
    void tick();                        // 자동 낙하 1칸. 착지 시 on_block_landed 호출
    bool try_move(int dx, int dy);      // 이동 시도 → 충돌 시 롤백, 성공 여부 반환
    bool try_rotate();                  // 회전 시도 → 충돌 시 back_rotate로 롤백
    void hard_drop();                   // 한 번에 바닥까지 낙하
    bool any_dirty() const;             // 그릴 게 있는지

    int  ask_game_mode();               // 시작 시 1~5 모드 선택을 사용자에게 받음
    int  ask_start_level();             // 시작 시 1~8 레벨 선택을 사용자에게 받음
    void wait_any_key();                // 콘솔에서 아무 키나 눌릴 때까지 블로킹
    void wait_for_logo_key();           // 로고 화면용: 데모 블록 깜빡임 + 키 대기
    void hold_block();                  // 홀드 → 현재 블록과 홀드 블록 교체

// 파생 클래스에게 필요한 getter, setter 함수 및 주요 함수
protected:
    Board& get_board() { return board; }                        // Board 참조로 반환
    const StageManager& get_stage() const { return stage; }     // StageManager 참조로 반환
    GameStats& get_stats() { return stats; }                    // GameStats 구조체 참조로 반환
    std::unique_ptr<Block>& get_current() { return current; }   // 현재 블록 참조로 반환
    std::unique_ptr<Block>& get_next() { return next; }         // 다음 블록 참조로 반환
    InputManager& get_input() { return input; }                 // InputManager 객체 참조로 반환
    IRenderer& get_renderer() { return renderer; }              // IRenderer 객체 참조로 반환
    GameMode get_mode() const { return mode; }

    void set_dirty_block(bool b) { dirty_block = b; }           // dirty_block 값 수정
    void set_dirty_next(bool b)  { dirty_next = b; }            // dirty_next 값 수정
    void set_dirty_board(bool b) { dirty_board = b; }           // dirty_board 값 수정
    void reset_last_drawn() { last_drawn_block.reset(); }       // last_drawn_block 메모리 해제

    virtual void spawn_next_block();                            // next → current 승격 후 새 next 생성
    virtual void render_frame();                                // 한 프레임 화면 출력 (더티 플래그 기반 부분 갱신)
    virtual void handle_input(char c = 0);                      // 한 프레임의 입력 1회 폴링 후 분기 처리
    virtual void on_block_landed();                             // merge → clear → 점수/레벨 갱신 → 다음 스폰
    virtual void reset_state(int start_level);                  // 새 판 시작용 — 보드/통계/블록 모두 초기화
};
