#include "StoreScene.h"
#include "../Game.h"
#include "../Actors/Actor.h"
#include "../Actors/UIStatWidget.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/UIButtonComponent.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Audio/AudioSystem.h"

StoreScene::StoreScene(Game* game)
    : Scene(game)
    , mCoinWidget(nullptr)
    , mHealthWidget(nullptr)
    , mMeleeWidget(nullptr)
    , mRangedWidget(nullptr)
    , mFireRateWidget(nullptr)
    , mSpeedWidget(nullptr)
    , mPiercingWidget(nullptr)
    , mFireRateButton(nullptr)
    , mDamageButton(nullptr)
    , mVelocityButton(nullptr)
    , mPiercingButton(nullptr)
    , mSelectedButtonIndex(0)
{
}

StoreScene::~StoreScene()
{
    delete mCoinWidget;
}

void StoreScene::Load()
{
    GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level-transition.wav");

    const float centerX = Game::WINDOW_WIDTH / 2.0f;

    auto* bgActor = new Actor(GetGame());
    bgActor->SetPosition(Vector2(centerX, Game::WINDOW_HEIGHT / 2.0f));
    auto* bgRect = new RectComponent(bgActor, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, RendererMode::TRIANGLES);
    bgRect->SetColor(Vector4(36.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f, 1.0f));

    auto* titleActor = new Actor(GetGame());
    auto* titleText = new UITextComponent(titleActor);
    titleText->SetFont("../Assets/Fonts/Jacquard12-Regular.ttf");
    titleText->SetText("Store", Color::White, 108);
    titleActor->SetPosition(Vector2(centerX, 100.0f));

    mCoinWidget = new UIStatWidget(GetGame(), "SOULS", 36);
    mCoinWidget->SetOutline(true);
    mCoinWidget->SetPosition(Vector2(60.0f, 35.0f), HAlign::Left);
    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));

    const float statusStartY = 35.0f;
    const float statusSpacing = 90.0f;

    mHealthWidget = new UIStatWidget(GetGame(), "HEALTH", 30);
    mHealthWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, statusStartY), HAlign::Right);
    mHealthWidget->SetValue(std::to_string(GetGame()->GetPlayerHealth()) + "/" + std::to_string(GetGame()->GetPlayerMaxHealth()));

    mMeleeWidget = new UIStatWidget(GetGame(), "MELEE ATK", 30);
    mMeleeWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, statusStartY + statusSpacing), HAlign::Right);
    mMeleeWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerMeleeDamage())));

    mRangedWidget = new UIStatWidget(GetGame(), "RANGED ATK", 30);
    mRangedWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, statusStartY + (statusSpacing * 2.0f)), HAlign::Right);
    mRangedWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerRangedDamage())));

    mFireRateWidget = new UIStatWidget(GetGame(), "FIRE RATE", 30);
    mFireRateWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, statusStartY + (statusSpacing * 3.0f)), HAlign::Right);
    {
        char fireRateStr[16];
        snprintf(fireRateStr, sizeof(fireRateStr), "%.2f/s", 1.0f / GetGame()->GetPlayerFireRate());
        mFireRateWidget->SetValue(fireRateStr);
    }

    mSpeedWidget = new UIStatWidget(GetGame(), "SPEED", 30);
    mSpeedWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, statusStartY + (statusSpacing * 4.0f)), HAlign::Right);
    mSpeedWidget->SetValue(GetGame()->GetSpeedDisplayValue());

    mPiercingWidget = new UIStatWidget(GetGame(), "PIERCING", 30);
    mPiercingWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, statusStartY + (statusSpacing * 5.0f)), HAlign::Right);
    mPiercingWidget->SetValue(std::to_string(GetGame()->GetPlayerPiercing()));

    const Vector2 buttonSize(320.0f, 50.0f);
    const float buttonSpacing = 70.0f;
    const float firstButtonY = 250.0f;

    auto* fireRateActor = new Actor(GetGame());
    fireRateActor->SetPosition(Vector2(centerX, firstButtonY));
    mFireRateButton = new UIButtonComponent(fireRateActor, "Shooting Cadence (Price: " + std::to_string(GetGame()->GetFireRatePrice()) + ")", buttonSize,
        [this]() {
            int price = GetGame()->GetFireRatePrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendCoins(price);
                GetGame()->UpgradeFireRate();
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought.mp3");
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought-fail.wav");
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        "../Assets/Store/FireRate.png",
        Vector2(8.0f * 3.0f, 2.0f * 3.0f)
    );
    mButtons.push_back(mFireRateButton);

    auto* damageActor = new Actor(GetGame());
    damageActor->SetPosition(Vector2(centerX, firstButtonY + buttonSpacing));
    mDamageButton = new UIButtonComponent(damageActor, "Damage (Price: " + std::to_string(GetGame()->GetDamagePrice()) + ")", buttonSize,
        [this]() {
            int price = GetGame()->GetDamagePrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendCoins(price);
                GetGame()->UpgradeMeleeDamage();
                GetGame()->UpgradeRangedDamage();
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought.mp3");
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought-fail.wav");
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        "../Assets/Store/Damage.png",
        Vector2(13.0f * 2.0f, 14.0f * 2.0f)
    );
    mButtons.push_back(mDamageButton);

    auto* velocityActor = new Actor(GetGame());
    velocityActor->SetPosition(Vector2(centerX, firstButtonY + (buttonSpacing * 2.0f)));
    mVelocityButton = new UIButtonComponent(velocityActor, "Velocity (Price: " + std::to_string(GetGame()->GetVelocityPrice()) + ")", buttonSize,
        [this]() {
            int price = GetGame()->GetVelocityPrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendCoins(price);
                GetGame()->UpgradeSpeed();
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought.mp3");
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought-fail.wav");
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        "../Assets/Store/Velocity.png",
        Vector2(13.0f * 2.0f, 16.0f * 2.0f)
    );
    mButtons.push_back(mVelocityButton);

    auto* piercingActor = new Actor(GetGame());
    piercingActor->SetPosition(Vector2(centerX, firstButtonY + (buttonSpacing * 3.0f)));
    mPiercingButton = new UIButtonComponent(piercingActor, "Piercing (Price: " + std::to_string(GetGame()->GetPiercingPrice()) + ")", buttonSize,
        [this]() {
            int price = GetGame()->GetPiercingPrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendCoins(price);
                GetGame()->UpgradePiercing();
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought.mp3");
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/store-item-bought-fail.wav");
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        "../Assets/Store/Piercing.png",
        Vector2(14.0f * 2.0f, 7.0f * 2.0f)
    );
    mButtons.push_back(mPiercingButton);

    auto* nextLevelActor = new Actor(GetGame());
    nextLevelActor->SetPosition(Vector2(centerX, Game:: WINDOW_HEIGHT - 100.0f));
    auto* nextLevelButton = new UIButtonComponent(nextLevelActor, "CONTINUE", buttonSize,
        [this]() {
            GetGame()->SaveGame();
            GetGame()->SetScene(Game::GameScene::Gameplay);
        }
        ,
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36
    );
    mButtons.push_back(nextLevelButton);

    UpdateButtonSelection();
}

void StoreScene::Unload()
{
}

void StoreScene::Update(float deltaTime)
{
    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));
    
    // Update status widgets
    mHealthWidget->SetValue(std::to_string(GetGame()->GetPlayerHealth()) + "/" + std::to_string(GetGame()->GetPlayerMaxHealth()));
    mMeleeWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerMeleeDamage())));
    mRangedWidget->SetValue(std::to_string(static_cast<int>(GetGame()->GetPlayerRangedDamage())));
    {
        char fireRateStr[16];
        snprintf(fireRateStr, sizeof(fireRateStr), "%.2f/s", 1.0f / GetGame()->GetPlayerFireRate());
        mFireRateWidget->SetValue(fireRateStr);
    }
    mSpeedWidget->SetValue(GetGame()->GetSpeedDisplayValue());
    
    mPiercingWidget->SetValue(std::to_string(GetGame()->GetPlayerPiercing()));
    
    // Update button texts with current prices
    mFireRateButton->SetText("Shooting Cadence (Price: " + std::to_string(GetGame()->GetFireRatePrice()) + ")");
    mDamageButton->SetText("Damage (Price: " + std::to_string(GetGame()->GetDamagePrice()) + ")");
    mVelocityButton->SetText("Velocity (Price: " + std::to_string(GetGame()->GetVelocityPrice()) + ")");
    mPiercingButton->SetText("Piercing (Price: " + std::to_string(GetGame()->GetPiercingPrice()) + ")");
}

void StoreScene::ProcessInput(const uint8_t* keyState)
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

void StoreScene::SelectNextButton()
{
    mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();

    UpdateButtonSelection();

    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/select-option.wav");
}

void StoreScene::SelectPreviousButton()
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

void StoreScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        bool clickedOnContinueButton = mSelectedButtonIndex == mButtons.size() - 1;

        if (clickedOnContinueButton)
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/enter-level.wav");
        }

        mButtons[mSelectedButtonIndex]->Click();
    }
}

void StoreScene::UpdateButtonSelection()
{
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetSelected(i == mSelectedButtonIndex);
    }
}