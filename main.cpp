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
    // 콘솔 코드페이지를 UTF-8 (65001) 로 설정 → 소스의 UTF-8 한글/유니코드 글자(■, ▨, ◆ 등)가
    // CP949 환경에서도 깨지지 않고 출력됨. (수동 chcp 65001 입력 불필요)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 주의: std::ios::sync_with_stdio(false) 적용 금지.
    // Windows 콘솔의 C stdout 은 _IONBF (unbuffered) 인데, sync=false 로 바꾸면 cout 이
    // 자체 8KB stdio_filebuf 로 전환되어 buffered 가 됨. SetConsoleCursorPosition 은
    // 영향을 안 받고 즉시 실행되므로, 커서 이동과 실제 셀 출력 시점이 어긋나
    // 글자가 마지막 커서 위치에 몰리는 렌더 corruption 이 발생함.

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
