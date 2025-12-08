#include "InitialDifficultyScene.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "../Actors/Background.h"
#include "../Components/Drawing/UIImageButtonComponent.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Audio/AudioSystem.h"
#include <array>

InitialDifficultyScene::InitialDifficultyScene(Game* game)
    : Scene(game)
{
}

void InitialDifficultyScene::Load()
{
    GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/menu_false_memory_syndrome.mp3");
    // Garantir que o save esteja carregado antes de decidir qual cena iniciar
    GetGame()->LoadGame();
    const float centerX = Game::WINDOW_WIDTH / 2.0f;
    auto* bgActor = new Actor(GetGame());
    bgActor->SetPosition(Vector2(centerX, Game::WINDOW_HEIGHT / 2.0f));
    auto* bgRect = new RectComponent(bgActor, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, RendererMode::TRIANGLES, 80);
    bgRect->SetColor(Vector4(36.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f, 1.0f));

    Actor* titleActor = new Actor(GetGame());
    titleActor->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, 80.0f));
    auto* titleText = new UITextComponent(titleActor);
    titleText->SetFont("../Assets/Fonts/Jacquard12-Regular.ttf");
    titleText->SetText("Select Difficulty", Vector3(1.0f,1.0f,1.0f), 72);

    const float diffStartY = 220.0f;
    const float diffSpacingY = 100.0f;
    const int spriteBase = 16;
    mDifficultyBaseScale = 2.0f;
    std::array<std::string,3> diffImages = {"../Assets/GameDifficulty/easy.png", "../Assets/GameDifficulty/medium.png", "../Assets/GameDifficulty/hard.png"};
    std::array<std::string,3> diffLabels = {"Easy", "Medium", "Hard"};

    float iconCenterOffset = -60.0f;
    float labelCenterOffset = 60.0f;

    for (int d = 0; d < 3; ++d)
    {
        float y = diffStartY + (d * diffSpacingY);
        float iconWidthScaled = spriteBase * mDifficultyBaseScale;
        float iconCenterX = centerX + iconCenterOffset;

        Vector2 iconPos(iconCenterX, y);
        auto* btn = new UIImageButtonComponent(GetGame(), diffImages[d], iconPos, [this, d]() {
            Game::Difficulty gd = static_cast<Game::Difficulty>(d);
            GetGame()->SetDifficulty(gd);
            GetGame()->SaveGame();
            GetGame()->LoadGame();
            mSelectedIndex = d;
            UpdateDifficultySelection();
        }, spriteBase, spriteBase);

        btn->SetScale(Vector2(mDifficultyBaseScale, mDifficultyBaseScale));
        mDifficultyButtons.push_back(btn);

        Actor* labelActor = new Actor(GetGame());
        auto* labelText = new UITextComponent(labelActor);
        labelText->SetFont("../Assets/Fonts/Jersey10-Regular.ttf");
        labelText->SetText(diffLabels[d], Vector3(1.0f,1.0f,1.0f), 36);

        int textW = 0;
        if (labelText->GetTexture()) textW = labelText->GetTexture()->GetWidth();

        float iconLeft = iconCenterX - (iconWidthScaled / 2.0f);
        float labelLeft = iconLeft + iconWidthScaled + 12.0f;
        float labelCenterX = labelLeft + (textW / 2.0f);
        labelActor->SetPosition(Vector2(labelCenterX, y));
        mDifficultyLabels.push_back(labelText);

        Actor* hlActor = new Actor(GetGame());
        float totalWidth = 300.0f;
        float hlCenterX = centerX;
        hlActor->SetPosition(Vector2(hlCenterX, y));
        auto* hlRect = new RectComponent(hlActor, static_cast<int>(totalWidth), 64, RendererMode::TRIANGLES, 95);
        hlRect->SetColor(Vector4(1.0f,1.0f,1.0f,0.08f));
        hlRect->SetVisible(false);
        mDifficultyHighlightActors.push_back(hlActor);
    }

    Actor* startActor = new Actor(GetGame());
    float startY = diffStartY + (3 * diffSpacingY) + 20.0f;
    startActor->SetPosition(Vector2(centerX, startY));
    mStartButton = new UIButtonComponent(startActor, "START", Vector2(180.0f, 52.0f), [this]() {
        if (GetGame()->GetMaxUnlockedLevel() <= 1)
        {
            GetGame()->SetCurrentLevelID(LevelID::Tutorial);
        }
        else
        {
            GetGame()->SetCurrentLevelID(LevelID::Level1);
        }
        GetGame()->SetScene(Game::GameScene::Gameplay);
    }, UIButtonComponent::DEFAULT_DRAW_ORDER, 36);

    Actor* backActor = new Actor(GetGame());
    backActor->SetPosition(Vector2(centerX, startY + 80.0f));
    mBackButton = new UIButtonComponent(backActor, "BACK", Vector2(180.0f, 52.0f), [this]() {
        GetGame()->SetScene(Game::GameScene::MainMenu);
    }, UIButtonComponent::DEFAULT_DRAW_ORDER, 28);

    if (GetGame()->HasSaveFile())
    {
        mSelectedIndex = static_cast<int>(GetGame()->GetDifficulty());
    }
    else
    {
        mSelectedIndex = 3; // START
    }
    UpdateDifficultySelection();
}

void InitialDifficultyScene::Unload()
{
    mDifficultyButtons.clear();
    mDifficultyLabels.clear();
    mDifficultyHighlightActors.clear();
    mStartButton = nullptr;
    mBackButton = nullptr;
}

void InitialDifficultyScene::Update(float deltaTime) {}

void InitialDifficultyScene::ProcessInput(const uint8_t* keyState)
{
    if (keyState[SDL_SCANCODE_W] && !mUpPressed)
    {
        mUpPressed = true;
        mSelectedIndex = (mSelectedIndex - 1 + 5) % 5;
        UpdateDifficultySelection();
    }
    else if (!keyState[SDL_SCANCODE_W])
    {
        mUpPressed = false;
    }

    if (keyState[SDL_SCANCODE_S] && !mDownPressed)
    {
        mDownPressed = true;
        mSelectedIndex = (mSelectedIndex + 1) % 5;
        UpdateDifficultySelection();
    }
    else if (!keyState[SDL_SCANCODE_S])
    {
        mDownPressed = false;
    }

    if (keyState[SDL_SCANCODE_RETURN] && !mEnterPressed)
    {
        mEnterPressed = true;
        if (mSelectedIndex >= 0 && mSelectedIndex <= 2)
        {
            Game::Difficulty gd = static_cast<Game::Difficulty>(mSelectedIndex);
            mSelectedIndex = static_cast<int>(gd);
            GetGame()->SetDifficulty(gd);
            GetGame()->SaveGame();
            GetGame()->LoadGame();
            UpdateDifficultySelection();
        }
        else if (mSelectedIndex == 3)
        {
            if (mStartButton) mStartButton->Click();
        }
        else if (mSelectedIndex == 4)
        {
            if (mBackButton) mBackButton->Click();
        }
    }
    else if (!keyState[SDL_SCANCODE_RETURN])
    {
        mEnterPressed = false;
    }
}

void InitialDifficultyScene::UpdateDifficultySelection()
{
    int focusedIdx = mSelectedIndex;
    int savedIdx = static_cast<int>(GetGame()->GetDifficulty());

    for (int i = 0; i < static_cast<int>(mDifficultyHighlightActors.size()); ++i)
    {
        Actor* hlActor = mDifficultyHighlightActors[i];
        if (hlActor)
        {
            RectComponent* rc = hlActor->GetComponent<RectComponent>();
            if (rc)
            {
                bool isFocused = (i == focusedIdx);
                bool isSaved = (i == savedIdx);
                if (isFocused)
                {
                    rc->SetVisible(true);
                    rc->SetColor(Vector4(1.0f,1.0f,1.0f,0.14f));
                }
                else if (isSaved)
                {
                    rc->SetVisible(true);
                    rc->SetColor(Vector4(1.0f,1.0f,1.0f,0.08f));
                }
                else
                {
                    rc->SetVisible(false);
                }
            }
        }

        if (i < static_cast<int>(mDifficultyButtons.size()) && mDifficultyButtons[i])
        {
            float base = mDifficultyBaseScale;
            float focusedScale = mDifficultyBaseScale * 1.25f;
            mDifficultyButtons[i]->SetScale(Vector2(i == focusedIdx ? focusedScale : base,
                                   i == focusedIdx ? focusedScale : base));
        }
    }

    // Update Start/Back button selected state
    if (mStartButton) mStartButton->SetSelected(focusedIdx == 3);
    if (mBackButton) mBackButton->SetSelected(focusedIdx == 4);
}
