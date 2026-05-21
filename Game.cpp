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

    int start_level = ask_start_level();    // 1~8 선택

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
    dirty_board = dirty_block = dirty_next = dirty_stats = true;
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
    switch (input.get_input()) {
    case 'l': try_move(-1, 0); break;       // 좌
    case 'r': try_move( 1, 0); break;       // 우
    case 'd':                               // 소프트 드롭: 못 내려가면 즉시 착지
        if (!try_move(0, 1)) on_block_landed();
        break;
    case 'u': try_rotate();    break;       // 회전
    case 's': hard_drop();     break;       // 하드 드롭
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
        }
    }

    spawn_next_block();
}

void Game::spawn_next_block()
{
    current = std::move(next);
    next    = std::make_unique<Block>(stage.current().stick_rate);
    // gravity_tick 은 일부러 리셋하지 않음 — 원본의 메인 루프 카운터처럼 위상 연속 유지
    dirty_block = true;
    dirty_next  = true;
    last_drawn_block.reset();                   // 직전에 그린 블록은 이미 보드에 merge 됨 → 추적 무효화
}

bool Game::any_dirty() const
{
    return dirty_board || dirty_block || dirty_next || dirty_stats;
}

void Game::render_frame()
{
    // 1) 블록이 움직였으면 이전 위치 먼저 지움 (보드가 함께 dirty면 어차피 덮어쓰니 무해)
    if (dirty_block && last_drawn_block) {
        renderer.erase_block(*last_drawn_block,
                             last_drawn_block->get_x(),
                             last_drawn_block->get_y());
    }

    // 2) 보드 갱신 (벽 색이 바뀌었거나, 줄 삭제·머지 발생 등)
    //    보드가 블록 영역까지 덮어쓰므로 블록도 다시 그려야 함.
    if (dirty_board) {
        renderer.draw_board(board, stage.display_level());
        dirty_board = false;
        dirty_block = true;
    }

    // 3) 블록 갱신
    if (dirty_block && current) {
        renderer.draw_block(*current, current->get_x(), current->get_y());
        last_drawn_block = std::make_unique<Block>(*current);   // 다음 프레임에서 지우기 위한 사본
        dirty_block = false;
    }

    // 4) 다음 블록 미리보기
    if (dirty_next) {
        if (next) {
            renderer.draw_next_block(*next, stage.display_level());
        }
        dirty_next = false;
    }

    // 5) 통계 박스
    if (dirty_stats) {
        int lines_left = stage.current().clear_line - stats.cleared_in_stage;
        if (lines_left < 0) lines_left = 0;
        renderer.draw_stats(stage.display_level(), stats.score, lines_left);
        dirty_stats = false;
    }
}

