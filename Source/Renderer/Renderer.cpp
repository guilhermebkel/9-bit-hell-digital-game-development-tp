#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"

Renderer::Renderer(SDL_Window *window)
: mBaseShader(nullptr)
, mWindow(window)
, mContext(nullptr)
, mOrthoProjection(Matrix4::Identity)
{

}

Renderer::~Renderer()
{
    delete mSpriteVerts;
    mSpriteVerts = nullptr;
}

bool Renderer::Initialize(float width, float height)
{
    // Specify version 3.3 (core profile)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Turn on vsync
    SDL_GL_SetSwapInterval(1);

    // Create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }

	// Make sure we can create/compile shaders
	if (!LoadShaders()) {
		SDL_Log("Failed to load shaders.");
		return false;
	}

    // Create quad for drawing sprites
    CreateSpriteVerts();

    // Set the clear color to light grey
    glClearColor(0.419f, 0.549f, 1.0f, 1.0f);

    // Enable alpha blending on textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create orthografic projection matrix
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);


    // Activate shader
    mBaseShader->SetActive();
    mBaseShader->SetIntegerUniform("uTexture", 0);

    return true;
}

void Renderer::Shutdown()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    mBaseShader->Unload();
    delete mBaseShader;

    SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::Clear()
{
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
                    const Vector3 &color, Texture *texture, const Vector4 &textureRect, float textureFactor)
{
    mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);
    mBaseShader->SetVectorUniform("uColor", color);
    mBaseShader->SetVectorUniform("uTexRect", textureRect);
    mBaseShader->SetVectorUniform("uCameraPos", cameraPos);

    if(vertices)
    {
        vertices->SetActive();
    }

    if(texture)
    {
        texture->SetActive();
        mBaseShader->SetFloatUniform("uTextureFactor", textureFactor);
    }
    else {
        mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    }

    if (mode == RendererMode::LINES)
    {
        glDrawElements(GL_LINE_LOOP, vertices->GetNumIndices(), GL_UNSIGNED_INT,nullptr);
    }
    else if(mode == RendererMode::TRIANGLES)
    {
        glDrawElements(GL_TRIANGLES, vertices->GetNumIndices(), GL_UNSIGNED_INT,nullptr);
    }
}

void Renderer::DrawRect(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                        const Vector2 &cameraPos, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, mSpriteVerts, color);
}

void Renderer::DrawTexture(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                           Texture *texture, const Vector4 &textureRect, const Vector2 &cameraPos, bool flip,
                           float textureFactor)
{
    float flipFactor = flip ? -1.0f : 1.0f;

    Matrix4 model = Matrix4::CreateScale(Vector3(size.x * flipFactor, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(RendererMode::TRIANGLES, model, cameraPos, mSpriteVerts, color, texture, textureRect, textureFactor);
}

void Renderer::DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                            const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, vertexArray, color);
}

void Renderer::Present()
{
	// Swap the buffers
	SDL_GL_SwapWindow(mWindow);
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mBaseShader = new Shader();
	if (!mBaseShader->Load("../Shaders/Base")) {
		return false;
	}

	mBaseShader->SetActive();

    return true;
}

void Renderer::CreateSpriteVerts()
{
    float vertices[] = {
        -0.5f,  0.5f, 0.f, 1.f,
         0.5f,  0.5f, 1.f, 1.f,
         0.5f, -0.5f, 1.f, 0.f,
        -0.5f, -0.5f, 0.f, 0.f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}


Texture* Renderer::GetTexture(const std::string& fileName)
{
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName))
        {
            mTextures.emplace(fileName, tex);
            return tex;
        }
        else
        {
            delete tex;
            return nullptr;
        }
    }
    return tex;
}