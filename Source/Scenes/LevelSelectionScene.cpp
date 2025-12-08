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

#include "../Components/Drawing/ShapeComponent.h"
#include "../Components/Drawing/SolidShapeComponent.h"
#include "../Components/Drawing/StaticSpriteComponent.h"

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
    GetGame()->LoadGame();

    new Background(GetGame(), "../Assets/LevelSelectionBackground.png");

    Actor* mTitleActor = new Actor(GetGame());
    mTitleActor->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, 80.0f));
    auto* titleText = new UITextComponent(mTitleActor);
    titleText->SetFont("../Assets/Fonts/Jacquard12-Regular.ttf");
    titleText->SetText("Select Level", Vector3(1.0f, 1.0f, 1.0f), 72);

    Actor* gridManager = new Actor(GetGame());
    auto* shapeLines = new ShapeComponent(gridManager, 90);
    shapeLines->SetColor(Vector3(0.2f, 0.2f, 0.2f));

    const float startY = 180.0f;
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

        shapeLines->AddLine(Vector2(leftX, currentY), Vector2(rightX, currentY), lineThickness);

        if (i > 0)
        {
            shapeLines->AddLine(Vector2(prevLeftX, prevY), Vector2(leftX, currentY), lineThickness);
            shapeLines->AddLine(Vector2(prevRightX, prevY), Vector2(rightX, currentY), lineThickness);

            Actor* highlightActor = new Actor(GetGame());
            highlightActor->SetPosition(Vector2::One);

            std::vector<Vector2> points = {
                Vector2(prevLeftX, prevY),
                Vector2(prevRightX, prevY),
                Vector2(rightX, currentY),
                Vector2(leftX, currentY)
            };

            auto* solidShape = new SolidShapeComponent(highlightActor, points, Vector4(0.1f, 0.1f, 0.1f, 1.0f), 80);
            solidShape->SetVisible(false);

            mHighlightActors.push_back(highlightActor);
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

        int maxUnlockedLevel = GetGame()->GetMaxUnlockedLevel();
        int currentLevelIndex = static_cast<int>(i) + 1;
        bool isLocked = currentLevelIndex > maxUnlockedLevel;

        int imageSpriteWidth = 20 * 2.5f;
        int imageSpriteHeight = 11 * 2.5f;
        auto* btn = new UIImageButtonComponent(GetGame(), path, pos, [this, id = levels[i].id, isLocked]() {
            if (!isLocked)
            {
                GetGame()->SetCurrentLevelID(id);
                GetGame()->SetScene(Game::GameScene::Gameplay);
            }

        }, imageSpriteWidth, imageSpriteHeight);

        if (isLocked)
        {
            btn->GetSprite()->SetColor(Vector3(0.4f, 0.4f, 0.4f));
        }

        mLevelButtons.push_back(btn);
    }

    mBackButtonActor = new Actor(GetGame());
    mBackButtonActor->SetPosition(Vector2(centerX, currentY + 10.0f));

    mBackButton = new UIButtonComponent(mBackButtonActor, "BACK", Vector2(200.0f, 50.0f), [this]() {
        GetGame()->SetScene(Game::GameScene::MainMenu);
    }, UIButtonComponent::DEFAULT_DRAW_ORDER, 32);

    mTotalButtons = static_cast<int>(mLevelButtons.size()) + 1;
    UpdateButtonSelection();
}

void LevelSelectionScene::Unload()
{
    mLevelButtons.clear();
    mHighlightActors.clear();
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
    if (mSelectedButtonIndex < mLevelButtons.size())
    {
        mLevelButtons[mSelectedButtonIndex]->Click();
    }
    else if (mSelectedButtonIndex == mLevelButtons.size())
    {
        mBackButton->Click();
    }

    bool clickedOnAnyLevelButton = mSelectedButtonIndex < mLevelButtons.size();

    if (clickedOnAnyLevelButton)
    {
        int maxUnlockedLevel = GetGame()->GetMaxUnlockedLevel();
        int currentLevelIndex = mSelectedButtonIndex + 1;
        bool isLocked = currentLevelIndex > maxUnlockedLevel;

        if (isLocked)
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/error.mp3");
        } else
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-level.wav");
        }
    } else
    {
        GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-option.wav");
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

    for (size_t i = 0; i < mHighlightActors.size(); ++i)
    {
        bool isSelected = (i == mSelectedButtonIndex);

        auto comp = mHighlightActors[i]->GetComponent<SolidShapeComponent>();
        if (comp)
        {
            comp->SetVisible(isSelected);
        }
    }
}