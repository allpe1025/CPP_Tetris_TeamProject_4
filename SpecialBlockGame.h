#pragma once
#include "Game.h"
class SpecialBlockGame :
    public Game
{
public:
    SpecialBlockGame(IRenderer& r, InputManager& i);

private:
    void spawn_next_block() override;

    // 2. 착지했을 때 다형성을 이용해서 한 줄로 효과 발동!
    void on_block_landed() override;
};

