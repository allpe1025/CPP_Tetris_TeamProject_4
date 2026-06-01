#pragma once
#include "Block.h"
class StarBlock :
    public Block
{
public:
    StarBlock();
    std::string get_display_text() const override { return "★"; }

    void execute_effect(Board& board) override;
};

