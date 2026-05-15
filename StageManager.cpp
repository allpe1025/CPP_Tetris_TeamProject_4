#include "StageManager.h"

StageManager::StageManager()
    : stages{
        StageConfig{40, 20, 20},
        StageConfig{38, 18, 20},
        StageConfig{35, 18, 20},
        StageConfig{30, 17, 20},
        StageConfig{25, 16, 20},
        StageConfig{20, 14, 20},
        StageConfig{15, 14, 20},
        StageConfig{10, 13, 20},
        StageConfig{6,  12, 20},
        StageConfig{4,  11, 99999}
    },
    current_level(0)
{}

const StageConfig& StageManager::current() const
{
    return stages[current_level];
}

bool StageManager::set_level(int level_index)
{
    if (level_index < 0 || level_index >= stage_count) {
        return false;
    }

    current_level = level_index;
    return true;
}

bool StageManager::advance()
{
    if (current_level + 1 >= stage_count) {
        return false;
    }

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
    return current_level + 1;
}

int StageManager::max_level() const
{
    return stage_count;
}
