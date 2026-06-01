#pragma once

#include <array>

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
    static constexpr int max_clear  = 4;            // 한 번에 지울 수 있는 최대 라인 수 (테트로미노)

    // 가득 찬 줄 좌표 묶음 — 동적 vector 대신 고정 크기 배열로 heap 할당 회피.
    struct FullLines {
        std::array<int, max_clear> rows{};
        int count = 0;
    };

private:
    // 연속 메모리 2D 배열 (스택/inline 보관). vector<vector<>> 의 포인터 추적 / 힙 할당 제거.
    std::array<std::array<Cell, width>, height> grid{};

    // 보드 상태가 바뀔 때마다 증가하는 카운터. ghost 캐시 등의 무효화 기준으로 사용.
    int version_ = 0;

public:
    Board();

    void reset();                                       // 빈 보드 + 외벽으로 초기화
    bool check_collision(const Block& block) const;     // 블록이 벽/바닥/Fixed 셀과 겹치는지
    void merge_block(const Block& block);               // 블록의 채워진 셀을 Fixed 로 굳혀 grid 에 반영

    // 줄 삭제를 두 단계로 분리 (사이에 애니메이션을 끼워넣기 위함):
    //   1) find_full_lines()  — 가득 찬 줄의 y 좌표만 반환 (보드는 변경 X)
    //   2) remove_lines(rows) — 해당 줄들을 지우고 위 줄을 끌어내림 (컬럼 단위 압축)
    FullLines find_full_lines() const;
    void remove_lines(const FullLines& full);

    void set_cell(int x, int y, Cell value);            // 특정 위치의 블록 한 개만 변경하기 위한 함수
    Cell get_cell(int x, int y) const;                  // 렌더러가 보드 상태를 읽을 때 사용

    bool is_inside(int x, int y) const;

    int version() const { return version_; }            // 캐시 무효화 검사용 (예: ghost 위치 캐시)

private:
    bool is_full_line(int y) const;
};
