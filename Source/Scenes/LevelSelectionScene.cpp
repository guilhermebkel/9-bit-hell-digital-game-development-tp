#include "LevelSelectionScene.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "../Actors/Background.h"
#include "../Components/Drawing/UIImageButtonComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Audio/AudioSystem.h"
#include <array>

LevelSelectionScene::LevelSelectionScene(Game* game)
    : Scene(game)
    , mBackButton(nullptr)
    , mBackButtonActor(nullptr)
    , mTotalButtons(0)
    , mSelectedButtonIndex(0)
{}

LevelSelectionScene::~LevelSelectionScene()
{
}

void LevelSelectionScene::Load()
{
    new Background(GetGame(), "../Assets/LevelSelectionBackground.png");

    Actor* mTitleActor = new Actor(GetGame());
    mTitleActor->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, 80.0f));
    auto* titleText = new UITextComponent(mTitleActor);
    titleText->SetText("SELECT LEVEL", Vector3(1.0f, 1.0f, 1.0f), 48);

    auto CreateLine = [&](Vector2 start, Vector2 end, float thickness)
    {
        Vector2 diff = end - start;
        float length = diff.Length();
        float angle = Math::Atan2(diff.y, diff.x);
        Vector2 center = start + (diff * 0.5f);

        Actor* lineActor = new Actor(GetGame());
        lineActor->SetPosition(center);
        lineActor->SetRotation(angle);

        auto* rect = new RectComponent(lineActor, static_cast<int>(length), static_cast<int>(thickness), RendererMode::TRIANGLES, 90);

        rect->SetColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f));
    };

    const float startY = 188.0f;
    const float spacingY = 55.0f;
    const float centerX = Game::WINDOW_WIDTH / 2.0f;
    const float topWidth = 500.0f;
    const float bottomWidth = 200.0f;
    const float lineThickness = 3.0f;

    int numHorizontalLines = 10;
    float currentY = startY - (spacingY / 2.0f);

    float prevLeftX = 0.0f;
    float prevRightX = 0.0f;
    float prevY = 0.0f;

    for (int i = 0; i < numHorizontalLines; ++i)
    {
        float t = static_cast<float>(i) / (numHorizontalLines - 1);
        float currentWidth = Math::Lerp(topWidth, bottomWidth, t);

        float leftX = centerX - (currentWidth / 2.0f);
        float rightX = centerX + (currentWidth / 2.0f);

        CreateLine(Vector2(leftX, currentY), Vector2(rightX, currentY), lineThickness);

        if (i > 0)
        {
            CreateLine(Vector2(prevLeftX, prevY), Vector2(leftX, currentY), lineThickness);
            CreateLine(Vector2(prevRightX, prevY), Vector2(rightX, currentY), lineThickness);
        }

        prevLeftX = leftX;
        prevRightX = rightX;
        prevY = currentY;

        currentY += spacingY;
    }

    struct LevelData {
        std::string suffix;
        LevelID id;
    };

    std::array<LevelData, 9> levels = {{
        {"I",    LevelID::Level1}, {"II",   LevelID::Level2}, {"III",  LevelID::Level3},
        {"IV",   LevelID::Level4}, {"V",    LevelID::Level5}, {"VI",   LevelID::Level6},
        {"VII",  LevelID::Level7}, {"VIII", LevelID::Level8}, {"IX",   LevelID::Level9}
    }};

    for (size_t i = 0; i < levels.size(); ++i)
    {
        std::string path = "../Assets/MenuLevelNumbers/" + levels[i].suffix + ".png";

        Vector2 pos(centerX, startY + (i * spacingY));

        int imageSpriteWidth = 20 * 2.5f;
        int imageSpriteHeight = 11 * 2.5f;
        auto* btn = new UIImageButtonComponent(GetGame(), path, pos, [this, id = levels[i].id]() {
            GetGame()->SetCurrentLevelID(id);
            GetGame()->SetScene(Game::GameScene::Gameplay);
        }, imageSpriteWidth, imageSpriteHeight);

        mLevelButtons.push_back(btn);
    }

    mBackButtonActor = new Actor(GetGame());
    mBackButtonActor->SetPosition(Vector2(centerX, currentY + 20.0f));

    mBackButton = new UIButtonComponent(mBackButtonActor, "BACK", Vector2(150.0f, 40.0f), [this]() {
        GetGame()->SetScene(Game::GameScene::MainMenu);
    });

    mTotalButtons = static_cast<int>(mLevelButtons.size()) + 1;
    UpdateButtonSelection();
}

void LevelSelectionScene::Unload()
{
    mLevelButtons.clear();
    mBackButton = nullptr;
    mBackButtonActor = nullptr;
}

void LevelSelectionScene::Update(float deltaTime) {}

void LevelSelectionScene::ProcessInput(const uint8_t* keyState)
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

void LevelSelectionScene::SelectNextButton()
{
    if (mTotalButtons == 0) return;
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mTotalButtons;
    UpdateButtonSelection();
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void LevelSelectionScene::SelectPreviousButton()
{
    if (mTotalButtons == 0) return;
    if (mSelectedButtonIndex == 0)
    {
        mSelectedButtonIndex = mTotalButtons - 1;
    }
    else
    {
        mSelectedButtonIndex--;
    }
    UpdateButtonSelection();
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void LevelSelectionScene::ClickSelectedButton()
{
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-option.wav");

    if (mSelectedButtonIndex < mLevelButtons.size())
    {
        mLevelButtons[mSelectedButtonIndex]->Click();
    }
    else if (mSelectedButtonIndex == mLevelButtons.size())
    {
        mBackButton->Click();
    }
}

void LevelSelectionScene::UpdateButtonSelection()
{
    for (size_t i = 0; i < mLevelButtons.size(); ++i)
    {
        mLevelButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }

    if (mBackButton)
    {
        bool isBackSelected = (mSelectedButtonIndex == mLevelButtons.size());
        mBackButton->SetSelected(isBackSelected);
    }
}