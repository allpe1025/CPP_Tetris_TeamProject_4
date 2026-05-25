#pragma once
#include "Block.h"
class CircleBlock :
    public Block
{
public:
    CircleBlock();
    std::string get_display_text() const override { return "●"; }

    void execute_effect(Board& board) override;
};

