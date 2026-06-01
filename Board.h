#pragma once

#include <vector>

class Block;

// 보드 한 칸의 상태. 외벽/바닥은 Wall, 착지하여 굳은 블록은 Fixed.
enum class Cell {
    Empty = 0,
    Wall  = 1,
    Fixed = 2
};

// 게임 보드 (grid 의 단일 소유자).
// 블록 좌표 충돌 판정, 착지한 블록을 grid 에 병합, 가득 찬 줄 탐색·제거를 책임진다.
class Board {
public:
    static constexpr int width      = 14;
    static constexpr int height     = 21;
    static constexpr int left_wall  = 0;
    static constexpr int right_wall = width - 1;
    static constexpr int floor      = height - 1;

private:
    std::vector<std::vector<Cell>> grid;

public:
    Board();

    void reset();                                       // 빈 보드 + 외벽으로 초기화
    bool check_collision(const Block& block) const;     // 블록이 벽/바닥/Fixed 셀과 겹치는지
    void merge_block(const Block& block);               // 블록의 채워진 셀을 Fixed 로 굳혀 grid 에 반영

    // 줄 삭제를 두 단계로 분리 (사이에 애니메이션을 끼워넣기 위함):
    //   1) find_full_lines()  — 가득 찬 줄의 y 좌표만 반환 (보드는 변경 X)
    //   2) remove_lines(rows) — 해당 줄들을 지우고 위 줄을 끌어내림 (컬럼 단위 압축)
    std::vector<int> find_full_lines() const;
    void remove_lines(const std::vector<int>& rows);
    
    void set_cell(int x, int y, Cell value);            // 특정 위치의 블록 한 개만 변경하기 위한 함수
    Cell get_cell(int x, int y) const;                  // 렌더러가 보드 상태를 읽을 때 사용

    bool is_inside(int x, int y) const;
private:
    bool is_full_line(int y) const;
    void clear_line(int y);                             // 단일 줄 제거 + 위 줄 끌어내림 (legacy)
};