#include "PauseScreen.h"
#include "../Game.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Audio/AudioSystem.h"
#include "UIStatWidget.h"

PauseScreen::PauseScreen(Game* game)
    : Actor(game)
    , mSelectedButtonIndex(0)
    , mHealthWidget(nullptr)
    , mMeleeWidget(nullptr)
    , mRangedWidget(nullptr)
    , mFireRateWidget(nullptr)
    , mSpeedWidget(nullptr)
    , mPiercingWidget(nullptr)
{
    SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f));
    auto* background = new RectComponent(
        this,
        Game::WINDOW_WIDTH,
        Game::WINDOW_HEIGHT,
        RendererMode::TRIANGLES,
        PauseScreen::DRAW_ORDER + 1
    );
    background->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.8f));

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;
    const float buttonSpacing = 70.0f;

    mTitleActor = new Actor(game);
    mTitleActor->SetPosition(Vector2(windowCenterX, windowCenterY - 150.0f));
    auto* titleText = new UITextComponent(mTitleActor, PauseScreen::DRAW_ORDER + 2);
    titleText->SetFont(Game::ResolvePath("Assets/Fonts/Jacquard12-Regular.ttf"));
    titleText->SetText("Paused", Color::White, 72);

    Actor* resumeButtonActor = new Actor(game);
    resumeButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY));
    auto resumeButton = new UIButtonComponent(resumeButtonActor, "RESUME", buttonSize,
        [this]() {
            GetGame()->TogglePause();
        },
        PauseScreen::DRAW_ORDER + 3,
        36
    );
    mButtons.push_back(resumeButton);
    mButtonActors.push_back(resumeButtonActor);

    Actor* quitButtonActor = new Actor(game);
    quitButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY + buttonSpacing));
    auto quitButton = new UIButtonComponent(quitButtonActor, "EXIT TO MAIN MENU", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::MainMenu);
            GetGame()->TogglePause();
        },
        PauseScreen::DRAW_ORDER + 4,
        36
    );
    mButtons.push_back(quitButton);
    mButtonActors.push_back(quitButtonActor);

    const float quitButtonY = windowCenterY + buttonSpacing;
    const float statusStartY = quitButtonY + 120.0f;
    const float statusSpacing = 90.0f;
    const float columnGap = 220.0f;
    const float colX0 = windowCenterX - columnGap;
    const float colX1 = windowCenterX;
    const float colX2 = windowCenterX + columnGap;

    mHealthWidget = new UIStatWidget(GetGame(), "HEALTH", 30, PauseScreen::DRAW_ORDER + 5);
    mHealthWidget->SetPosition(Vector2(colX0, statusStartY));
    mHealthWidget->SetValue(std::to_string(GetGame()->GetPlayerHealth()) + "/" + std::to_string(GetGame()->GetPlayerMaxHealth()));

    mMeleeWidget = new UIStatWidget(GetGame(), "MELEE ATK", 30, PauseScreen::DRAW_ORDER + 6);
    mMeleeWidget->SetPosition(Vector2(colX1, statusStartY));
    mMeleeWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerMeleeDamage())));

    mFireRateWidget = new UIStatWidget(GetGame(), "FIRE RATE", 30, PauseScreen::DRAW_ORDER + 7);
    mFireRateWidget->SetPosition(Vector2(colX2, statusStartY));
    {
        char fireRateStr[16];
        snprintf(fireRateStr, sizeof(fireRateStr), "%.2f/s", 1.0f / GetGame()->GetPlayerFireRate());
        mFireRateWidget->SetValue(fireRateStr);
    }

    mSpeedWidget = new UIStatWidget(GetGame(), "SPEED", 30, PauseScreen::DRAW_ORDER + 8);
    mSpeedWidget->SetPosition(Vector2(colX0, statusStartY + statusSpacing));
    mSpeedWidget->SetValue(GetGame()->GetSpeedDisplayValue());

    mRangedWidget = new UIStatWidget(GetGame(), "RANGED ATK", 30, PauseScreen::DRAW_ORDER + 9);
    mRangedWidget->SetPosition(Vector2(colX1, statusStartY + statusSpacing));
    mRangedWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerRangedDamage())));

    mPiercingWidget = new UIStatWidget(GetGame(), "PIERCING", 30, PauseScreen::DRAW_ORDER + 10);
    mPiercingWidget->SetPosition(Vector2(colX2, statusStartY + statusSpacing));
    mPiercingWidget->SetValue(std::to_string(GetGame()->GetPlayerPiercing()));

    UpdateButtonSelection();
}

void PauseScreen::OnProcessInput(const uint8_t* keyState)
{
    if (keyState[SDL_SCANCODE_W] && !mUpPressed)
    {
        mUpPressed = true;
        SelectPreviousButton();
    }
    else if (!keyState[SDL_SCANCODE_W])
    {
        mUpPressed = false;
    }

    if (keyState[SDL_SCANCODE_S] && !mDownPressed)
    {
        mDownPressed = true;
        SelectNextButton();
    }
    else if (!keyState[SDL_SCANCODE_S])
    {
        mDownPressed = false;
    }

    if (keyState[SDL_SCANCODE_RETURN] && !mEnterPressed)
    {
        mEnterPressed = true;
        ClickSelectedButton();
    }
    else if (!keyState[SDL_SCANCODE_RETURN])
    {
        mEnterPressed = false;
    }
}

void PauseScreen::SelectNextButton()
{
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
    UpdateButtonSelection();
    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/select-option.wav"));
}

void PauseScreen::SelectPreviousButton()
{
    if (mSelectedButtonIndex == 0)
    {
        mSelectedButtonIndex = mButtons.size() - 1;
    }
    else
    {
        mSelectedButtonIndex--;
    }
    UpdateButtonSelection();
    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/select-option.wav"));
}

void PauseScreen::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/enter-option.wav"));
        mButtons[mSelectedButtonIndex]->Click();
    }
}

void PauseScreen::UpdateButtonSelection()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }
}

void PauseScreen::DestroyChildActors()
{
    // Remover e deletar atores filhos
    if (mTitleActor)
    {
        GetGame()->RemoveActor(mTitleActor);
        delete mTitleActor;
        mTitleActor = nullptr;
    }
    
    for (auto actor : mButtonActors)
    {
        if (actor)
        {
            GetGame()->RemoveActor(actor);
            delete actor;
        }
    }
    mButtonActors.clear();
    
    // Delete status widgets
    if (mHealthWidget)
    {
        delete mHealthWidget;
        mHealthWidget = nullptr;
    }
    if (mMeleeWidget)
    {
        delete mMeleeWidget;
        mMeleeWidget = nullptr;
    }
    if (mRangedWidget)
    {
        delete mRangedWidget;
        mRangedWidget = nullptr;
    }
    if (mFireRateWidget)
    {
        delete mFireRateWidget;
        mFireRateWidget = nullptr;
    }
    if (mSpeedWidget)
    {
        delete mSpeedWidget;
        mSpeedWidget = nullptr;
    }
    if (mPiercingWidget)
    {
        delete mPiercingWidget;
        mPiercingWidget = nullptr;
    }
}

PauseScreen::~PauseScreen()
{
    DestroyChildActors();
}