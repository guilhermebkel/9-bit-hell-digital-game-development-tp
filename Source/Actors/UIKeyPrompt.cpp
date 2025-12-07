#include "UIKeyPrompt.h"
#include "../Game.h"
#include "Actor.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/UITextComponent.h"

UIKeyPrompt::UIKeyPrompt(Game* game, const Vector2& position, const std::string& key, const std::string& description, int drawOrder, float xSize)
    : mGame(game)
{
    float defaultYSize = 40.0f;

    Vector2 consolidatedPosition = Vector2(position.x + (xSize / 2.0f), position.y);

    mKeyBgActor = new Actor(game);
    mKeyBgActor->SetPosition(consolidatedPosition);
    mBgRect = new RectComponent(mKeyBgActor, xSize, defaultYSize, RendererMode::TRIANGLES, drawOrder);
    mBgRect->SetColor(Vector4(0.2f, 0.2f, 0.2f, 0.8f));

    mKeyTextActor = new Actor(game);
    mKeyTextActor->SetPosition(Vector2(consolidatedPosition.x, consolidatedPosition.y));
    mKeyText = new UITextComponent(mKeyTextActor, drawOrder + 1);
    mKeyText->SetText(key, Vector3(1.0f, 1.0f, 0.0f), 24);

    mDescTextActor = new Actor(game);
    mDescText = new UITextComponent(mDescTextActor, drawOrder);
    mDescText->SetText(description, Vector3(1.0f, 1.0f, 1.0f), 20);
    mDescTextActor->SetPosition(Vector2(consolidatedPosition.x + (xSize / 2.0f) + (mDescText->GetTexture()->GetWidth() / 2.0f) + 16.0f, consolidatedPosition.y));
}

UIKeyPrompt::~UIKeyPrompt()
{
    if (mKeyBgActor) mKeyBgActor->SetState(ActorState::Destroy);
    if (mKeyTextActor) mKeyTextActor->SetState(ActorState::Destroy);
    if (mDescTextActor) mDescTextActor->SetState(ActorState::Destroy);
}

void UIKeyPrompt::SetVisible(bool visible)
{
    ActorState state = visible ? ActorState::Active : ActorState::Paused;
    mKeyBgActor->SetState(state);
    mKeyTextActor->SetState(state);
    mDescTextActor->SetState(state);
}