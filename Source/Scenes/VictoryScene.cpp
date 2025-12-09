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
    GetGame()->SaveGame(); 

    auto* backgroundActor = new Background(GetGame(), Game::ResolvePath("Assets/VictoryBackground.png"));
    mSceneActors.push_back(backgroundActor);

    const Vector2 buttonSize(300.0f, 40.0f);
    const float windowCenterX = Game::WINDOW_WIDTH / 2.0f;
    const float windowCenterY = Game::WINDOW_HEIGHT / 2.0f;

    Actor* mTitleActor = new Actor(GetGame());
    mTitleActor->SetPosition(Vector2(windowCenterX, windowCenterY - 150.0f));
    auto* titleText = new UITextComponent(mTitleActor);
    titleText->SetFont(Game::ResolvePath("Assets/Fonts/Jacquard12-Regular.ttf"));
    titleText->SetText("Victory!", Color::White, 144);

    // Buttons: Store, Level Selection, Main Menu
    const float firstButtonY = windowCenterY + 10.0f;
    const float buttonGap = 60.0f;

    Actor* storeButtonActor = new Actor(GetGame());
    storeButtonActor->SetPosition(Vector2(windowCenterX, firstButtonY));
    auto storeButton = new UIButtonComponent(storeButtonActor, "STORE", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::Store, 0.5f);
        }
        ,
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36
    );
    mButtons.push_back(storeButton);

    Actor* levelSelButtonActor = new Actor(GetGame());
    levelSelButtonActor->SetPosition(Vector2(windowCenterX, firstButtonY + buttonGap));
    auto levelSelButton = new UIButtonComponent(levelSelButtonActor, "SELECT LEVEL", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::LevelSelection, 0.5f);
        }
        ,
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36
    );
    mButtons.push_back(levelSelButton);

    Actor* mainMenuButtonActor = new Actor(GetGame());
    mainMenuButtonActor->SetPosition(Vector2(windowCenterX, firstButtonY + buttonGap * 2));
    auto mainMenuButton = new UIButtonComponent(mainMenuButtonActor, "MAIN MENU", buttonSize,
        [this]() {
            GetGame()->SetScene(Game::GameScene::MainMenu);
        }
        ,
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36
    );
    mButtons.push_back(mainMenuButton);

    mSceneActors.push_back(mTitleActor);
    mSceneActors.push_back(storeButtonActor);
    mSceneActors.push_back(levelSelButtonActor);
    mSceneActors.push_back(mainMenuButtonActor);

    UpdateButtonSelection();

    // Tocar som de vitória ao abrir a tela
    if (GetGame() && GetGame()->GetAudioSystem())
    {
        // Para garantir que a música do último nível pare
        GetGame()->GetAudioSystem()->StopMusic();

        // Toca o efeito de vitória
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/victory.wav"));

        // Inicializa timer para tocar a música de vitória após 3 segundos
        mMusicDelayTimer = 0.0f;
        mPlayedVictoryMusic = false;
    }
}

void VictoryScene::Unload()
{
    for (auto actor : mSceneActors)
    {
        actor->SetState(ActorState::Destroy);
    }
    mSceneActors.clear();
}

void VictoryScene::Update(float deltaTime)
{
    // Atualiza o timer e, após 2 segundos, toca a música de vitória (uma vez)
    if (!mPlayedVictoryMusic && GetGame() && GetGame()->GetAudioSystem())
    {
        mMusicDelayTimer += deltaTime;
        if (mMusicDelayTimer >= 2.0f)
        {
            GetGame()->GetAudioSystem()->PlayMusic(Game::ResolvePath("Assets/Sounds/victory_b6_my_heart_will_stop_in joy.mp3"));
            mPlayedVictoryMusic = true;
        }
    }
}

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

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/select-option.wav"));
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

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/select-option.wav"));
}

void VictoryScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/enter-option.wav"));
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