#include "GameOverScene.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "../Actors/Background.h"
#include "../Actors/HUD.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Audio/AudioSystem.h"

GameOverScene::GameOverScene(Game* game)
    : Scene(game)
    , mSelectedButtonIndex(0)
{}

GameOverScene::~GameOverScene() {}

void GameOverScene::Load()
{
    new Background(GetGame(), "../Assets/GameOverBackground.png");

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;

    Actor* mTitleActor = new Actor(GetGame());
    mTitleActor->SetPosition(Vector2(windowCenterX, windowCenterY - 150.0f));
    auto* titleText = new UITextComponent(mTitleActor);
    titleText->SetText("GAME OVER", Color::White, 48);

    Actor* startButtonActor = new Actor(GetGame());
    startButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY));
    auto startButton = new UIButtonComponent(startButtonActor, "TRY AGAIN", buttonSize,
        [this]() {
            GetGame()->ResetCorruptionLevel();
            GetGame()->SetScene(Game::GameScene::Gameplay);
        }
    );
    mButtons.push_back(startButton);

    Actor* quitButtonActor = new Actor(GetGame());
    quitButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY + 50.0f));
    auto quitButton = new UIButtonComponent(quitButtonActor, "EXIT TO MAIN MENU", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::MainMenu);
        }
    );
    mButtons.push_back(quitButton);

    UpdateButtonSelection();
}

void GameOverScene::Unload()
{
    for (auto actor : mSceneActors)
    {
        actor->SetState(ActorState::Destroy);
    }
    mSceneActors.clear();
}

void GameOverScene::Update(float deltaTime) {}

void GameOverScene::ProcessInput(const uint8_t* keyState)
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

void GameOverScene::SelectNextButton()
{
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
    UpdateButtonSelection();
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void GameOverScene::SelectPreviousButton()
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

void GameOverScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-option.wav");
        mButtons[mSelectedButtonIndex]->Click();
    }
}

void GameOverScene::UpdateButtonSelection()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }
}