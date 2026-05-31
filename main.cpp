#include <cstdlib>
#include <ctime>
#include "ConsoleRenderer.h"
#include "InputManager.h"
#include "Game.h"
#include "SpecialBlockGame.h"
#include "SkillGame.h"

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));      // 블록 셔플용 난수 시드

    ConsoleRenderer renderer;
    InputManager    input;
    SkillGame game(renderer, input);
    game.run();
    return 0;
}
