#pragma once
#include "Block.h"
#include <string>
class SpecialBlock :
    public Block
{
private:
    const static inline char special_block[7][4][4][4] = {};
    int m_type;
public:
    SpecialBlock();
    ~SpecialBlock() = default;
    std::string get_display_text() const;
};