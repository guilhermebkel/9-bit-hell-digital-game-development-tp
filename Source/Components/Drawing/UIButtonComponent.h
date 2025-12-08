#pragma once
#include "DrawComponent.h"
#include <string>
#include <functional>
#include "../../Math.h"

class UIButtonComponent : public DrawComponent
{
public:
    static constexpr float DEFAULT_DRAW_ORDER = 200;

    UIButtonComponent(class Actor* owner,
                      const std::string& text,
                      Vector2 size,
                      std::function<void()> onClick,
                      int drawOrder = DEFAULT_DRAW_ORDER,
                      int textPointSize = 24,
                      const std::string& iconPath = "",
                      Vector2 iconSize = Vector2::Zero);

    ~UIButtonComponent() override;

    void Draw(class Renderer* renderer) override;
    float GetTextWidth() const;

    void SetSelected(bool selected) { mSelected = selected; }

    void SetText(const std::string& text);
    void Click();

    void SetPrice(int value);
    void SetPriceIcon(const std::string& iconPath, Vector2 size);
    void SetMainContentLeftAligned(bool leftAligned);
    void SetAffordable(bool affordable);
    bool IsAffordable() const { return mAffordable; }

private:
    void UpdatePositions();

    std::function<void()> mOnClick;
    class UITextComponent* mTextComponent;
    class Actor* mTextActor;

    class StaticSpriteComponent* mIconSprite;
    class Actor* mIconActor;
    Vector2 mIconSize;

    class UITextComponent* mPriceTextComponent;
    class Actor* mPriceTextActor;
    class StaticSpriteComponent* mPriceIconSprite;
    class Actor* mPriceIconActor;
    Vector2 mPriceIconSize;

    Vector2 mSize;
    bool mSelected;
    int mTextPointSize;
    Vector2 mHighlightSize;
    int mDrawOrder;
    float mPriceSpacing;
    float mPriceMargin;
    bool mMainContentLeftAligned;
    bool mAffordable;
    Vector3 mDisabledColor;
    Vector3 mTextColor;
    Vector3 mIconBaseColor;
    Vector3 mPriceIconBaseColor;
    void UpdateAffordabilityAppearance();
    Vector3 GetTextColor() const;
};