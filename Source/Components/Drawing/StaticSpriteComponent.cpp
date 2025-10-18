#include "StaticSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"

StaticSpriteComponent::StaticSpriteComponent(Actor* owner, const std::string& texturePath, int width, int height, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mTexture(nullptr)
    , mWidth(width)
    , mHeight(height)
{
    mTexture = owner->GetGame()->GetRenderer()->GetTexture(texturePath);
}

StaticSpriteComponent::~StaticSpriteComponent()
{
}

void StaticSpriteComponent::Draw(Renderer* renderer)
{
    if (mIsVisible && mTexture)
    {
        Vector2 size(mWidth * mOwner->GetScale().x, mHeight * mOwner->GetScale().y);

        renderer->DrawTexture(
            mOwner->GetPosition(),
            size,
            mOwner->GetRotation(),
            Vector3(1.0f, 1.0f, 1.0f),
            mTexture,
            Vector4::UnitRect,
            mOwner->GetGame()->GetCameraPos(),
            false,
            1.0f
        );
    }
}