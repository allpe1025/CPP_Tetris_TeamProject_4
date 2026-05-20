#include "Board.h"
#include "Block.h"

Board::Board()
{
    reset();
}

void Board::reset()
{
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
            if (shape[row][col] == 0) {
                continue;
            }

            int x = block_x + col;
            int y = block_y + row;

            if (x < 0 || x >= width || y >= height) {
                return true;
            }

            if (y < 0) {
                continue;
            }

            if (grid[y][x] != Cell::Empty) {
                return true;
            }
        }
    }

    return false;
}

vvoid Board::merge_block(const Block& block)
{
    const char (*shape)[4] = block.get_shape();

    int block_x = block.get_x();
    int block_y = block.get_y();

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 0) {
                continue;
            }

            int x = block_x + col;
            int y = block_y + row;

            if (is_inside(x, y)) {
                grid[y][x] = Cell::Fixed;
            }
        }
    }
}

int Board::clear_full_lines()
{
    int cleared_count = 0;

    for (int y = floor - 1; y >= 0; y--) {
        if (is_full_line(y)) {
            clear_line(y);
            cleared_count++;

            y++;
        }
    }

    return cleared_count;
}

Cell Board::get_cell(int x, int y) const
{
    if (!is_inside(x, y)) {
        return Cell::Wall;
    }

    return grid[y][x];
}

bool Board::is_inside(int x, int y) const
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool Board::is_full_line(int y) const
{
    for (int x = left_wall + 1; x < right_wall; x++) {
        if (grid[y][x] == Cell::Empty) {
            return false;
        }
    }

    return true;
}

void Board::clear_line(int y)
{
    for (int row = y; row > 0; row--) {
        for (int x = left_wall + 1; x < right_wall; x++) {
            grid[row][x] = grid[row - 1][x];
        }
    }

    for (int x = left_wall + 1; x < right_wall; x++) {
        grid[0][x] = Cell::Empty;
    }
}
