#pragma once
class Game;
class Actor;
class UITextComponent;
class StaticSpriteComponent;

#include "../Math.h"
#include <string>

enum class HAlign { Left, Center, Right };

class UIStatWidget
{
public:
    UIStatWidget(class Game* game, const std::string& label, int pointSize = 24, int drawOrder = 200);
    ~UIStatWidget();

    void SetPosition(const Vector2& position, HAlign align = HAlign::Left);
    
    void SetValue(const std::string& value);
    
    void SetOutline(bool enabled, const Vector3& color = Color::Black, float size = 2.0f);

    void SetIcon(const std::string& texturePath, const Vector2& size);
    void SetValueOffsetX(float offset);
    void SetIconValueLeftAligned(bool leftAligned);

private:
    void UpdatePositions();

    class Game* mGame;

    class Actor* mLabelActor;
    class Actor* mValueActor;
    class UITextComponent* mLabelText;
    class UITextComponent* mValueText;
    class Actor* mIconActor;
    class StaticSpriteComponent* mIconSprite;

    Vector2 mPosition;
    HAlign mAlignment;
    int mPointSize;
    const float mVerticalSpacing = 1.0f;
    Vector2 mIconSize;
    float mIconSpacing = 8.0f;
    int mDrawOrder;
    bool mIconValueLeftAligned;
    float mValueOffsetX;
};