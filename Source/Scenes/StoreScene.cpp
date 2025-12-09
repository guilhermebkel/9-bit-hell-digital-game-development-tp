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
    , mSpeedButton(nullptr)
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
    GetGame()->GetAudioSystem()->PlayMusic(Game::ResolvePath("Assets/Sounds/level-transition.wav"));

    const float centerX = Game::WINDOW_WIDTH / 2.0f;

    auto* bgActor = new Actor(GetGame());
    bgActor->SetPosition(Vector2(centerX, Game::WINDOW_HEIGHT / 2.0f));
    auto* bgRect = new RectComponent(bgActor, Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT, RendererMode::TRIANGLES);
    bgRect->SetColor(Vector4(36.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f, 1.0f));

    auto* titleActor = new Actor(GetGame());
    auto* titleText = new UITextComponent(titleActor);
    titleText->SetFont(Game::ResolvePath("Assets/Fonts/Jacquard12-Regular.ttf"));
    titleText->SetText("Store", Color::White, 108);
    titleActor->SetPosition(Vector2(centerX, 100.0f));

    mCoinWidget = new UIStatWidget(GetGame(), "SOULS", 36);
    mCoinWidget->SetOutline(true);
    mCoinWidget->SetPosition(Vector2(60.0f, 35.0f), HAlign::Left);
    mCoinWidget->SetValue(std::to_string(GetGame()->GetSoulCount()));
    mCoinWidget->SetIcon(Game::ResolvePath("Assets/Store/Soul.png"), Vector2(8.0f * 3.0f, 8.0f * 3.0f));
    mCoinWidget->SetIconValueLeftAligned(true);
    mCoinWidget->SetValueOffsetX(-30.0f);

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
    mFireRateButton = new UIButtonComponent(fireRateActor, "Firerate", buttonSize,
        [this]() {
            int price = GetGame()->GetFireRatePrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendSouls(price);
                GetGame()->UpgradeFireRate();
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought.mp3"));
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought-fail.wav"));
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        Game::ResolvePath("Assets/Store/FireRate.png"),
        Vector2(12.0f * 3.0f, 12.0f * 3.0f)
    );
    mFireRateButton->SetPriceIcon(Game::ResolvePath("Assets/Store/Soul.png"), Vector2(8.0f * 3.0f, 8.0f * 3.0f));
    {
        int price = GetGame()->GetFireRatePrice();
        mFireRateButton->SetPrice(price);
        mFireRateButton->SetAffordable(GetGame()->CanAfford(price));
    }
    mFireRateButton->SetMainContentLeftAligned(true);
    mButtons.push_back(mFireRateButton);

    auto* damageActor = new Actor(GetGame());
    damageActor->SetPosition(Vector2(centerX, firstButtonY + buttonSpacing));
    mDamageButton = new UIButtonComponent(damageActor, "Damage", buttonSize,
        [this]() {
            int price = GetGame()->GetDamagePrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendSouls(price);
                GetGame()->UpgradeMeleeDamage();
                GetGame()->UpgradeRangedDamage();
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought.mp3"));
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought-fail.wav"));
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        Game::ResolvePath("Assets/Store/Damage.png"),
        Vector2(12.0f * 3.0f, 12.0f * 3.0f)
    );
    mDamageButton->SetPriceIcon(Game::ResolvePath("Assets/Store/Soul.png"), Vector2(8.0f * 3.0f, 8.0f * 3.0f));
    {
        int price = GetGame()->GetDamagePrice();
        mDamageButton->SetPrice(price);
        mDamageButton->SetAffordable(GetGame()->CanAfford(price));
    }
    mDamageButton->SetMainContentLeftAligned(true);
    mButtons.push_back(mDamageButton);

    auto* speedActor = new Actor(GetGame());
    speedActor->SetPosition(Vector2(centerX, firstButtonY + (buttonSpacing * 2.0f)));
    mSpeedButton = new UIButtonComponent(speedActor, "Speed", buttonSize,
        [this]() {
            int price = GetGame()->GetSpeedPrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendSouls(price);
                GetGame()->UpgradeSpeed();
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought.mp3"));
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought-fail.wav"));
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        Game::ResolvePath("Assets/Store/Speed.png"),
        Vector2(12.0f * 3.0f, 12.0f * 3.0f)
    );
    mSpeedButton->SetPriceIcon(Game::ResolvePath("Assets/Store/Soul.png"), Vector2(8.0f * 3.0f, 8.0f * 3.0f));
    {
        int price = GetGame()->GetSpeedPrice();
        mSpeedButton->SetPrice(price);
        mSpeedButton->SetAffordable(GetGame()->CanAfford(price));
    }
    mSpeedButton->SetMainContentLeftAligned(true);
    mButtons.push_back(mSpeedButton);

    auto* piercingActor = new Actor(GetGame());
    piercingActor->SetPosition(Vector2(centerX, firstButtonY + (buttonSpacing * 3.0f)));
    mPiercingButton = new UIButtonComponent(piercingActor, "Piercing", buttonSize,
        [this]() {
            int price = GetGame()->GetPiercingPrice();
            if (GetGame()->CanAfford(price))
            {
                GetGame()->SpendSouls(price);
                GetGame()->UpgradePiercing();
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought.mp3"));
            }
            else
            {
                GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/store-item-bought-fail.wav"));
            }
        },
        UIButtonComponent::DEFAULT_DRAW_ORDER,
        36,
        Game::ResolvePath("Assets/Store/Piercing.png"),
        Vector2(12.0f * 3.0f, 12.0f * 3.0f)
    );
    mPiercingButton->SetPriceIcon(Game::ResolvePath("Assets/Store/Soul.png"), Vector2(8.0f * 3.0f, 8.0f * 3.0f));
    {
        int price = GetGame()->GetPiercingPrice();
        mPiercingButton->SetPrice(price);
        mPiercingButton->SetAffordable(GetGame()->CanAfford(price));
    }
    mPiercingButton->SetMainContentLeftAligned(true);
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
    mCoinWidget->SetValue(std::to_string(GetGame()->GetSoulCount()));
    
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
    {
        int price = GetGame()->GetFireRatePrice();
        mFireRateButton->SetPrice(price);
        mFireRateButton->SetAffordable(GetGame()->CanAfford(price));
    }

    {
        int price = GetGame()->GetDamagePrice();
        mDamageButton->SetPrice(price);
        mDamageButton->SetAffordable(GetGame()->CanAfford(price));
    }

    {
        int price = GetGame()->GetSpeedPrice();
        mSpeedButton->SetPrice(price);
        mSpeedButton->SetAffordable(GetGame()->CanAfford(price));
    }

    {
        int price = GetGame()->GetPiercingPrice();
        mPiercingButton->SetPrice(price);
        mPiercingButton->SetAffordable(GetGame()->CanAfford(price));
    }
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

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/select-option.wav"));
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

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/select-option.wav"));
}

void StoreScene::ClickSelectedButton()
{
    if (!mButtons.empty())
    {
        bool clickedOnContinueButton = mSelectedButtonIndex == mButtons.size() - 1;

        if (clickedOnContinueButton)
        {
            GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/enter-level.wav"));
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