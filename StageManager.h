#pragma once
#include <vector>

// 한 스테이지의 난이도 파라미터.
//   speed       : 자동 낙하 간격 (프레임 수, 작을수록 빠름)
//   stick_rate  : 막대(I) 등장 확률(%)
//   clear_line  : 다음 스테이지로 넘어가기 위해 지워야 하는 줄 수
struct StageConfig {
    int speed;
    int stick_rate;
    int clear_line;
};

// 스테이지 진행 상태를 관리한다.
// Game 은 현재 난이도와 레벨 정보를 이 클래스를 통해서만 얻는다.
class StageManager {
private:
    static constexpr int stage_count = 10;

    std::vector<StageConfig> stages;
    int current_level;                              // 0-based 내부 인덱스

public:
    StageManager();

    const StageConfig& current() const;             // 현재 레벨의 난이도 설정
    bool set_level(int level_index);                // 0-based로 레벨 지정. 범위 밖이면 false
    bool advance();                                 // 다음 레벨로 진행. 마지막이면 false
    bool should_advance(int cleared_lines) const;   // 누적 라인이 목표 도달했는지

    int level_index()   const;                      // 0-based 내부 인덱스
    int display_level() const;                      // 사용자 표기용 1-based
    int max_level()     const;
};
