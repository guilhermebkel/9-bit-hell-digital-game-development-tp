#include "SolidShapeComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/VertexArray.h"

SolidShapeComponent::SolidShapeComponent(Actor* owner, const std::vector<Vector2>& points, const Vector4& color, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mVertexArray(nullptr)
    , mColorVec4(color)
{
    if (points.size() != 4) return;

    float vertices[] = {
        points[0].x, points[0].y, 0.0f, 0.0f, // 0: Top Left
        points[1].x, points[1].y, 0.0f, 0.0f, // 1: Top Right
        points[2].x, points[2].y, 0.0f, 0.0f, // 2: Bot Right
        points[3].x, points[3].y, 0.0f, 0.0f  // 3: Bot Left
    };

    unsigned int indices[] = {
        0, 1, 2, // Primeiro triângulo
        2, 3, 0  // Segundo triângulo
    };

    mVertexArray = new VertexArray(vertices, 4, indices, 6);
}

SolidShapeComponent::~SolidShapeComponent()
{
    delete mVertexArray;
}

void SolidShapeComponent::Draw(Renderer* renderer)
{
    if (mIsVisible && mVertexArray)
    {
        renderer->DrawGeometry(
            Vector2::Zero,
            Vector2(1.0f, 1.0f),
            0.0f,
            Vector3(mColorVec4.x, mColorVec4.y, mColorVec4.z), // Passa RGB
            Vector2::Zero,
            mVertexArray,
            RendererMode::TRIANGLES 
        );
    }
}