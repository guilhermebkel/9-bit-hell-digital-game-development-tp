#pragma once
#include "DrawComponent.h"
#include <vector>

class ShapeComponent : public DrawComponent
{
public:
    ShapeComponent(class Actor* owner, int drawOrder = 100);
    ~ShapeComponent();

    void Draw(class Renderer* renderer) override;

    void AddLine(const Vector2& start, const Vector2& end, float thickness);

    void SetColor(const Vector3& color);

private:
    std::vector<class Actor*> mLineActors;
};