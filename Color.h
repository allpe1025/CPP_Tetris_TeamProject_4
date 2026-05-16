#pragma once
#include <Windows.h>
enum class Color
{
    BLACK = 0, DARK_BLUE, DARK_GREEN, DARK_SKY_BLUE, DARK_RED,
    DARK_VIOLET, DARK_YELLOW, GRAY, DARK_GRAY, BLUE,
    GREEN, SKY_BLUE, RED, VIOLET, YELLOW, WHITE
};

class ColorUtility {
public:
    static void apply(Color color) { //블럭 색 설정
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<int>(color));
    }
};
