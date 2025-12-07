#pragma once
#include "../Math.h"
#include <string>
#include <vector>

class UIKeyPrompt
{
public:
    // size: Tamanho do quadrado/retângulo da tecla
    // fontSize: Tamanho da fonte
    UIKeyPrompt(class Game* game, const Vector2& consolidatedPosition, const std::string& key, const std::string& description, int drawOrder, float xSize = 40.0f);
    ~UIKeyPrompt();

    void SetVisible(bool visible);

private:
    class Game* mGame;
    
    // Atores componentes do widget
    class Actor* mKeyBgActor;   // O quadrado
    class Actor* mKeyTextActor; // O texto "J", "K", etc
    class Actor* mDescTextActor;// O texto "Attack", etc

    // Componentes para controle direto (se precisar mudar cor/visibilidade)
    class RectComponent* mBgRect;
    class UITextComponent* mKeyText;
    class UITextComponent* mDescText;
};