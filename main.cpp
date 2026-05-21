#include <cstdlib>
#include <ctime>
#include "ConsoleRenderer.h"
#include "InputManager.h"
#include "Game.h"

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));      // 블록 셔플용 난수 시드

    ConsoleRenderer renderer;
    InputManager    input;
    Game game(renderer, input);
    game.run();
    return 0;
}
