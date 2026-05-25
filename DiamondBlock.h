#pragma once
#include "Block.h"
class DiamondBlock :
    public Block
{
public:
    DiamondBlock();
    std::string get_display_text() const override { return "◆"; }

    void execute_effect(Board& board) override;
};

