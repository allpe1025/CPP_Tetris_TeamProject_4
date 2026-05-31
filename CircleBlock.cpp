#include "CircleBlock.h"
#include "Board.h"

CircleBlock::CircleBlock()
	:Block(-1)
{
}

// 원 블록 특수 능력 : 블록의 각 칸마다 3x3 영역의 모든 블록 제거
void CircleBlock::execute_effect(Board& board)
{
    int x = get_x();
    int y = get_y();
    const char (*shape_data)[4] = get_shape();

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape_data[row][col] == 1) {
                int x2 = x + col;
                int y2 = y + row;

                // 각 칸을 중심으로 3x3 영역 순회
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int x3 = x2 + j;
                        int y3 = y2 + i;

                        if (board.is_inside(x3, y3) && board.get_cell(x3, y3) == Cell::Fixed) {
                            board.set_cell(x3, y3, Cell::Empty);
                        }
                    }
                }
            }
        }
    }
    set_shape(7);   // 특수 블록은 merge 하지 않기 위해 빈 블록으로 변경
}
