#pragma once
#include <vector>

struct StageConfig {
    int speed;
    int stick_rate;
    int clear_line;
};

class StageManager {
private:
    static constexpr int stage_count = 10;

    std::vector<StageConfig> stages;
    int current_level;

public:
    StageManager();                         

    const StageConfig& current() const;      
    bool set_level(int level_index);         
    bool advance();                          
    bool should_advance(int cleared_lines) const;

    int level_index() const;                
    int display_level() const;              
    int max_level() const;
};