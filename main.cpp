#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include "ConsoleRenderer.h"
#include "InputManager.h"
#include "Game.h"
#include "SpecialBlockGame.h"
#include "SkillGame.h"

namespace {
    // 모드 선택은 main 에서 먼저 처리하여 해당 모드에 맞는 Game 서브클래스를 인스턴스화한다.
    // (Inverted, HiddenStack 은 base Game 의 mode 분기로 처리되고,
    //  SpecialBlock 은 SpecialBlockGame, Quest 는 SkillGame 으로 디스패치된다.)
    int ask_mode_for_dispatch()
    {
        std::cout << "==== Select Game Mode ====\n";
        std::cout << "  1. Basic         (기본)\n";
        std::cout << "  2. Inverted      (좌우/회전 키 반전)\n";
        std::cout << "  3. Hidden Stack  (쌓인 블록 숨김)\n";
        std::cout << "  4. Special Block (특수 블록 등장)\n";
        std::cout << "  5. Quest         (스킬 / 퀘스트)\n";

        int m = 0;
        while (m < 1 || m > 5) {
            std::cout << "Select Mode[1-5]: ";
            std::cin >> m;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(1024, '\n');
                m = 0;
            }
        }
        return m;
    }
}

int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));      // 블록 셔플용 난수 시드

    ConsoleRenderer renderer;
    InputManager    input;

    GameMode mode = static_cast<GameMode>(ask_mode_for_dispatch());

    std::unique_ptr<Game> game;
    switch (mode) {
    case GameMode::SpecialBlock:
        game = std::make_unique<SpecialBlockGame>(renderer, input);
        break;
    case GameMode::Quest:
        game = std::make_unique<SkillGame>(renderer, input);
        break;
    default:    // Basic / Inverted / HiddenStack 은 base Game 의 mode 분기로 처리
        game = std::make_unique<Game>(renderer, input);
        break;
    }
    game->set_mode(mode);
    game->run();
    return 0;
}
