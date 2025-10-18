#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "../Math.h"
#include "VertexArray.h"
#include "Texture.h"

enum class RendererMode
{
    TRIANGLES,
    LINES
};

class Renderer
{
public:
	Renderer(SDL_Window* window);
	~Renderer();

	bool Initialize(float width, float height);
	void Shutdown();

    void DrawRect(const Vector2 &position, const Vector2 &size,  float rotation,
                  const Vector3 &color, const Vector2 &cameraPos, RendererMode mode);

    void DrawTexture(const Vector2 &position, const Vector2 &size,  float rotation,
                     const Vector3 &color, Texture *texture,
                     const Vector4 &textureRect = Vector4::UnitRect,
                     const Vector2 &cameraPos = Vector2::Zero, bool flip = false,
                     float textureFactor = 1.0f);

    void DrawGeometry(const Vector2 &position, const Vector2 &size,  float rotation,
                      const Vector3 &color, const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode);

    void Clear();
    void Present();

    // Getters
    class Texture* GetTexture(const std::string& fileName);
	class Shader* GetBaseShader() const { return mBaseShader; }

private:
    void Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
              const Vector3 &color,  Texture *texture = nullptr, const Vector4 &textureRect = Vector4::UnitRect, float textureFactor = 1.0f);

	bool LoadShaders();
    void CreateSpriteVerts();

	// Game
	class Game* mGame;

	// Basic shader
	class Shader* mBaseShader;

    // Sprite vertex array
    class VertexArray *mSpriteVerts;

	// Window
	SDL_Window* mWindow;

	// OpenGL context
	SDL_GLContext mContext;

	// Ortho projection for 2D shaders
	Matrix4 mOrthoProjection;

    // Map of textures loaded
    std::unordered_map<std::string, class Texture*> mTextures;
};