#include "SkillGame.h"
#include "Board.h"
#include "Block.h"
#include <cstdlib>

SkillGame::SkillGame(IRenderer& r, InputManager& i)
	:Game(r, i)
{
    w_shape = rand() % 6 + 1;
}

void SkillGame::handle_input(char c)
{
    c = get_input().get_input();
    switch (c)
    {
    case 'q':
        if (has_q) use_skill_q();
        break;
    case 'w':
        if (has_w) use_skill_w();
        break;
    case 'e':
        if (has_e) use_skill_e();
        break;
    case 't':
        has_q = true;
        has_w = true;
        has_e = true;
        break;
    default:
        Game::handle_input(c);  // 부모의 handle_input()에서 입력 처리
        break;
    }
}

void SkillGame::reset_state(int start_level)
{
    Game::reset_state(start_level);

    has_q = false;
    has_w = false;
    has_e = false;
    q_count = 0;
    w_count = 0;
    w_shape = rand() % 6 + 1;
}

void SkillGame::render_frame()
{
    Game::render_frame();

    get_renderer().draw_skill_status(has_q, has_w, has_e, q_count, w_count, w_shape);
}

void SkillGame::on_block_landed()
{
    // w 스킬 조건 확인
    if (get_current().get()->get_shape_num() == w_shape && !has_w) {
        w_count++;
        if (w_count >= 5) {
            has_w = true;
            w_count = 0;
        }
    }

    // q, e 스킬 조건 확인을 위해 현재 블록 착지 전 맵의 블록 총 개수 세기
    int before_fixed_cells = 0;
    for (int y = 0; y < Board::height; ++y) {
        for (int x = Board::left_wall + 1; x < Board::right_wall; ++x) {
            if (get_board().get_cell(x, y) == Cell::Fixed) {
                before_fixed_cells++;
            }
        }
    }

    Game::on_block_landed();

    // 현재 블록 착지 후 맵의 총 블록 개수
    int after_fixed_cells = 0;
    for (int y = 0; y < Board::height; ++y) {
        for (int x = Board::left_wall + 1; x < Board::right_wall; ++x) {
            if (get_board().get_cell(x, y) == Cell::Fixed) {
                after_fixed_cells++;
            }
        }
    }

    int broken_cells = (before_fixed_cells + 4) - after_fixed_cells;    // 파괴된 블록 개수 (착지한 블록 포함)
    int row_width = Board::right_wall - Board::left_wall - 1;           // 맵의 행 길이

    // q 스킬 조건 확인
    if (broken_cells > 0) {
        if (!has_q) {
            q_count += broken_cells;
            if (q_count >= 36) {
                has_q = true;
                q_count = 0;
            }
        }

        // E 스킬 조건 확인 : 한 번에 지워진 줄 수 = 부서진 블록 개수 / 맵 행의 길이
        int lines_cleared = broken_cells / row_width;
        if (lines_cleared == 4) {
            has_e = true;
        }
    }
}

// Q스킬 : 랜덤으로 연속된 2열 삭제
void SkillGame::use_skill_q()
{
    has_q = false;

    int play_width = Board::right_wall - Board::left_wall - 1;
    if (play_width >= 2) {
        // 벽 범위 내에서 랜덤한 열을 선택
        int rand_x = (Board::left_wall + 1) + (rand() % (play_width - 1));

        // 연속된 2열의 바닥부터 위로 모든 칸을 공백으로 초기화
        for (int y = 0; y < Board::floor; ++y) {
            get_board().set_cell(rand_x, y, Cell::Empty);
            get_board().set_cell(rand_x + 1, y, Cell::Empty);
        }
    }

    set_dirty_board(true);  // 보드 갱신
}

// W스킬 : 맨 아래 2행 삭제
void SkillGame::use_skill_w()
{
    has_w = false;

    // Board::FullLines 는 std::array 기반 (heap 할당 없음, 최대 4 라인)
    Board::FullLines remove_rows{};
    if (Board::floor - 1 >= 0) remove_rows.rows[remove_rows.count++] = Board::floor - 1; // 맨 아래 행
    if (Board::floor - 2 >= 0) remove_rows.rows[remove_rows.count++] = Board::floor - 2; // 맨 아래 한 칸 위의 행

    get_board().remove_lines(remove_rows);

    set_dirty_board(true);  // 보드 갱신
}

// E스킬 : 맵에 존재하는 전체 블럭 삭제
void SkillGame::use_skill_e()
{
    has_e = false;

    // Board.cpp의 reset() 활용
    get_board().reset();

    set_dirty_board(true);  // 보드 갱신
}
