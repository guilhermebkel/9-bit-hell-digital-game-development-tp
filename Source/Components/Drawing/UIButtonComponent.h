#pragma once
#include "DrawComponent.h"
#include <string>
#include <functional>
#include "../../Math.h"

class UIButtonComponent : public DrawComponent
{
public:
    UIButtonComponent(class Actor* owner, const std::string& text, Vector2 size, std::function<void()> onClick, int drawOrder = 200);
    ~UIButtonComponent() override;

    void Draw(class Renderer* renderer) override;
    void OnUpdate(float deltaTime);

    void SetSelected(bool selected) { mSelected = selected; }
    bool GetSelected() const { return mSelected; }

    void Click();

private:
    void CenterText();

    std::function<void()> mOnClick;
    class UITextComponent* mTextComponent;
    class Actor* mTextActor;
    
    Vector2 mSize;
    bool mSelected;
};