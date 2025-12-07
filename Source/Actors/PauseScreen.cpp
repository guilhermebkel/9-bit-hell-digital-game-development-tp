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
        PauseScreen::DRAW_ORDER
    );
    background->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;

    mTitleActor = new Actor(game);
    mTitleActor->SetPosition(Vector2(windowCenterX, windowCenterY - 150.0f));
    auto* titleText = new UITextComponent(mTitleActor, PauseScreen::DRAW_ORDER);
    titleText->SetText("PAUSED", Color::White, 48);

    Actor* resumeButtonActor = new Actor(game);
    resumeButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY));
    auto resumeButton = new UIButtonComponent(resumeButtonActor, "RESUME", buttonSize,
        [this]() {
            GetGame()->TogglePause();
        },
        PauseScreen::DRAW_ORDER
    );
    mButtons.push_back(resumeButton);
    mButtonActors.push_back(resumeButtonActor);

    Actor* quitButtonActor = new Actor(game);
    quitButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY + 50.0f));
    auto quitButton = new UIButtonComponent(quitButtonActor, "EXIT TO MAIN MENU", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::MainMenu);
            GetGame()->TogglePause();
        },
        PauseScreen::DRAW_ORDER
    );
    mButtons.push_back(quitButton);
    mButtonActors.push_back(quitButtonActor);

    // Player status widgets (20% larger text = 14 * 1.2 = 16.8 ≈ 20)
    // Positioned in middle-right area: third column, middle row
    const float statusX = windowCenterX + 250.0f;  // Right side, moved +100px
    const float statusCenterY = windowCenterY;      // Center vertical
    
    mHealthWidget = new UIStatWidget(GetGame(), "HEALTH", 20, PauseScreen::DRAW_ORDER);
    mHealthWidget->SetPosition(Vector2(statusX, statusCenterY - 130.0f));
    mHealthWidget->SetValue(std::to_string(GetGame()->GetPlayerHealth()) + "/" + std::to_string(GetGame()->GetPlayerMaxHealth()));

    mMeleeWidget = new UIStatWidget(GetGame(), "MELEE ATK", 20, PauseScreen::DRAW_ORDER);
    mMeleeWidget->SetPosition(Vector2(statusX, statusCenterY - 65.0f));
    mMeleeWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerMeleeDamage())));

    mRangedWidget = new UIStatWidget(GetGame(), "RANGED ATK", 20, PauseScreen::DRAW_ORDER);
    mRangedWidget->SetPosition(Vector2(statusX, statusCenterY));
    mRangedWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerRangedDamage())));

    mFireRateWidget = new UIStatWidget(GetGame(), "FIRE RATE", 20, PauseScreen::DRAW_ORDER);
    mFireRateWidget->SetPosition(Vector2(statusX, statusCenterY + 65.0f));
    {
        char fireRateStr[16];
        snprintf(fireRateStr, sizeof(fireRateStr), "%.2f/s", 1.0f / GetGame()->GetPlayerFireRate());
        mFireRateWidget->SetValue(fireRateStr);
    }

    mSpeedWidget = new UIStatWidget(GetGame(), "SPEED", 20, PauseScreen::DRAW_ORDER);
    mSpeedWidget->SetPosition(Vector2(statusX, statusCenterY + 130.0f));
    mSpeedWidget->SetValue(GetGame()->GetSpeedDisplayValue());

    mPiercingWidget = new UIStatWidget(GetGame(), "PIERCING", 20, PauseScreen::DRAW_ORDER);
    mPiercingWidget->SetPosition(Vector2(statusX, statusCenterY + 195.0f));
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
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
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
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void PauseScreen::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-option.wav");
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