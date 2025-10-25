#pragma once
#include "Actor.h"
#include <vector>
#include "HUD.h"

class PauseScreen : public Actor
{
public:
    static constexpr float DRAW_ORDER = HUD::DRAW_ORDER + 2; // Higher than HUD

    explicit PauseScreen(class Game* game);
    ~PauseScreen();

    void OnProcessInput(const uint8_t* keyState) override;

private:
    void SelectNextButton();
    void SelectPreviousButton();
    void ClickSelectedButton();
    void UpdateButtonSelection();

    size_t mSelectedButtonIndex;

    std::vector<class UIButtonComponent*> mButtons;
    class Actor* mTitleActor;

    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
};
