#include "StarBlock.h"
#include "Board.h"

StarBlock::StarBlock()
	:Block(-1)
{
}

// 별 블록 특수 능력 : 블록의 각 칸과 인접한 모든 벽이 아닌 블록 제거
void StarBlock::execute_effect(Board& board)
{
    int x = get_x();
    int y = get_y();
    const char (*shape)[4] = get_shape();

    // 각 칸 별 상하좌우 검사용 배열
    int dx[] = { 0, 0, -1, 1 };
    int dy[] = { -1, 1, 0, 0 };

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 1) {
                int x2 = x + col;
                int y2 = y + row;

                // 상하좌우 검사
                for (int i = 0; i < 4; i++) {
                    int x3 = x2 + dx[i];
                    int y3 = y2 + dy[i];

                    // 인접한 벽이 아닌 고정된 블록을 빈 칸으로 대체
                    if (board.is_inside(x3, y3) && board.get_cell(x3, y3) == Cell::Fixed) {
                        board.set_cell(x3, y3, Cell::Empty);
                    }
                }
            }
        }
    }
    set_shape(7);   // 특수 블록은 merge 하지 않기 위해 빈 블록으로 변경
}
