#include "SpecialBlock.h"

SpecialBlock::SpecialBlock()
	:Block(0)
{
	m_type = rand() % 3;
}

std::string SpecialBlock::get_display_text() const
{
	switch (m_type)
	{
	case 0:
		return "★";		//특수블록 1 : 기획 필요
	case 1:
		return "";		//특수블록 2 : 기획 필요
	case 2:
		return "";		//특수블록 3 : 기획 필요
	default:
		return "■";
	}
}