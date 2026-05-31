#include "Game.h"
#include <Windows.h>
#include <conio.h>
#include <cstdlib>
#include <iostream>

namespace {
    // 한 프레임 길이(ms). StageConfig::speed 는 이 단위의 배수가 된다.
    // 예) speed=40 → 40 * 15 = 600ms 마다 1칸 자동 낙하. 원본 Sleep(15)와 동일.
    constexpr int FRAME_MS = 15;
}

Game::Game(IRenderer& r, InputManager& i)
    : renderer(r), input(i)
{
}

void Game::run()
{
    renderer.clear();
    renderer.draw_logo();
    wait_for_logo_key();                    // 데모 블록 깜빡이며 키 대기 (원본 흐름)

    mode = static_cast<GameMode>(ask_game_mode());  // 1~5 선택 (모드가 레벨 선택보다 먼저)
    int start_level = ask_start_level();            // 1~8 선택

    // 원본은 게임오버 후 init() 호출하며 무한 반복. 동일 흐름.
    while (true) {
        reset_state(start_level);

        running = true;
        while (running) {
            handle_input();
            tick();
            if (any_dirty()) render_frame();    // 더티 플래그가 켜진 영역만 갱신
            Sleep(FRAME_MS);
        }

        renderer.draw_game_over();
        Sleep(1000);
        wait_any_key();
        // 같은 시작 레벨로 다시 시작 (원본도 init 후 input_data 다시 호출하지만
        // 사용자 입력 흐름을 매번 끊지 않게 여기선 한 번 받은 레벨을 재사용).
    }
}

int Game::ask_game_mode()
{
    // 콘솔 텍스트 입력으로 모드 선택 (ask_start_level 과 동일한 스타일).
    // 1~5 이외 입력은 다시 받음. 화면을 한 번 비운 뒤 로고 위에 메뉴를 출력한다.
    renderer.clear();
    std::cout << "==== Select Game Mode ====\n";
    std::cout << "  1. Basic         (기본)\n";
    std::cout << "  2. Inverted      (좌우/회전 키 반전)\n";
    std::cout << "  3. Hidden Stack  (쌓인 블록 숨김)\n";
    std::cout << "  4. Special Block (특수 블록 등장)\n";
    std::cout << "  5. Quest         (퀘스트)\n";

    int m = 0;
    while (m < 1 || m > 5) {
        std::cout << "Select Mode[1-5]: ";
        std::cin >> m;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            m = 0;
        }
    }
    return m;
}

int Game::ask_start_level()
{
    int lv = 0;
    while (lv < 1 || lv > 8) {
        std::cout << "\nSelect Start level[1-8]: ";
        std::cin >> lv;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1024, '\n');
            lv = 0;
        }
    }
    return lv;
}

void Game::reset_state(int start_level)
{
    board.reset();
    stage.set_level(start_level - 1);       // 사용자 입력은 1~8, 내부 인덱스는 0~7
    stats = GameStats{};
    gravity_tick = 0;
    current.reset();
    next = std::make_unique<Block>(stage.current().stick_rate);
    spawn_next_block();
    renderer.clear();
    last_drawn_block.reset();
	last_drawn_ghost.reset();
	hold.reset();
	can_hold = true;
    dirty_board = dirty_block = dirty_next = dirty_stats = dirty_hold = true;
}

void Game::wait_any_key()
{
    while (!_kbhit()) Sleep(30);
    _getch();                               // 입력 버퍼에서 꺼냄
}

void Game::wait_for_logo_key()
{
    // 원본 show_logo 의 데모 블록 루프 이식.
    // 40 tick 마다 (≈1.2초) 무작위 블록 4개를 새로 뿌리면서 키가 눌릴 때까지 대기.
    int tick = 0;
    while (!_kbhit()) {
        if (tick % 40 == 0) {
            renderer.draw_logo_demo();
        }
        Sleep(30);
        tick++;
    }
    _getch();
}

void Game::handle_input()
{
    // === 팀원 작업: Inverted 모드 ===
    // mode == GameMode::Inverted 일 때 좌우 키 + 회전 방향을 반전한다.
    // 예시 (참고용):
    //   bool inv = (mode == GameMode::Inverted);
    //   case 'l': try_move(inv ?  1 : -1, 0); break;
    //   case 'r': try_move(inv ? -1 :  1, 0); break;
    //   case 'u':
    //       if (inv) { current->back_rotate(); if (board.check_collision(*current)) current->rotate();  else dirty_block = true; }
    //       else     { try_rotate(); }
    //       break;
    // (현재는 Basic 동작만)
    switch (input.get_input()) {
    case 'l': try_move(-1, 0); break;       // 좌
    case 'r': try_move( 1, 0); break;       // 우
    case 'd':                               // 소프트 드롭: 못 내려가면 즉시 착지
        if (!try_move(0, 1)) on_block_landed();
        break;
    case 'u': try_rotate();    break;       // 회전
    case 's': hard_drop();     break;       // 하드 드롭
	case 'h': hold_block();    break;       // 홀드
    default:                   break;       // 입력 없음 (0) 또는 미정의 키
    }
}

void Game::tick()
{
    gravity_tick++;
    if (gravity_tick < stage.current().speed) {
        return;
    }
    gravity_tick = 0;

    if (!try_move(0, 1)) {
        on_block_landed();
    }
}

bool Game::try_move(int dx, int dy)
{
    if (!current) return false;

    // 1) 후보 위치로 이동
    if      (dx == -1) current->move_left();
    else if (dx ==  1) current->move_right();
    if      (dy == -1) current->move_up();
    else if (dy ==  1) current->move_down();

    // 2) 충돌이면 원위치로 롤백
    if (board.check_collision(*current)) {
        if      (dx == -1) current->move_right();
        else if (dx ==  1) current->move_left();
        if      (dy == -1) current->move_down();
        else if (dy ==  1) current->move_up();
        return false;
    }
    dirty_block = true;
    return true;
}

bool Game::try_rotate()
{
    if (!current) return false;

    current->rotate();
    if (board.check_collision(*current)) {
        current->back_rotate();
        return false;
    }
    dirty_block = true;
    return true;
}

void Game::hard_drop()
{
    while (try_move(0, 1)) { /* 더 이상 내려갈 수 없을 때까지 반복 */ }
    on_block_landed();
}

void Game::on_block_landed()
{
    // 게임 오버: 블록이 보드에 채 진입하지 못한 채로 막힘 (원본 move_block의 y<=0 로직 이식)
    if (current->get_y() < 0) {
        running = false;
        return;
    }
    renderer.draw_block(*current, current->get_x(), current->get_y());
    Sleep(100);                           //+ 착지한 블록 0.1초 출력
    board.merge_block(*current);

    // 1) 가득 찬 줄 탐지 (보드는 아직 변경 X)
    auto full_rows = board.find_full_lines();
    // 2) 각 줄에 깜빡임 애니메이션 (원본 동작 이식)
    for (int row : full_rows) {
        renderer.animate_line_clear(row);
    }
    // 3) 실제 줄 제거 + 위 줄 끌어내림
    board.remove_lines(full_rows);

    const int cleared = static_cast<int>(full_rows.size());
    stats.total_cleared    += cleared;
    stats.cleared_in_stage += cleared;
    // 원본 공식: 줄당 100 + level*10 + rand()%10 (랜덤 보너스 + 레벨 보너스)
    for (int i = 0; i < cleared; i++) {
        stats.score += 100 + stage.level_index() * 10 + (rand() % 10);
    }

    // === 팀원 작업: Quest 모드 ===
    // mode == GameMode::Quest 일 때 퀘스트 상태를 갱신하고 달성 시 보상.
    // - 퀘스트 상태는 Game 멤버로 새로 추가 (예: struct QuestState { int target_lines, cleared; } quests;).
    //   reset_state() 에서 함께 초기화.
    // - 갱신 예 (의사 코드):
    //     if (mode == GameMode::Quest) {
    //         quests.cleared += cleared;
    //         if (quests.cleared >= quests.target_lines) {
    //             stats.score += 500;       // 보너스
    //             quests.cleared = 0;
    //             quests.target_lines += 2; // 다음 퀘스트
    //         }
    //         dirty_stats = true;
    //     }
    // - UI 표기가 필요하면 IRenderer::draw_stats 의 시그니처 확장 또는 draw_quest 메서드 신설.

    // 보드/통계 변경 → 다음 프레임에 다시 그림
    dirty_board = true;
    dirty_stats = true;

    // 레벨업 판정: 현재 스테이지의 목표 라인을 채웠으면 다음 스테이지로 진행
    if (stage.should_advance(stats.cleared_in_stage)) {
        if (stage.advance()) {
            stats.cleared_in_stage = 0;
            renderer.clear();                   // 벽 색이 바뀌므로 화면 전체 재출력
            dirty_board = dirty_block = dirty_next = dirty_stats = true;
            last_drawn_block.reset();           // 이전 잔상 추적 무효화
            last_drawn_ghost.reset();           // 이전 ghost 블록 추적 무효화
        }
    }

    spawn_next_block();
}

void Game::spawn_next_block()
{
    current = std::move(next);
    next    = std::make_unique<Block>(stage.current().stick_rate);
    // === 팀원 작업: SpecialBlock 모드 ===
    // mode == GameMode::SpecialBlock 일 때 일정 확률로 next 를 "특수 블록" 으로 교체한다.
    // - 특수 블록은 Block 을 상속한 별도 클래스를 새로 만들거나 (예: SpecialBlock),
    //   Block 내부에 enum SpecialKind 같은 식별자를 추가하는 두 방식이 가능. 본인이 선택.
    // - 사용 예 (의사 코드):
    //     if (mode == GameMode::SpecialBlock && (rand() % 100) < 15) {
    //         next = std::make_unique<SpecialBlock>(...);
    //     }
    // - 특수 효과 발동 시점은 on_block_landed (merge 직후 ~ 점수 가산 사이) 에 분기 추가.
    // gravity_tick 은 일부러 리셋하지 않음 — 원본의 메인 루프 카운터처럼 위상 연속 유지
    dirty_block = true;
    dirty_next  = true;
	can_hold = true;            // 새 블록이 등장했으므로 홀드 사용 가능
    last_drawn_block.reset();                   // 직전에 그린 블록은 이미 보드에 merge 됨 → 추적 무효화
	last_drawn_ghost.reset();                   // 직전에 그린 ghost 블록도 사라질 예정 → 추적 무효화
}

bool Game::any_dirty() const
{
    return dirty_board || dirty_block || dirty_next || dirty_stats || dirty_hold;
}

void Game::render_frame()
{
    // 1) 블록이 움직였으면 이전 ghost 블록 먼저 지움
    if (dirty_block && last_drawn_ghost) {
        renderer.erase_block(*last_drawn_ghost,
                             last_drawn_ghost->get_x(),
                             last_drawn_ghost->get_y());
        last_drawn_ghost.reset();
    }

    // 2) 블록이 움직였으면 이전 위치 지움 (보드가 함께 dirty면 어차피 덮어쓰니 무해)
    if (dirty_block && last_drawn_block) {
        renderer.erase_block(*last_drawn_block,
                             last_drawn_block->get_x(),
                             last_drawn_block->get_y());
		last_drawn_block.reset();
    }

    // 3) 보드 갱신 (벽 색이 바뀌었거나, 줄 삭제·머지 발생 등)
    //    보드가 블록 영역까지 덮어쓰므로 블록도 다시 그려야 함.
    // === 팀원 작업: HiddenStack 모드 ===
    // 현재 블록이 "내려가는 중" 일 때는 쌓인 Fixed 셀을 그리지 않고 벽(Wall)만 그린다.
    // - "내려가는 중" 판정 기준은 직접 정해야 함 (예: 새 블록이 스폰된 후 첫 hard_drop/soft_drop 입력 발생 시 플래그 ON,
    //   on_block_landed 끝에서 OFF — Game 멤버 bool falling_started 같은 걸 추가).
    // - 렌더링 분기: ConsoleRenderer 에 draw_board_hidden(const Board&, int level) 같은 변형 메서드를 추가하거나,
    //   IRenderer::draw_board 시그니처에 bool hide_fixed 파라미터를 추가.
    // - 사용 예 (의사 코드):
    //     if (mode == GameMode::HiddenStack && falling_started)
    //         renderer.draw_board_hidden(board, stage.display_level());
    //     else
    //         renderer.draw_board(board, stage.display_level());
    if (dirty_board) {
        renderer.draw_board(board, stage.display_level());
        dirty_board = false;
        dirty_block = true;
    }

    // 4) 블록 갱신
    if (dirty_block && current) {
		Block ghost = *current;    // 고스트 블록은 현재 블록의 사본

		while (!board.check_collision(ghost)) {
			ghost.move_down();
		}
		ghost.move_up();           // 충돌한 바로 위가 착지 위치 → 한 칸 올리기

		renderer.draw_ghost_block(ghost, ghost.get_x(), ghost.get_y());   // 고스트 블록 그리기
        renderer.draw_block(*current, current->get_x(), current->get_y());
        last_drawn_block = std::make_unique<Block>(*current);   // 다음 프레임에서 지우기 위한 사본
		last_drawn_ghost = std::make_unique<Block>(ghost);     // 다음 프레임에서 지우기 위한 사본
        dirty_block = false;
    }

    // 5) 다음 블록 미리보기
    if (dirty_next) {
        if (next) {
            renderer.draw_next_block(*next, stage.display_level());
        }
        dirty_next = false;
    }

	// 6) 홀드 블록
    if (dirty_hold) {
        renderer.draw_hold_block(hold.get(), stage.display_level());
        dirty_hold = false;
    }

    // 7) 통계 박스
    if (dirty_stats) {
        int lines_left = stage.current().clear_line - stats.cleared_in_stage;
        if (lines_left < 0) lines_left = 0;
        renderer.draw_stats(stage.display_level(), stats.score, lines_left);
        dirty_stats = false;
    }
}

void Game::hold_block()
{
	if (!current || !can_hold) return;
    // 처음 홀드하는 경우: current → hold, next → current, 새 next 생성
	if (!hold) {     
		hold = std::move(current);
		current = std::move(next);
		next = std::make_unique<Block>(stage.current().stick_rate);
        dirty_next = true;
    }
    else {
		//이미 홀드한 블록이 있는 경우: current ↔ hold 스왑
		std::swap(current, hold);
    }

	current->reset_position();    // 홀드 후 현재 블록은 새로 등장하는 것처럼 위치/회전 초기화
	hold->reset_position();       // 홀드에 들어간 블록도 미리보기용으로 초기회

	can_hold = false;            // 착지 전까지 홀드 사용 불가
	dirty_block = true;
	dirty_hold = true;
}

