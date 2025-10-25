#pragma once
#include "../Math.h"
#include <string>

// Enum para controlar o alinhamento horizontal do widget
enum class HAlign { Left, Center, Right };

class UIStatWidget
{
public:
    UIStatWidget(class Game* game, const std::string& label, int pointSize = 24);
    ~UIStatWidget();

    void SetPosition(const Vector2& position, HAlign align = HAlign::Left);
    
    void SetValue(const std::string& value);
    
    void SetOutline(bool enabled, const Vector3& color = Color::Black, float size = 2.0f);

private:
    void UpdatePositions();

    class Game* mGame;

    class Actor* mLabelActor;
    class Actor* mValueActor;
    class UITextComponent* mLabelText;
    class UITextComponent* mValueText;

    Vector2 mPosition;
    HAlign mAlignment;
    int mPointSize;
    const float mVerticalSpacing = 4.0f;
};