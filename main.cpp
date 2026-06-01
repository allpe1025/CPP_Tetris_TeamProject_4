#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <conio.h>
#include <Windows.h>
#include "ConsoleRenderer.h"
#include "InputManager.h"
#include "Game.h"
#include "SpecialBlockGame.h"
#include "SkillGame.h"

namespace {
    // 로고 화면을 띄우고 키 입력 대기. (Game::wait_for_logo_key 와 동일 로직 —
    // 다만 모드 선택 전에 보여줘야 해서 main 으로 끌어올림.)
    void show_intro_screen(IRenderer& renderer)
    {
        renderer.clear();
        renderer.draw_logo();
        int tick = 0;
        while (!_kbhit()) {
            if (tick % 40 == 0) {
                renderer.draw_logo_demo();
            }
            Sleep(30);
            tick++;
        }
        _getch();
    }

    // 모드 선택은 main 에서 먼저 처리하여 해당 모드에 맞는 Game 서브클래스를 인스턴스화한다.
    // (Inverted, HiddenStack 은 base Game 의 mode 분기로 처리되고,
    //  SpecialBlock 은 SpecialBlockGame, Quest 는 SkillGame 으로 디스패치된다.)
    int ask_mode_for_dispatch(IRenderer& renderer)
    {
        renderer.clear();           // 로고 지우고 메뉴 표시
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

    // 1) 로고 먼저 (게임이 뭔지 알려줌)
    show_intro_screen(renderer);

    // 2) 모드 선택
    GameMode mode = static_cast<GameMode>(ask_mode_for_dispatch(renderer));

    // 3) 모드에 맞는 Game 서브클래스 생성
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

    // 4) 게임 진입 (run 내부에서 레벨 선택 후 메인 루프)
    game->run();
    return 0;
}
