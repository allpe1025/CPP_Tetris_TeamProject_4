#pragma once

#include <vector>

class Block;

enum class Cell {
    Empty = 0,
    Wall = 1,
    Fixed = 2
};

class Board {
public:
    static constexpr int width = 14;
    static constexpr int height = 21;
    static constexpr int left_wall = 0;
    static constexpr int right_wall = width - 1;
    static constexpr int floor = height - 1;

private:
    std::vector<std::vector<Cell>> grid;

public:
    Board();

    void reset();                                   // 보드 초기화
    bool check_collision(const Block& block) const; // strike_check 역할
    void merge_block(const Block& block);           // merge_block 역할
    int clear_full_lines();                         // check_full_line 역할

    Cell get_cell(int x, int y) const;              // 렌더러가 보드 상태를 읽을 때 사용

private:
    bool is_inside(int x, int y) const;
    bool is_full_line(int y) const;
    void clear_line(int y);
};
