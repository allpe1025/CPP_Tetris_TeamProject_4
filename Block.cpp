#include "Block.h"
#include <cstdlib>
#include <ctime>

using namespace std;

Block::Block(int stick_rate)
{
	int i = rand() % 100;
	if (i <= stick_rate) {		//막대기 나올확률 계산
		m_shape = 0;			//막대기 모양으로 초기화
	}
	m_shape = (rand() % 6) + 1;	//m_shape에는 1~6의 값이 들어감
	m_angle = 0;
	m_x = START_X;
	m_y = START_Y;
	m_color = m_shape + 9;
}

const auto Block::get_shape() const
{
	return block[m_shape][m_angle];
}