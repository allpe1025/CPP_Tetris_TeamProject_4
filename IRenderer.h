#pragma once
class IRenderer
{
    public:
        virtual ~IRenderer() = default;
        virtual void draw_board(const class Board& board, int level) = 0;
        virtual void draw_block(const class Block& block, int x, int y) = 0;
        virtual void erase_block(const class Block& block, int x, int y) = 0;
        virtual void draw_next_block(const class Block& next) = 0;
        virtual void draw_stats(int level, int score, int lines_left) = 0;
        virtual void draw_game_over() = 0;
        
        virtual void draw_logo() = 0;
        virtual void clear() = 0;
};

