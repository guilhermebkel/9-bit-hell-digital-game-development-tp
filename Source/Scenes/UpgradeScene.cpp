#include "UpgradeScene.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "../Actors/UIStatWidget.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"

UpgradeScene::UpgradeScene(Game* game)
    : Scene(game)
    , mCoinWidget(nullptr)
    , mSelectedButtonIndex(0)
{
}

UpgradeScene::~UpgradeScene()
{
    delete mCoinWidget;
}

void UpgradeScene::Load()
{
    const float centerX = Game::WINDOW_WIDTH / 2.0f;

    auto* bgActor = new Actor(GetGame());
    bgActor->SetPosition(Vector2(centerX, Game::WINDOW_HEIGHT / 2.0f));
    auto* bgRect = new RectComponent(bgActor, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, RendererMode::TRIANGLES);
    bgRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.85f));

    auto* titleActor = new Actor(GetGame());
    auto* titleText = new UITextComponent(titleActor);
    titleText->SetText("SCORE", Color::White, 48);
    titleActor->SetPosition(Vector2(centerX, 100.0f));

    mCoinWidget = new UIStatWidget(GetGame(), "COINS", 24);
    mCoinWidget->SetOutline(true);
    mCoinWidget->SetPosition(Vector2(20.0f, 32.0f), HAlign::Left);
    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));

    const Vector2 buttonSize(400.0f, 40.0f);

    auto* fireRateActor = new Actor(GetGame());
    fireRateActor->SetPosition(Vector2(centerX, 250.0f));
    auto fireRateButton = new UIButtonComponent(fireRateActor, "Shooting Cadence (Price: 10)", buttonSize,
        [this]() {
            if (GetGame()->CanAfford(10))
            {
                GetGame()->SpendCoins(10);
                GetGame()->UpgradeFireRate();
            }
        }
    );
    mButtons.push_back(fireRateButton);

    auto* nextLevelActor = new Actor(GetGame());
    nextLevelActor->SetPosition(Vector2(centerX, Game:: WINDOW_HEIGHT - 100.0f));
    auto nextLevelButton = new UIButtonComponent(nextLevelActor, "CONTINUE", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::Gameplay);
        }
    );
    mButtons.push_back(nextLevelButton);

    UpdateButtonSelection();
}

void UpgradeScene::Unload()
{
}

void UpgradeScene::Update(float deltaTime)
{
    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));
}

void UpgradeScene::ProcessInput(const uint8_t* keyState)
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

void UpgradeScene::SelectNextButton()
{
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
    UpdateButtonSelection();
}

void UpgradeScene::SelectPreviousButton()
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
}

void UpgradeScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        mButtons[mSelectedButtonIndex]->Click();
    }
}

void UpgradeScene::UpdateButtonSelection()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }
}