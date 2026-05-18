#pragma once
#include <string>
class Block
{
private:
	//block 모양 배열
	const static inline char block[7][4][4][4] = {
		//막대모양
		1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,	1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,	1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,

		//네모모양
		1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,

		//'ㅓ' 모양
		0,1,0,0,1,1,0,0,0,1,0,0,0,0,0,0,	1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,	0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,

		//'ㄱ'모양
		1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,	1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,	0,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,

		//'ㄴ' 모양
		1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,	1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,0,1,0,0,1,1,0,0,0,0,0,0,	1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,

		//'Z' 모양
		1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,	1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,	0,1,0,0,1,1,0,0,1,0,0,0,0,0,0,0,

		//'S' 모양
		0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,	0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,	1,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0

	};
	const static inline int START_X = 5, START_Y = -4;	//시작 위치

	int m_shape, m_angle, m_x, m_y, m_color;	//멤버 변수

public:
	//========= 생성자 및 소멸자 =========
	Block() = delete;				//디폴트 생성자 명시적 삭제
	Block(int stick_rate);	//막대기 확률을 인자로 받는 생성자
	~Block() = default;		//역할 없는 소멸자

	//========= getter 함수 =========
	const auto get_shape() const;	//현재 모양을 리턴하는 함수
	int get_angle() const { return m_angle; }
	int get_x() const { return m_x; }
	int get_y() const { return m_y; }
	int get_color() const { return m_color; }
	virtual std::string get_display_text() const { return "■"; }	//출력되는 문자 형태

	//========= 이동 및 회전 함수 =========
	void move_left() { m_x--; }	
	void move_right() { m_x++; }	
	void move_down() { m_y++; }
	void move_up() { m_y--; }	//바닥 충돌 판정 시 위치 되돌리기용
	void rotate() { m_angle = (m_angle + 1) % 4; }
	void back_rotate() { m_angle = (m_angle + 3) % 4; }	//회전 불가능 시 되돌리기용

protected:
	//========= setter 함수 =========
	void set_shape(int shape) { m_shape = shape; }
	void set_angle(int angle) { m_angle = angle; }
	void set_x(int x) { m_x = x; }
	void set_y(int y) { m_y = y; }
	void set_color(int color) { m_color = color; }
};