#include "SpecialBlockGame.h"
#include "StarBlock.h"
#include "DiamondBlock.h"
#include "CircleBlock.h"
#include <cstdlib>

SpecialBlockGame::SpecialBlockGame(IRenderer& r, InputManager& i)
	:Game(r, i)
{
}

void SpecialBlockGame::spawn_next_block()
{
    get_current() = std::move(get_next());

    // 30% 확률로 특수 블록으로 생성
    int rand_val = rand() % 100;
    int stick_rate = get_stage().current().stick_rate;
    if (rand_val < 30) {
        // 각 특수 블록이 생성될 확률은 1/3
        int type = rand() % 3;
        if (type == 0)      get_next() = std::make_unique<StarBlock>();
        else if (type == 1) get_next() = std::make_unique<DiamondBlock>();
        else                get_next() = std::make_unique<CircleBlock>();
    }
    else {
        get_next() = std::make_unique<Block>(stick_rate);
    }

    // 화면 갱신을 위해 부모의 더티 플래그 값 수정
    set_dirty_block(true);
    set_dirty_next(true);
    reset_last_drawn();
}