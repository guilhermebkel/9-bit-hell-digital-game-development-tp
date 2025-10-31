# 9-bit Hell

> 8 bits weren't enough for Hell.

Este é o repositório do projeto final para a disciplina de Desenvolvimento de Jogos Digitais. "9-bit Hell" é um jogo de ação *top-down shooter* com uma estética retrô em pixel art, inspirado na jornada pelos nove círculos do Inferno de Dante Alighieri.

## Resumo da Ideia

Em "9-bit Hell", o jogador assume o papel de uma alma perdida, condenada a atravessar os nove círculos do Inferno em busca de redenção. A jogabilidade é inspirada em clássicos do gênero *shooter*, como *Asteroids*, mas com uma temática sombria e uma mecânica central única: a **Corrupção Progressiva**.

Quanto mais tempo o jogador permanece em uma fase, mais sua visão é reduzida por um efeito de vinheta que escurece a tela, aumentando a tensão e a dificuldade. Para sobreviver, o jogador deve derrotar hordas de inimigos, coletar moedas para melhorias e encontrar itens de purificação que limpam temporariamente a corrupção da tela. A jornada representa uma luta contra a própria corrupção interior, refletida na degradação visual e no desafio crescente.

### Funcionalidades Principais

*   **Jogabilidade Top-Down Shooter:** Movimentação livre em 360º e sistema de combate corpo a corpo.
*   **Mecânica de Corrupção:** A visão do jogador diminui com o tempo, criando uma atmosfera opressiva e um desafio constante.
*   **Perspectiva 2.5D:** A ordem de renderização é baseada na posição Y dos personagens, criando uma ilusão de profundidade similar a jogos clássicos de *beat 'em up*.
*   **Itens Colecionáveis:** Colete moedas para futuras melhorias e itens de purificação para restaurar sua visão.
*   **Estética Retrô:** Arte e som inspirados na era 8-bit, mas com um toque moderno.

## Configuração e Compilação

Para compilar e executar o projeto localmente, você precisará de um ambiente de desenvolvimento C++ e algumas bibliotecas.

### Dependências

*   **Compilador C++:** Um compilador com suporte a C++11 ou superior (ex: GCC, Clang, MSVC).
*   **CMake:** Versão 3.10 ou superior.
*   **SDL2:** Biblioteca principal para criação de janela, input e renderização.
*   **SDL2_image:** Biblioteca auxiliar para carregar formatos de imagem como PNG.
*   **GLEW:** Biblioteca para gerenciamento de extensões OpenGL.

#### Instalação das Dependências (Exemplo para Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libglew-dev
```

### Instruções de Compilação

O projeto utiliza CMake para gerar os arquivos de build. O processo a seguir é o padrão para compilação fora da árvore de fontes (*out-of-source build*).

1.  **Crie um diretório de build:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Execute o CMake para gerar os arquivos de projeto:**
    ```bash
    cmake ..
    ```

3.  **Compile o código:**
    ```bash
    make
    ```
    *Se estiver usando um gerador diferente (como no Windows com Visual Studio), o CMake criará uma solução (`.sln`) que você pode abrir e compilar.*

### Como Jogar

Após a compilação, o executável estará dentro da pasta `build`.

```bash
# Dentro da pasta 'build'
./9-bit-Hell  # O nome do executável pode variar dependendo da configuração do CMakeLists.txt
```

### Controles

*   **W, A, S, D:** Movimento do Jogador
*   **J:** Ação de Ataque Corpo a Corpo
*   **K:** Ação de Ataque à Distância
*   **Enter:** Selecionar Opção

## Integrantes

*   **Gabriel Vieira Pereira da Silva** - 2020105777
*   **Guilherme Mota Bromonschenkel Lima** - 2019027571
*   **Pedro Medina Lara Silva** - 2022423431
