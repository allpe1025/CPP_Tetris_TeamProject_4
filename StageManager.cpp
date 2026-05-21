#include "StageManager.h"

StageManager::StageManager()
    : stages{
        // {speed, stick_rate(%), clear_line} — 원본 tetris.cpp 의 stage_data 동일
        StageConfig{40, 20, 20},
        StageConfig{38, 18, 20},
        StageConfig{35, 18, 20},
        StageConfig{30, 17, 20},
        StageConfig{25, 16, 20},
        StageConfig{20, 14, 20},
        StageConfig{15, 14, 20},
        StageConfig{10, 13, 20},
        StageConfig{6,  12, 20},
        StageConfig{4,  11, 99999}              // 마지막 단계: 사실상 무한 (목표 도달 불가)
    },
    current_level(0)
{}

const StageConfig& StageManager::current() const
{
    return stages[current_level];
}

bool StageManager::set_level(int level_index)
{
    if (level_index < 0 || level_index >= stage_count) return false;
    current_level = level_index;
    return true;
}

bool StageManager::advance()
{
    if (current_level + 1 >= stage_count) return false; // 마지막 단계면 그대로 머무름
    current_level++;
    return true;
}

bool StageManager::should_advance(int cleared_lines) const
{
    return cleared_lines >= current().clear_line;
}

int StageManager::level_index() const
{
    return current_level;
}

int StageManager::display_level() const
{
    return current_level + 1;                   // 사용자에겐 1-based
}

int StageManager::max_level() const
{
    return stage_count;
}
