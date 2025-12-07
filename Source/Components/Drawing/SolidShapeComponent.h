#pragma once
#include "DrawComponent.h"
#include <vector>

class SolidShapeComponent : public DrawComponent
{
public:
    SolidShapeComponent(class Actor* owner, const std::vector<Vector2>& points, const Vector4& color, int drawOrder = 100);
    ~SolidShapeComponent();

    void Draw(class Renderer* renderer) override;
    void SetColor(const Vector4& color) { mColorVec4 = color; }

private:
    class VertexArray* mVertexArray;
    Vector4 mColorVec4;
};