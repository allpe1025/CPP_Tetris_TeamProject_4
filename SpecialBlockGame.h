#pragma once
#include "Game.h"
class SpecialBlockGame :
    public Game
{
public:
    SpecialBlockGame(IRenderer& r, InputManager& i);

private:
    void spawn_next_block() override;
};

