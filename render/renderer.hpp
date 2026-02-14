#pragma once

#ifndef PSP_ENGINE_RENDER
#define PSP_ENGINE_RENDER

#include"../core/core.hpp"

#define BUFFER_WIDTH (512)
#define BUFFER_HEIGHT (272)
#define SCREEN_WIDTH (480)
#define SCREEN_HEIGHT BUFFER_HEIGHT

// +==================================================================
// |                           STRUCTS                               |
// +==================================================================

// +============================+
// |           VERTEX           |
// +============================+

/*
* Estrutura de vertice para renderização, sem normais
*/
typedef struct{
    float u, v;
    unsigned int cor;
    float x, y, z;
} Vertex;

// +============================+
// |           MESH             |
// +============================+

typedef struct{
    Vertex* vertices;
    u32 num_vertices;
    u16* indices;
    u32 qnt_indices;
}Mesh;

// +============================+
// |          TEXTURES          |
// +============================+

/*
@param width: tem que ser potencia de 2 Ex: 2, 4, 16, 32, 64
@param height: é livre
*/
typedef struct{
    int width, height;
    void* data;
} Texture;

/*
Posicao x e y na textura em coordenadas de pixel
tamanho w e h da sub textura em coordenadas de pixel ex: 16p x 16p
*/
typedef struct {
    int x, y;
    int w, h;
    Texture* tex;
} sub_texture;

// ====================================================================================================
//                  FUNÇÕES INICIALIZAÇÃO PSP GU                                                      |
// ====================================================================================================

// Get Memory Size
unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm);

// Vram Buffer Request
void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm);

// Vram Texture Request
void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm);

/*Inicaliza a Graphics Unit do PsP e cria a ViewPort e os Buffers*/
void initGu();

//finaliza o Graphic Unit
void endGu();

/*Inicia a gpu para desenho*/
void startFrame();

//Limpa a tela
void clear(unsigned int color);

/*fecha a gpu para desenho*/
void endFrame();

void set_renderTarget(Texture* texture);

void reset_drawBuffer();

// ====================================================================================================
//                            FUNÇÕES AUXILIARES                                                      |
// ====================================================================================================

unsigned int pow2(const unsigned int value);

void copy_texture_data(void *dest, const void* src, const unsigned int pW, const unsigned int width, const unsigned int height);

void swizzle_fast(u8 *out, const u8 *in, const unsigned int width, const unsigned int height);

// ====================================================================================================
//                            FUNÇÕES DE CRIAÇÃO                                                      |
// ====================================================================================================

// Criacao de Vertices
Vertex create_vertex(float u, float v, unsigned int color, float x, float y, float z);

/*Aloca o Mesh*/
Mesh* create_mesh(u32 vcount, u32 qnt_index);

// Texturas

Texture* load_texture(const char* filename, const int vram, const int shouldFlip = false);


sub_texture* create_sub_texture(Texture* texture, int px, int py, int sizex, int sizey);

// ====================================================================================================
//                            FUNÇÕES DE DESTRUIÇÃO                                                   |
// ====================================================================================================

//******************************
//     Destrutor de Vertex     *
//******************************

void destroy_vertex(Vertex* vert);

//******************************
//     Destrutor de Texturas   *
//******************************

void destroy_texture(Texture* tex);

//*****************************
//      Destrutor de MESH     *
//*****************************

void destroy_mesh(Mesh* mesh);

// ====================================================================================================
//                            FUNÇÕES TEXTURA                                                         |
// ====================================================================================================

//***********************************
// Funções de Auxiliares de Textura *
//***********************************

void should_blend(bool should);

void get_uv_index(Texture* tex, int x, int y, int sizeX, int sizeY, float* buf);


//*****************************
// Funções de Bind de textura *
//*****************************


void bind_texture(Texture *tex, int TFX_MODE = GU_TFX_REPLACE, int swizzle = 1);


void bind_sub_texture(sub_texture* stexture, Mesh* mesh, int TFX_MODE = GU_TFX_REPLACE, int swizzle = 1,int offset = 0);


// ====================================================================================================
//                            FUNÇÕES DE DESENHAR                                                     |
// ====================================================================================================

 
//*************************************
// Funções de Desenho de Vertex bruto *
//*************************************

/*
*@param DrawMode: GU_TRANSFORM_3D ou GU_TRANSFORM_2D
*/
void render_vertex(Vertex* model, unsigned short* indexes, unsigned int indices_count, int DrawMode = GU_TRANSFORM_3D);


//*****************************
// Funções de Desenho de MESH *
//*****************************

/*
*@param MODE: GU_TRANSFORM_3D ou GU_TRANSFORM_2D
*/
void draw_mesh(Mesh* mesh, int offset = 0, unsigned int MODE = GU_TRANSFORM_3D);

/*
@params texture: uma textura para renderizar em 2d na tela
@params TFX_MODE = modo de mistura da placa
*/
void draw_texture(Texture * texture, float x, float y, float w, float h, int TFX_MODE = GU_TFX_REPLACE, int swizzle = 1);

/*
@params texture: uma subtextura para renderizar em 2d na tela
@params TFX_MODE = modo de mistura da placa
*/
void draw_texture(sub_texture* texture, float x, float y, float w, float h, int TFX_MODE = GU_TFX_REPLACE);


/*
* Desenha uma textura na tela em em 3D
@param tex: textura
@param w: escala em x
@param h: escala em y
*/
void draw_sprite(Texture* tex, float w = 0, float h = 0);

/*
* Desenha uma sub_textura na tela em 3D
@param tex: textura
@param w: escala em x
@param h: escala em y
*/
void draw_sprite(sub_texture* tex, float w = 0, float h = 0);

/*
*Renderiza um Quadrado em 2D na tela
@param xc: x center
@param yc: y center
@param w: escala em x
@param h: escala em y
@param cor: cor do quadrado
*/
void draw_square(int xc, int yc, int w, int h, unsigned int cor);

/*
*Renderiza um Quadrado 2D em 3D na tela
@param w: escala em x
@param h: escala em y
@param cor: cor do quadrado
*/
void draw_square2D(float w, float h, unsigned int cor, int type = GU_TRANSFORM_3D);

/*
*Desenha um ponto 3D no mundo
*/
void draw_point(unsigned int cor);

/*
*Desenha uma linha 3D no mundo
*/
void draw_line3D(vec3 pi, vec3 pf, unsigned int cor);


/*
* Desenha um circulo 3D na tela
*/
void draw_Circle3D(float radius, unsigned int color);


#endif