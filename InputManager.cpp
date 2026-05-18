#include "InputManager.h"

char InputManager::get_input()
{
    if (_kbhit()) {
        int key = _getch();
        if (key == 0xffffffe0) { // È®ÀåÅ°
            key = _getch();
            switch (key) {
            case 0x4b: return 'l';
            case 0x4d: return 'r';
            case 0x48: return 'u';
            case 0x50: return 'd';
            }
        }
        else {
            if (key == 32) return 's';
        }
    }
    return 0;
}
