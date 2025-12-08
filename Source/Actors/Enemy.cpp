#include "Enemy.h"

#include "Player.h"
#include "../Game.h"
#include "../Scenes/GameplayScene.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include <sstream>
#include <algorithm>
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Random.h"
#include "../Audio/AudioSystem.h"

Enemy::Enemy(Game* game, EnemyType type, float forwardSpeed, float deathTime)
    : Actor(game)
      , mIsDying(false)
      , mHasDealtDamage(false)
      , mForwardSpeed(forwardSpeed)
      , mDyingTimer(deathTime)
      , mRigidBodyComponent(nullptr)
      , mColliderComponent(nullptr)
      , mDrawComponent(nullptr)
      , mAIState(AIState::Moving)
      , mStateTimer(0.0f)
      , mIsFlashing(false)
      , mFlashTimer(0.0f)
      , mOriginalColor(Vector3::Zero)
{
    float diffMul = 1.0f;
    if (GetGame()) diffMul = GetGame()->GetDifficultyMultiplier();

    mMaxHealth = BASE_HEALTH * diffMul;
    mHealth = mMaxHealth;

    mForwardSpeed *= diffMul;

    std::string texturePath;
    std::string jsonPath;

    switch (type)
    {
    case EnemyType::Eye:
        texturePath = "../Assets/Sprites/EyeEnemy/EyeEnemy.png";
        jsonPath = "../Assets/Sprites/EyeEnemy/EyeEnemy.json";
        mDrawComponent = new AnimatorComponent(this, texturePath, jsonPath, Enemy::EYE_SPRITE_WIDTH, Enemy::EYE_SPRITE_HEIGHT);
        mOriginalColor = Vector3(1.0f, 0.39f, 0.39f);
        mDrawComponent->SetColor(mOriginalColor);
        mDrawComponent->AddAnimation("attack", {0, 1});
        mDrawComponent->AddAnimation("idle", {2, 3});
        mDrawComponent->AddAnimation("walk", {4, 5});
        mDrawComponent->AddAnimation("dead", {2});
        mDrawComponent->SetAnimation("walk");
        mDrawComponent->SetAnimFPS(4.0f);
        break;

    case EnemyType::Horn:
        texturePath = "../Assets/Sprites/HornEnemy/HornEnemy.png";
        jsonPath = "../Assets/Sprites/HornEnemy/HornEnemy.json";
        mDrawComponent = new AnimatorComponent(this, texturePath, jsonPath, Enemy::SPRITE_WIDTH, Enemy::SPRITE_HEIGHT);
        mOriginalColor = Vector3(0.5f, 1.0f, 0.5f);
        mDrawComponent->SetColor(mOriginalColor);
        mDrawComponent->AddAnimation("attack", {0, 1});
        mDrawComponent->AddAnimation("idle", {2, 3});
        mDrawComponent->AddAnimation("walk", {5, 4});
        mDrawComponent->AddAnimation("dead", {2});
        mDrawComponent->SetAnimation("walk");
        mDrawComponent->SetAnimFPS(4.0f);
        break;

    case EnemyType::Fat:
        texturePath = "../Assets/Sprites/FatEnemy/FatEnemy.png";
        jsonPath = "../Assets/Sprites/FatEnemy/FatEnemy.json";
        mDrawComponent = new AnimatorComponent(this, texturePath, jsonPath, Enemy::SPRITE_WIDTH, Enemy::SPRITE_HEIGHT);
        mOriginalColor = Vector3(0.44f, 0.56f, 0.25f);
        mDrawComponent->SetColor(mOriginalColor);
        mDrawComponent->AddAnimation("attack", {0, 1, 2});
        mDrawComponent->AddAnimation("being-hit", {3});
        mDrawComponent->AddAnimation("idle", {4, 5});
        mDrawComponent->AddAnimation("walk", {7, 6});
        mDrawComponent->AddAnimation("dead", {4});
        mDrawComponent->SetAnimation("walk");
        mDrawComponent->SetAnimFPS(4.0f);
        break;
    }

    // Apply exact hex-based tints per level as requested by designer
    if (GetGame())
    {
        auto levelID = GetGame()->GetCurrentLevelID();

        auto hexToVec3 = [](const std::string& hex) {
            unsigned int v = 0;
            if (hex.size() == 6)
            {
                std::stringstream ss;
                ss << std::hex << hex;
                ss >> v;
            }
            float r = ((v >> 16) & 0xFF) / 255.0f;
            float g = ((v >> 8) & 0xFF) / 255.0f;
            float b = (v & 0xFF) / 255.0f;
            return Vector3(r, g, b);
        };

        // Exact colors (from most light to dark per palette)
        const Vector3 red_light   = hexToVec3("8c4342"); // level1 (light)
        const Vector3 red_mid     = hexToVec3("9c2828"); // level2 (mid)
        const Vector3 red_dark    = hexToVec3("6d1515"); // level3 (dark)

        const Vector3 green_light = hexToVec3("7d944b"); // level4
        const Vector3 green_mid   = hexToVec3("647d30"); // level5
        const Vector3 green_dark  = hexToVec3("5b7225"); // level6

        const Vector3 purple_light = hexToVec3("5d4a70"); // level7
        const Vector3 purple_mid   = hexToVec3("4e3f60"); // level8
        const Vector3 purple_dark  = hexToVec3("453059"); // level9

        // Tutorial: keep original color (no overlay)
        if (levelID == LevelID::Tutorial)
        {
            // do nothing
        }
        else if (levelID == LevelID::Level1)
        {
            // Light red for Level 1
            if (type == EnemyType::Eye)
            {
                mOriginalColor = red_light;
            }
        }
        else if (levelID == LevelID::Level2)
        {
            // Mid red for Level 2
            if (type == EnemyType::Eye)
            {
                mOriginalColor = red_mid;
            }
        }
        else if (levelID == LevelID::Level3)
        {
            // Dark red for Level 3
            if (type == EnemyType::Eye)
            {
                mOriginalColor = red_dark;
            }
        }
        else if (levelID == LevelID::Level4)
        {
            if (type == EnemyType::Eye || type == EnemyType::Fat)
            {
                mOriginalColor = green_light;
            }
        }
        else if (levelID == LevelID::Level5)
        {
            if (type == EnemyType::Eye || type == EnemyType::Fat)
            {
                mOriginalColor = green_mid;
            }
        }
        else if (levelID == LevelID::Level6)
        {
            if (type == EnemyType::Eye || type == EnemyType::Fat)
            {
                mOriginalColor = green_dark;
            }
        }
        else if (levelID == LevelID::Level7)
        {
            mOriginalColor = purple_light;
        }
        else if (levelID == LevelID::Level8)
        {
            mOriginalColor = purple_mid;
        }
        else if (levelID == LevelID::Level9)
        {
            mOriginalColor = purple_dark;
        }

        const float BRIGHTNESS_BOOST = 1.35f;
        mOriginalColor.x = std::min(mOriginalColor.x * BRIGHTNESS_BOOST, 1.0f);
        mOriginalColor.y = std::min(mOriginalColor.y * BRIGHTNESS_BOOST, 1.0f);
        mOriginalColor.z = std::min(mOriginalColor.z * BRIGHTNESS_BOOST, 1.0f);

        if (mDrawComponent)
        {
            mDrawComponent->SetColor(mOriginalColor);
        }
    }

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f);

    const int dy = (int)((Enemy::SPRITE_HEIGHT / 2.0f) - (Enemy::PHYSICS_HEIGHT / 2.0f));
    mColliderComponent = new AABBColliderComponent(this, 0, dy, Enemy::PHYSICS_WIDTH, Enemy::PHYSICS_HEIGHT,
                                                   ColliderLayer::Enemy);

    const float aggroWidth = AGGRO_AREA_SIZE;
    const float aggroHeight = AGGRO_AREA_SIZE;
    mAggroCollider = new AABBColliderComponent(this, 0, dy, aggroWidth, aggroHeight, ColliderLayer::EnemyAggro, true);

    Vector2 initialVelocity = Vector2::Zero;
    while (initialVelocity.Length() < 1.0f)
    {
        initialVelocity = Random::GetVector(Vector2(-mForwardSpeed, -mForwardSpeed),
                                            Vector2(mForwardSpeed, mForwardSpeed));
    }
    mRigidBodyComponent->SetVelocity(initialVelocity);
}

void Enemy::Kill()
{
    if (mIsDying) return;
    mIsDying = true;
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/monster-hurt.wav");
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    
    mIsFlashing = true;
    mFlashTimer = HIT_FLASH_DURATION;
    mDrawComponent->SetColor(Vector3(10.0f, 10.0f, 10.0f));
}

void Enemy::TakeDamage(float amount)
{
    if (mIsDying) return;
    
    mHealth -= amount;
    
    mIsFlashing = true;
    mFlashTimer = HIT_FLASH_DURATION;
    mDrawComponent->SetColor(Vector3(10.0f, 10.0f, 10.0f));
    
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/monster-hurt.wav");
    
    if (mHealth <= 0.0f)
    {
        Kill();
    }
}

void Enemy::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    if (mIsFlashing)
    {
        mFlashTimer -= deltaTime;
        if (mFlashTimer <= 0.0f)
        {
            mIsFlashing = false;
            mDrawComponent->SetColor(mOriginalColor);
        }
    }

    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f)
        {
            mState = ActorState::Destroy;
        }
        return;
    }

    UpdateAI(deltaTime);

    Vector2 pos = GetPosition();
    Vector2 vel = mRigidBodyComponent->GetVelocity();

    const float halfWidth = Enemy::SPRITE_WIDTH / 2.0f;
    const float halfHeight = Enemy::SPRITE_HEIGHT / 2.0f;

    if ((pos.x <= halfWidth && vel.x < 0.0f) || (pos.x >= Game::WINDOW_WIDTH - halfWidth && vel.x > 0.0f))
    {
        vel.x *= -1.0f;
    }

    if ((pos.y <= GetGame()->GetUpperBoundary() + halfHeight && vel.y < 0.0f) || (pos.y >= Game::WINDOW_HEIGHT -
        halfHeight && vel.y > 0.0f))
    {
        vel.y *= -1.0f;
    }

    mRigidBodyComponent->SetVelocity(vel);

    if (vel.x < -1.0f)
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
    else if (vel.x > 1.0f)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }

    if (mDrawComponent)
    {
        mDrawComponent->SetDrawOrder(100 + static_cast<int>(GetPosition().y));
    }
}

void Enemy::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        if (mAIState == AIState::Attacking && !mHasDealtDamage)
        {
            Player* player = dynamic_cast<Player*>(other->GetOwner());

            if (player)
            {
                float distance = Vector2::Distance(GetPosition(), player->GetPosition());
                if (distance < Enemy::ATTACK_DISTANCE)
                {
                    player->TakeDamage(Enemy::ATTACK_DAMAGE);
                    mHasDealtDamage = true;
                }

                mAIState = AIState::Cooldown;
                mStateTimer = Enemy::ATTACK_COOLDOWN;
            }
        }
    }
}

void Enemy::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        if (mAIState == AIState::Attacking && !mHasDealtDamage)
        {
            Player* player = dynamic_cast<Player*>(other->GetOwner());
            if (player)
            {
                float distance = Vector2::Distance(GetPosition(), player->GetPosition());
                if (distance < Enemy::ATTACK_DISTANCE)
                {
                    player->TakeDamage(Enemy::ATTACK_DAMAGE);
                    mHasDealtDamage = true;
                }

                mAIState = AIState::Cooldown;
                mStateTimer = Enemy::ATTACK_COOLDOWN;
            }
        }
    }
}

void Enemy::UpdateAI(float deltaTime)
{
    mStateTimer -= deltaTime;
    const Player* player = GetGame()->GetPlayer();

    if (!player)
    {
        mAIState = AIState::Moving;
        return;
    }

    auto playerCollider = player->GetComponent<AABBColliderComponent>();

    switch (mAIState)
    {
    case AIState::Moving:
        {
            mRigidBodyComponent->SetEnabled(true);
            mDrawComponent->SetAnimation("walk");

            if (playerCollider && mAggroCollider->Intersect(*playerCollider))
            {
                mAIState = AIState::Chasing;
            }
            break;
        }

    case AIState::Chasing:
        {
            mRigidBodyComponent->SetEnabled(true);
            mDrawComponent->SetAnimation("walk");

            if (!playerCollider || !mAggroCollider->Intersect(*playerCollider))
            {
                mAIState = AIState::Moving;
                Vector2 newVel = Random::GetVector(Vector2(-mForwardSpeed, -mForwardSpeed),
                                                   Vector2(mForwardSpeed, mForwardSpeed));
                mRigidBodyComponent->SetVelocity(newVel);
                return;
            }

            Vector2 direction = player->GetPosition() - GetPosition();
            direction.Normalize();

            mRigidBodyComponent->SetVelocity(direction * mForwardSpeed);

            if (direction.x < 0.0f)
            {
                SetScale(Vector2(-1.0f, 1.0f));
            }
            else
            {
                SetScale(Vector2(1.0f, 1.0f));
            }

            if (Vector2::Distance(GetPosition(), player->GetPosition()) < Enemy::ATTACK_DISTANCE)
            {
                mAIState = AIState::WindUp;
                mStateTimer = Enemy::ATTACK_WIND_UP_TIME;
                mRigidBodyComponent->SetVelocity(Vector2::Zero);
            }
            break;
        }

    case AIState::WindUp:
        {
            mRigidBodyComponent->SetEnabled(false);

            if (mStateTimer <= 0.0f)
            {
                mAIState = AIState::Attacking;
                mStateTimer = Enemy::ATTACK_DURATION;
                mHasDealtDamage = false;
                mDrawComponent->SetAnimation("attack");
            }
            break;
        }

    case AIState::Attacking:
        {
            if (mStateTimer <= 0.0f)
            {
                mAIState = AIState::Cooldown;
                mStateTimer = Enemy::ATTACK_COOLDOWN;
                mDrawComponent->SetAnimation("walk");
            }
            break;
        }

    case AIState::Cooldown:
        {
            if (mStateTimer <= 0.0f)
            {
                if (playerCollider && mAggroCollider->Intersect(*playerCollider))
                {
                    mAIState = AIState::Chasing;
                }
                else
                {
                    mAIState = AIState::Moving;
                }
            }
            break;
        }
    }
}
