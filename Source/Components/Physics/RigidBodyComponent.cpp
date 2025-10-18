//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include <SDL.h>
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "RigidBodyComponent.h"
#include "AABBColliderComponent.h"

const float MAX_SPEED_X = 750.0f;
const float MAX_SPEED_Y = 750.0f;
const float GRAVITY = 2000.0f;

RigidBodyComponent::RigidBodyComponent(class Actor* owner, float mass, float friction, bool applyGravity, int updateOrder)
        :Component(owner, updateOrder)
        ,mMass(mass)
        ,mApplyGravity(applyGravity)
        ,mFrictionCoefficient(friction)
        ,mVelocity(Vector2::Zero)
        ,mAcceleration(Vector2::Zero)
{

}

void RigidBodyComponent::ApplyForce(const Vector2 &force)
{
    mAcceleration += force * (1.f/mMass);
}

void RigidBodyComponent::Update(float deltaTime)
{
    // Apply gravity acceleration
    if(mApplyGravity)
    {
        ApplyForce(Vector2::UnitY * GRAVITY);
    }

    if(!Math::NearlyZero(mVelocity.Length()) && mFrictionCoefficient != 0.0f)
    {
        ApplyForce(mVelocity * -mFrictionCoefficient);
    }

    mVelocity += mAcceleration * deltaTime;

    mVelocity.x = Math::Clamp<float>(mVelocity.x, -MAX_SPEED_X, MAX_SPEED_X);
    mVelocity.y = Math::Clamp<float>(mVelocity.y, -MAX_SPEED_Y, MAX_SPEED_Y);

    if(mVelocity.Length() < 1.0f)
    {
        mVelocity = Vector2::Zero;
    }

    if (!Math::NearlyZero(mVelocity.Length()))
    {
        Vector2 newPos = mOwner->GetPosition() + mVelocity * deltaTime;
        mOwner->SetPosition(newPos);
    }

    // auto collider = mOwner->GetComponent<AABBColliderComponent>();
    // if (collider)
    // {
    //     collider->DetectHorizontalCollision(this);
    //     collider->DetectVertialCollision(this);
    // }

    mAcceleration.Set(0.f, 0.f);
}