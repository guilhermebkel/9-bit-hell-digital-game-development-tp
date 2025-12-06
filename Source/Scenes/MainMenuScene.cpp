#include "MainMenuScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Audio/AudioSystem.h"

MainMenuScene::MainMenuScene(Game* game) :
    Scene(game)
    , mSelectedButtonIndex(0)
{}

void MainMenuScene::Load()
{
    // Toca a música de fundo do menu em loop
    GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/menu-d5-stage2-the-way-ahead-feels-lonely.mp3");

    GetGame()->ResetPlayerUpgrades();
    GetGame()->ResetCorruptionLevel();

    new Background(GetGame(), "../Assets/MenuBackground.png");

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;

    Actor* mTitleActor = new Actor(GetGame());
    mTitleActor->SetPosition(Vector2(windowCenterX, windowCenterY - 150.0f));
    auto* titleText = new UITextComponent(mTitleActor);
    titleText->SetText("9-BIT HELL", Color::White, 48);

    Actor* startButtonActor = new Actor(GetGame());
    startButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY));
    auto startButton = new UIButtonComponent(startButtonActor, "START GAME", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::Gameplay);
        }
    );
    mButtons.push_back(startButton);

    Actor* quitButtonActor = new Actor(GetGame());
    quitButtonActor->SetPosition(Vector2(windowCenterX, windowCenterY + 50.0f));
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
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
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
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void MainMenuScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        /**
         * TODO:
         * - Using a more robust validation to avoid breaking this business rule in case the index of 'Start Button' changes.
         */
        bool clickedOnStartGameButton = mSelectedButtonIndex == 0;

        if (clickedOnStartGameButton)
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/start-game.wav");
        } else
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-option.wav");
        }

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