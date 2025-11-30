#include "Enemy.h"

#include "Player.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Random.h" // Precisamos para a velocidade aleatória

Enemy::Enemy(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mDyingTimer(deathTime)
        , mRigidBodyComponent(nullptr)
        , mColliderComponent(nullptr)
        , mDrawComponent(nullptr)
        , mAIState(AIState::Moving)
        , mStateTimer(0.0f)
{
    mDrawComponent = new AnimatorComponent(
        this,
        "../Assets/Sprites/Principal/Principal.png",
        "../Assets/Sprites/Principal/Principal.json",
        Enemy::SPRITE_WIDTH,
        Enemy::SPRITE_HEIGHT
    );

    mDrawComponent->SetColor(Vector3(1.0f, 0.39f, 0.39f));

    mDrawComponent->AddAnimation("idle", {1, 2});
    mDrawComponent->AddAnimation("walk", {8, 9, 10, 11, 12, 13, 14, 15, 16});
    mDrawComponent->AddAnimation("attack", {3, 4, 5, 6});
    mDrawComponent->AddAnimation("dead", {7, 0});

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(8.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f);

    // Align collider base with sprite base
    const int dy = (int)((Enemy::SPRITE_HEIGHT / 2.0f) - (Enemy::PHYSICS_HEIGHT / 2.0f));
    mColliderComponent = new AABBColliderComponent(this, 0, dy, Enemy::PHYSICS_WIDTH, Enemy::PHYSICS_HEIGHT, ColliderLayer::Enemy);

    const float aggroWidth = AGGRO_AREA_SIZE;
    const float aggroHeight = AGGRO_AREA_SIZE;
    mAggroCollider = new AABBColliderComponent(this, 0, dy, aggroWidth, aggroHeight, ColliderLayer::EnemyAggro, true);

    Vector2 initialVelocity = Vector2::Zero;
    while (initialVelocity.Length() < 1.0f)
    {
        initialVelocity = Random::GetVector(Vector2(-mForwardSpeed, -mForwardSpeed), Vector2(mForwardSpeed, mForwardSpeed));
    }
    mRigidBodyComponent->SetVelocity(initialVelocity);
}

void Enemy::Kill()
{
    if (mIsDying) return;
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Enemy::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

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

    if ((pos.y <= GetGame()->GetUpperBoundary() + halfHeight && vel.y < 0.0f) || (pos.y >= Game::WINDOW_HEIGHT - halfHeight && vel.y > 0.0f))
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
        if (mAIState == AIState::Attacking)
        {
            Player* player = dynamic_cast<Player*>(other->GetOwner());

            if (player)
            {
                player->TakeDamage(Enemy::ATTACK_DAMAGE);

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
        if (mAIState == AIState::Attacking)
        {
            Player* player = dynamic_cast<Player*>(other->GetOwner());
            if (player)
            {
                player->TakeDamage(Enemy::ATTACK_DAMAGE);

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
                Vector2 newVel = Random::GetVector(Vector2(-mForwardSpeed, -mForwardSpeed), Vector2(mForwardSpeed, mForwardSpeed));
                mRigidBodyComponent->SetVelocity(newVel);
                return;
            }

            Vector2 direction = player->GetPosition() - GetPosition();
            direction.Normalize();

            mRigidBodyComponent->SetVelocity(direction * mForwardSpeed);

            if (direction.x < 0.0f) {
                SetScale(Vector2(-1.0f, 1.0f));
            } else {
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