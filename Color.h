#pragma once
#include <Windows.h>

// Windows 콘솔 색상 코드.
// 값은 SetConsoleTextAttribute 에 그대로 넘기는 정수와 일치해야 한다.
enum class Color
{
    BLACK = 0, DARK_BLUE, DARK_GREEN, DARK_SKY_BLUE, DARK_RED,
    DARK_VIOLET, DARK_YELLOW, GRAY, DARK_GRAY, BLUE,
    GREEN, SKY_BLUE, RED, VIOLET, YELLOW, WHITE
};

// 콘솔 글자 색 설정 유틸.
class ColorUtility {
public:
    static void apply(Color color) {            // 현재 콘솔에 색상 적용
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));
    }
};
