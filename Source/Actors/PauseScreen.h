#pragma once
#include "Actor.h"
#include <vector>
#include "HUD.h"

class UIStatWidget;

class PauseScreen : public Actor
{
public:
    static constexpr float DRAW_ORDER = HUD::DRAW_ORDER + 20; // Higher than HUD

    explicit PauseScreen(class Game* game);
    ~PauseScreen();

    void OnProcessInput(const uint8_t* keyState) override;
    
    void DestroyChildActors();

private:
    void SelectNextButton();
    void SelectPreviousButton();
    void ClickSelectedButton();
    void UpdateButtonSelection();

    size_t mSelectedButtonIndex;

    std::vector<class UIButtonComponent*> mButtons;
    std::vector<class Actor*> mButtonActors;  // Armazenar referências dos atores dos botões
    class Actor* mTitleActor;
    
    // Status widgets for cleanup
    UIStatWidget* mHealthWidget;
    UIStatWidget* mMeleeWidget;
    UIStatWidget* mRangedWidget;
    UIStatWidget* mFireRateWidget;
    UIStatWidget* mSpeedWidget;
    UIStatWidget* mPiercingWidget;

    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
};
