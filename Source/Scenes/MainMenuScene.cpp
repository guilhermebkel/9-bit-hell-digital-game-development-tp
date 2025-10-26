#include "MainMenuScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Components/Drawing/UIButtonComponent.h"

MainMenuScene::MainMenuScene(Game* game) :
    Scene(game)
    , mSelectedButtonIndex(0)
{}

void MainMenuScene::Load()
{
    new Background(GetGame(), "../Assets/MenuBackground.png");

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;

    Actor* startButtonActor = new Actor(GetGame());
    startButtonActor->SetPosition(Vector2(windowCenterX, 350.0f));
    auto startButton = new UIButtonComponent(startButtonActor, "START GAME", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::Gameplay);
        }
    );
    mButtons.push_back(startButton);

    Actor* quitButtonActor = new Actor(GetGame());
    quitButtonActor->SetPosition(Vector2(windowCenterX, 400.0f));
    auto quitButton = new UIButtonComponent(quitButtonActor, "EXIT", buttonSize,
        [this]() {
            GetGame()->Quit();
        }
    );
    mButtons.push_back(quitButton);

    UpdateButtonSelection();
}

void MainMenuScene::Unload()
{
}

void MainMenuScene::Update(float deltaTime)
{
}

void MainMenuScene::ProcessInput(const uint8_t* keyState)
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

void MainMenuScene::SelectNextButton()
{
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
    UpdateButtonSelection();
}

void MainMenuScene::SelectPreviousButton()
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

void MainMenuScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        mButtons[mSelectedButtonIndex]->Click();
    }
}

void MainMenuScene::UpdateButtonSelection()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }
}