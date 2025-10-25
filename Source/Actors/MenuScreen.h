#pragma once
#include "Actor.h"
#include <vector>

class MenuScreen : public Actor
{
public:
    explicit MenuScreen(class Game* game);
    ~MenuScreen() override;

    void OnProcessInput(const uint8_t* keyState) override;

private:
    void SelectNextButton();
    void SelectPreviousButton();
    void ClickSelectedButton();
    void UpdateButtonSelection();

    std::vector<class UIButtonComponent*> mButtons;
    size_t mSelectedButtonIndex;
    
    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
};