//
// Created by Lucas N. Ferreira on 03/08/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/VertexArray.h"
#include <vector>
#include <SDL.h>

class DrawComponent : public Component
{
public:
    // (Lower draw order corresponds with further back)
    DrawComponent(class Actor* owner, int drawOrder = 100);
    ~DrawComponent();

    virtual void Draw(Renderer* renderer);
    int GetDrawOrder() const { return mDrawOrder; }

    void SetVisible(bool visible) { mIsVisible = visible; }
    void SetColor(const Vector3& color) { mColor = color; }

protected:
    int mDrawOrder;
    bool mIsVisible;
    Vector3 mColor;
};
