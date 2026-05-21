#include "Block.h"
#include <cstdlib>

Block::Block(int stick_rate)
{
    // 막대(I) 등장 확률 처리: stick_rate(%) 확률로 막대 강제 스폰, 그 외에는 6종 중 하나
    if ((rand() % 100) <= stick_rate) {     // 원본과 동일하게 boundary inclusive (stick_rate=20 이면 21%)
        shape = 0;                          // 0 = 막대(I)
    }
    else {
        shape = (rand() % 6) + 1;           // 1~6 = 그 외 6종
    }

    angle = 0;
    x = START_X;
    y = START_Y;

    // 원본 tetris.cpp::show_cur_block 의 shape별 색 매핑 그대로 이식.
    // (값은 Color enum 정수값과 일치 — RED=12, BLUE=9, SKY_BLUE=11, WHITE=15, YELLOW=14, VIOLET=13, GREEN=10)
    static const int SHAPE_COLORS[7] = {
        12,  // 0 I → RED
        9,   // 1 O → BLUE
        11,  // 2 T → SKY_BLUE
        15,  // 3 L → WHITE
        14,  // 4 J → YELLOW
        13,  // 5 Z → VIOLET
        10   // 6 S → GREEN
    };
    color = SHAPE_COLORS[shape];
}

// 로고 데모 전용: 무작위 모양/각도/위치를 명시적으로 지정해서 만든다.
Block::Block(int shape_idx, int angle_idx, int x_pos, int y_pos)
{
    shape = shape_idx;
    angle = angle_idx;
    x     = x_pos;
    y     = y_pos;

    // 색은 일반 생성자와 동일한 매핑 사용
    static const int SHAPE_COLORS[7] = { 12, 9, 11, 15, 14, 13, 10 };
    color = SHAPE_COLORS[shape];
}

const char (*Block::get_shape() const)[4]
{
    return block[shape][angle];
}
