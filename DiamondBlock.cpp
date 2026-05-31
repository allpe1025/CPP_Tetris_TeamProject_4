#include "DiamondBlock.h"
#include "Board.h"

DiamondBlock::DiamondBlock()
	:Block(-1)
{
}


// 다이아몬드 블록 특수 능력 : 블록의 각 칸의 수평 방향에 존재하는 모든 블록을 제거
void DiamondBlock::execute_effect(Board& board)
{
	int y = get_y();
	const char (*shape)[4] = get_shape();
	bool rows[Board::height] = { false };

    // 블록이 차지하고 있는 행 저장
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 1) {
                int y2 = y + row;
                if (y2 >= 0 && y2 < Board::height) {
                    rows[y2] = true;
                }
            }
        }
    }

    // 저장된 모든 행에 존재하는 벽이 아닌 블록 제거
    for (int y = 0; y < Board::height; y++) {
        if (rows[y]) {
            for (int x = Board::left_wall + 1; x < Board::right_wall; x++) {
                if (board.get_cell(x, y) == Cell::Fixed) {
                    board.set_cell(x, y, Cell::Empty);
                }
            }
        }
    }
    set_shape(7);   // 특수 블록은 merge 하지 않기 위해 빈 블록으로 변경
}
