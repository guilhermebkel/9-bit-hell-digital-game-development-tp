#include "PauseScreen.h"
#include "../Game.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Components/Drawing/RectComponent.h"

PauseScreen::PauseScreen(Game* game)
    : Actor(game)
    , mSelectedButtonIndex(0)
{
    SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f));
    auto* background = new RectComponent(
        this,
        Game::WINDOW_WIDTH,
        Game::WINDOW_HEIGHT,
        RendererMode::TRIANGLES,
        PauseScreen::DRAW_ORDER - 1
    );
    background->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;

    mTitleActor = new Actor(game);
    mTitleActor->SetPosition(Vector2(windowCenterX - 100.0f, windowCenterY - 150.0f));
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

    Actor* quitButtonActor = new Actor(game);
    quitButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY + 50.0f));
    auto quitButton = new UIButtonComponent(quitButtonActor, "EXIT TO MAIN MENU", buttonSize,
        [this]() {
            GetGame()->TogglePause();
            GetGame()->SetScene(Game::GameScene::MainMenu);
        },
        PauseScreen::DRAW_ORDER
    );
    mButtons.push_back(quitButton);

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
}

void PauseScreen::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
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

PauseScreen::~PauseScreen()
{
    for (auto button : mButtons)
    {
        button->GetOwner()->SetState(ActorState::Destroy);
    }
    if (mTitleActor)
    {
        mTitleActor->SetState(ActorState::Destroy);
    }
}