#pragma once
#include <conio.h>

// 키보드 입력 폴링.
// 방향키 ←→↑↓ 와 스페이스(하드 드롭) 만 인식한다. 비차단 — 키가 없으면 즉시 0 반환.
class InputManager
{
public:
    char get_input();                       // 키를 단일 문자 코드로 반환. 입력 없으면 0
};
