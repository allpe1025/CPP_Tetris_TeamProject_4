#include "InputManager.h"

char InputManager::get_input()
{
    if (!_kbhit()) return 0;

    int key = _getch();
    if (key == 0xE0) {                          // 방향키 prefix — 두 번째 _getch 가 실제 키
        key = _getch();
        switch (key) {
        case 0x4b: return 'l';                  // ← 왼쪽
        case 0x4d: return 'r';                  // → 오른쪽
        case 0x48: return 'u';                  // ↑ 위 (회전)
        case 0x50: return 'd';                  // ↓ 아래 (소프트 드롭)
        }
        return 0;
    }
    if (key == 32) return 's';                  // 스페이스 (하드 드롭)
    return 0;
}
