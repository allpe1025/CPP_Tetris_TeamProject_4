#include "Board.h"
#include "Block.h"

Board::Board()
{
    reset();
}

void Board::reset()
{
    // 외곽(좌·우·바닥)은 Wall, 내부는 Empty 로 초기화
    grid.assign(height, std::vector<Cell>(width, Cell::Empty));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == left_wall || x == right_wall || y == floor) {
                grid[y][x] = Cell::Wall;
            }
            else {
                grid[y][x] = Cell::Empty;
            }
        }
    }
}

bool Board::check_collision(const Block& block) const
{
    const char (*shape)[4] = block.get_shape();
    int block_x = block.get_x();
    int block_y = block.get_y();

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 0) continue;             // 블록의 빈 셀은 검사 X

            int x = block_x + col;
            int y = block_y + row;

            if (x < 0 || x >= width || y >= height) return true;    // 좌·우·아래 바깥
            if (y < 0) {
                // 좌·우 벽을 블록 스폰 위치(-4)보다 3블럭 위까지(y >= -7) 연장 →
                // 스폰 직후 블록이 양옆으로 빠져나가 즉시 게임오버 되는 문제 방지.
                if (y >= -7 && (x == left_wall || x == right_wall)) return true;
                continue;                                           // 그 외 보드 위쪽 바깥은 자유
            }

            if (grid[y][x] != Cell::Empty) return true;     // 벽 또는 굳은 블록과 충돌
        }
    }

    return false;
}

void Board::merge_block(const Block& block)
{
    const char (*shape)[4] = block.get_shape();
    int block_x = block.get_x();
    int block_y = block.get_y();

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 0) continue;
            int x = block_x + col;
            int y = block_y + row;
            if (is_inside(x, y)) {
                grid[y][x] = Cell::Fixed;                   // 굳어서 보드의 일부가 됨
            }
        }
    }
}

std::vector<int> Board::find_full_lines() const
{
    std::vector<int> rows;
    for (int y = floor - 1; y >= 0; y--) {                  // 바닥부터 위로 스캔
        if (is_full_line(y)) {
            rows.push_back(y);
        }
    }
    return rows;
}

void Board::remove_lines(const std::vector<int>& rows)
{
    // clear_line 을 여러 번 호출하면 인덱스가 어긋나므로 (한 번 shift 하면 이후 인덱스가 다 밀림),
    // 컬럼 단위로 한 번에 압축: "지울 행이 아닌" 셀만 모아 → 바닥부터 다시 채움 → 위는 Empty.
    if (rows.empty()) return;

    auto is_removed = [&](int y) {
        for (int r : rows) if (r == y) return true;
        return false;
    };

    for (int x = left_wall + 1; x < right_wall; x++) {
        std::vector<Cell> survivors;
        for (int y = floor - 1; y >= 0; y--) {
            if (!is_removed(y)) survivors.push_back(grid[y][x]);
        }
        int y = floor - 1;
        for (Cell c : survivors) { grid[y][x] = c; y--; }
        while (y >= 0) { grid[y][x] = Cell::Empty; y--; }
    }
}

void Board::set_cell(int x, int y, Cell value)
{
    if (is_inside(x, y)) {
        grid[y][x] = value;
    }
}

Cell Board::get_cell(int x, int y) const
{
    if (!is_inside(x, y)) return Cell::Wall;                // 바깥은 안전상 Wall 로 취급
    return grid[y][x];
}

bool Board::is_inside(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::is_full_line(int y) const
{
    // 좌·우 벽 제외한 내부 칸이 전부 Empty 가 아니면 가득 찬 줄
    for (int x = left_wall + 1; x < right_wall; x++) {
        if (grid[y][x] == Cell::Empty) return false;
    }
    return true;
}

void Board::clear_line(int y)
{
    // 한 줄을 지우고 위 줄들을 1칸씩 끌어내림. remove_lines 도입 후로는 거의 미사용.
    for (int row = y; row > 0; row--) {
        for (int x = left_wall + 1; x < right_wall; x++) {
            grid[row][x] = grid[row - 1][x];
        }
    }
    for (int x = left_wall + 1; x < right_wall; x++) {
        grid[0][x] = Cell::Empty;
    }
}
