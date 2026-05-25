#pragma once
#include <string>

// 테트로미노 한 개를 표현한다.
// 자기 모양 데이터·위치·각도·색을 모두 가지고 있고, 이동/회전 메소드도 자기 자신을 통해 호출된다.
class Block
{
private:
    // 블록 모양 데이터: [shape][angle][row][col]
    //   shape 0: I(막대), 1: O(네모), 2: T('ㅓ'), 3: L('ㄱ'),
    //   shape 4: J('ㄴ'), 5: Z, 6: S
    const static inline char block[7][4][4][4] = {
        // I (막대)
        1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,    1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,    1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,    1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,

        // O (네모)
        1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,    1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,    1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,    1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,

        // T ('ㅓ')
        0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0,    1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,    1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,    0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,

        // L ('ㄱ')
        1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,    1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,    1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,    0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,

        // J ('ㄴ')
        1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,    1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,    0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,    1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,

        // Z
        1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,    0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,    1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,    0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,

        // S
        0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,    1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,    0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,    1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0
    };

    static constexpr int START_X = 5;
    static constexpr int START_Y = -4;          // 보드 위쪽 바깥에서 떨어지기 시작

    int shape, angle, x, y, color;

public:
    Block() = delete;                                       // 디폴트 생성 금지: stick_rate를 반드시 받아야 함
    Block(int stick_rate);                                  // stick_rate: 막대(I) 등장 확률(%)
    Block(int shape_idx, int angle_idx, int x_pos, int y_pos); // 로고 데모용: 모양·각도·좌표 직접 지정
    virtual ~Block() = default;                             // 다형성 소멸을 위해 virtual

    // === getter ===
    const char (*get_shape() const)[4];                     // 현재 회전 상태의 4x4 모양 반환
    int get_angle() const { return angle; }
    int get_x()     const { return x; }
    int get_y()     const { return y; }
    int get_color() const { return color; }
    virtual std::string get_display_text() const { return "■"; }   // 콘솔에 그릴 문자

    // === 이동 / 회전 ===
    void move_left()    { x--; }
    void move_right()   { x++; }
    void move_down()    { y++; }
    void move_up()      { y--; }                            // 충돌 시 롤백 전용
    void rotate()       { angle = (angle + 1) % 4; }
    void back_rotate()  { angle = (angle + 3) % 4; }        // 회전 충돌 시 롤백 전용

protected:
    // === setter (파생 클래스 전용) ===
    void set_shape(int s) { shape = s; }
    void set_angle(int a) { angle = a; }
    void set_x(int new_x) { x = new_x; }
    void set_y(int new_y) { y = new_y; }
    void set_color(int c) { color = c; }
};
