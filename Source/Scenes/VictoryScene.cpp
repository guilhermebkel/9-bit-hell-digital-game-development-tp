#include "VictoryScene.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "../Actors/Background.h"
#include "../Actors/HUD.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Audio/AudioSystem.h"

VictoryScene::VictoryScene(Game* game)
    : Scene(game)
    , mSelectedButtonIndex(0)
{}

VictoryScene::~VictoryScene() {}

void VictoryScene::Load()
{
    new Background(GetGame(), "../Assets/VictoryBackground.png");

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;

    Actor* mTitleActor = new Actor(GetGame());
    mTitleActor->SetPosition(Vector2(windowCenterX, windowCenterY - 150.0f));
    auto* titleText = new UITextComponent(mTitleActor);
    titleText->SetText("VICTORY!", Color::White, 48);

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

void VictoryScene::Unload()
{
    for (auto actor : mSceneActors)
    {
        actor->SetState(ActorState::Destroy);
    }
    mSceneActors.clear();
}

void VictoryScene::Update(float deltaTime) {}

void VictoryScene::ProcessInput(const uint8_t* keyState)
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

void VictoryScene::SelectNextButton()
{
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();

    UpdateButtonSelection();

    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void VictoryScene::SelectPreviousButton()
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

void VictoryScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-option.wav");
        mButtons[mSelectedButtonIndex]->Click();
    }
}

void VictoryScene::UpdateButtonSelection()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }
}