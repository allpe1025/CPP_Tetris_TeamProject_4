#pragma once
#include "Game.h"
class SkillGame :
    public Game
{
private:
    // 스킬 보유 여부 (스킬이 이미 있으면 추가 보유 불가)
    bool has_q = false;
    bool has_w = false;
    bool has_e = false;

    // 스킬 획득 조건 충족 확인용 변수
    int q_count = 0;    // q스킬 : 부순 블록 수 (36개(4줄))
    int w_count = 0;    // w스킬 : 배치한 특정 블록 수 (5개)
    int w_shape = 0;    // w스킬을 얻기 위해 배치해야 하는 블록의 종류


public:
    SkillGame(IRenderer& r, InputManager& i);
    virtual ~SkillGame() = default;

    void handle_input(char c = 0) override;
    void on_block_landed() override;
    void reset_state(int start_level) override;
    void render_frame();

private:
    void use_skill_q();
    void use_skill_w();
    void use_skill_e();
};

