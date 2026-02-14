#include "renderer.hpp"
// STBI LIB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ====================================================================================================
//                  FUNÇÕES INICIALIZAÇÃO PSP GU                                                      |
// ====================================================================================================

void* fbp0;
void* fbp1;
void* zbp;

static unsigned int list[262144] __attribute__((aligned(16)));

// Get Memory Size
unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm)
{
	switch (psm)
	{
		case GU_PSM_T4:
			return (width * height) >> 1;

		case GU_PSM_T8:
			return width * height;

		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;

		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;

		default:
			return 0;
	}
}

// Vram Buffer Request
void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
	static unsigned int staticOffset = 0;
	unsigned int memSize = getMemorySize(width,height,psm);
	void* result = (void*)staticOffset;
	staticOffset += memSize;

	return result;
}

// Vram Texture Request
void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm)
{
	void* result = getStaticVramBuffer(width,height,psm);
	return (void*)(((unsigned int)result) + ((unsigned int)sceGeEdramGetAddr()));
}

/*Inicaliza a Graphics Unit do PsP e cria a ViewPort e os Buffers*/
void initGu(){
    fbp0 = getStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
    fbp1 = getStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
    zbp = getStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_4444);

    sceGuInit();

    //seta os buffers
    sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUFFER_WIDTH);
	sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,fbp1,BUFFER_WIDTH);
	sceGuDepthBuffer(zbp,BUFFER_WIDTH);

	sceGuOffset(2048 - (SCREEN_WIDTH/2),2048 - (SCREEN_HEIGHT/2));
	sceGuViewport(2048,2048,SCREEN_WIDTH,SCREEN_HEIGHT);

	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);

	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);

	sceGuFinish();
	sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

//finaliza o Graphic Unit
void endGu(){
    sceGuTerm();
}

/*Inicia a gpu para desenho*/
void startFrame(){
    sceGuStart(GU_DIRECT, list);
}

//Limpa a tela
void clear(unsigned int color){
    sceGuClearColor(color);
    sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT|GU_STENCIL_BUFFER_BIT);
}

/*fecha a gpu para desenho*/
void endFrame(){
    sceGuFinish();
    sceGuSync(GU_SYNC_WAIT, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

void set_renderTarget(Texture* texture){
    sceGuDrawBufferList(GU_PSM_8888,(void*)texture->data, texture->width);
    sceGuOffset(2048 - (texture->width/2),2048 - (texture->height/2));
	sceGuViewport(2048,2048,texture->width,texture->height);
}

void reset_drawBuffer(){
    sceGuDrawBufferList(GU_PSM_8888,(void*)fbp0, BUFFER_WIDTH);
    sceGuOffset(2048 - (SCREEN_WIDTH/2),2048 - (SCREEN_HEIGHT/2));
	sceGuViewport(2048,2048,SCREEN_WIDTH,SCREEN_HEIGHT);
}

// ====================================================================================================
//                            FUNÇÕES AUXILIARES                                                      |
// ====================================================================================================

unsigned int pow2(const unsigned int value){
    unsigned int porft = 1;
    while(porft < value){
        porft =  porft * 2;
    }
    return porft;
}

void copy_texture_data(void *dest, const void* src, const unsigned int pW, const unsigned int width, const unsigned int height){
    for(unsigned int y = 0; y < height; y++){
        for(unsigned int x = 0; x < width; x++){
            ((unsigned int*)dest)[x + y * pW] = ((unsigned int*)src)[x + y * width];
        }
    }
}

void swizzle_fast(u8 *out, const u8 *in, const unsigned int width, const unsigned int height) {
    unsigned int blockx, blocky;
    unsigned int j;

    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width - 16) / 4;
    unsigned int src_row = width * 8;

    const u8 *ysrc = in;
    u32 *dst = (u32 *)out;

    for (blocky = 0; blocky < height_blocks; ++blocky) {
        const u8 *xsrc = ysrc;
        for (blockx = 0; blockx < width_blocks; ++blockx) {
            const u32 *src = (u32 *)xsrc;
            for (j = 0; j < 8; ++j) {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

// Criacao de Vertices
Vertex create_vertex(float u, float v, unsigned int color, float x, float y, float z){
    Vertex vert = {
        .u = u,
        .v = v,
        .cor = color,
        .x = x,
        .y = y,
        .z = z
    };
    return vert;
}

/*Aloca o Mesh*/
Mesh* create_mesh(u32 vcount, u32 qnt_index){
    Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
    if(mesh == NULL)
        return NULL;
    
    mesh->vertices = (Vertex*)memalign(16, sizeof(Vertex)* vcount);
    if(mesh->vertices == NULL){
        free(mesh);
        return NULL;
    }
    mesh->num_vertices = vcount;

    if(qnt_index != 0){
        mesh->indices = (u16*)memalign(16, sizeof(u16)* qnt_index);
        if(mesh->indices == NULL){
            free(mesh->vertices);
            free(mesh);
            return NULL;
        }
    }else{
        mesh->indices = NULL;
    }
    mesh->qnt_indices = qnt_index;

    return mesh;
}

// Texturas

Texture* load_texture(const char* filename, const int vram, const int shouldFlip) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(shouldFlip);
    unsigned char *data = stbi_load(filename, 
                                    &width, &height,
                                    &nrChannels, STBI_rgb_alpha);
    if(!data)
        return NULL;

    Texture* tex = (Texture*)malloc(sizeof(Texture));
    tex->width = width;
    tex->height = height;

    unsigned int *dataBuffer =
        (unsigned int *)memalign(16, tex->height * tex->width * 4);

    // Copy to Data Buffer
    copy_texture_data(dataBuffer, data, tex->width, tex->width, tex->height);

    // Free STB Data
    stbi_image_free(data);

    unsigned int* swizzled_pixels = NULL;
    size_t size = tex->height * tex->width * 4;
    if(vram){
        swizzled_pixels = (unsigned int *)getStaticVramTexture(tex->width, tex->height, GU_PSM_8888);
    } else {
        swizzled_pixels = (unsigned int *)memalign(16, size);
    }
    
    swizzle_fast((u8*)swizzled_pixels, (const u8*)dataBuffer, tex->width * 4, tex->height);

    free(dataBuffer);
    tex->data = swizzled_pixels;

    sceKernelDcacheWritebackInvalidateAll();

    return tex;
}


sub_texture* create_sub_texture(Texture* texture, int px, int py, int sizex, int sizey) {
    sub_texture* stexture = (sub_texture*)malloc(sizeof(sub_texture));
    if(stexture == NULL)
        return NULL;
    stexture->tex = texture;
    stexture->x = px;
    stexture->y = py;
    stexture->w = sizex;
    stexture->h = sizey;

    return stexture;
}



// ====================================================================================================
//                            FUNÇÕES DE DESTRUIÇÃO                                                   |
// ====================================================================================================

//******************************
//     Destrutor de Vertex     *
//******************************

void destroy_vertex(Vertex* vert){
    free(vert);
}

//******************************
//     Destrutor de Texturas   *
//******************************

void destroy_texture(Texture* tex){
    free(tex->data);
    free(tex);
}

//*****************************
//      Destrutor de MESH     *
//*****************************

void destroy_mesh(Mesh* mesh) {
    free(mesh->vertices);
    free(mesh->indices);
    free(mesh);
}

// ====================================================================================================
//                            FUNÇÕES TEXTURA                                                         |
// ====================================================================================================

//***********************************
// Funções de Auxiliares de Textura *
//***********************************

void should_blend(bool should){
    if(should){
        sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuEnable(GU_BLEND);
    }else{
        sceGuDisable(GU_BLEND);
    }
}

void get_uv_index(Texture* tex, int x, int y, int sizeX, int sizeY, float* buf) {
    float sizeXX = ((float)sizeX) / ((float)tex->width); //1 pixel
    float sizeYY = ((float)sizeY) / ((float)tex->height); // 1 pixel

    float xi = ((float)x) / ((float)tex->width);
    float yi = ((float)y) / ((float)tex->height);
    float w = xi + sizeXX;
    float h = yi + sizeYY;
    
    // 0 0 -> 0 1
    buf[0] = xi;
    buf[1] = h;
    // 0 1 -> 0 0
    buf[2] = xi;
    buf[3] = yi;
    // 1 1 -> 1 0
    buf[4] = w;
    buf[5] = yi;
    // 1 0 -> 1 1
    buf[6] = w;
    buf[7] = h;
}


//*****************************
// Funções de Bind de textura *
//*****************************


void bind_texture(Texture *tex, int TFX_MODE, int swizzle){
    if(!tex)
        return;
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, swizzle);
    sceGuTexFunc(TFX_MODE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_REPEAT, GU_REPEAT);
    sceGuTexScale(1.0f,1.0f);
	sceGuTexOffset(0.0f,0.0f);
    sceGuTexImage(0, tex->width, tex->height, tex->width, tex->data);
}


void bind_sub_texture(sub_texture* stexture, Mesh* mesh, int TFX_MODE, int swizzle, int offset) {
    bind_texture(stexture->tex, TFX_MODE, swizzle);

    float buf[8];
    get_uv_index(stexture->tex, stexture->x, stexture->y, stexture->w, stexture->h, buf);
    
    mesh->vertices[offset * 4 + 0].u = buf[0]; mesh->vertices[offset * 4 + 0].v = buf[1];
    mesh->vertices[offset * 4 + 1].u = buf[2]; mesh->vertices[offset * 4 + 1].v = buf[3];
    mesh->vertices[offset * 4 + 2].u = buf[4]; mesh->vertices[offset * 4 + 2].v = buf[5];
    mesh->vertices[offset * 4 + 3].u = buf[6]; mesh->vertices[offset * 4 + 3].v = buf[7];

    sceKernelDcacheWritebackInvalidateAll();
}


// ====================================================================================================
//                            FUNÇÕES DE DESENHAR                                                     |
// ====================================================================================================

 
//*************************************
// Funções de Desenho de Vertex bruto *
//*************************************

/*
*@param DrawMode: GU_TRANSFORM_3D ou GU_TRANSFORM_2D
*/
void render_vertex(Vertex* model, unsigned short* indexes, unsigned int indices_count, int DrawMode) {
    if(indexes != NULL){
        sceGumDrawArray(GU_TRIANGLES,                                      // Primitiva: Desenha uma "tira" de triângulos
                        GU_INDEX_16BIT | GU_TEXTURE_32BITF | 
                        GU_COLOR_8888| GU_VERTEX_32BITF | DrawMode, // Formato do vértice
                        indices_count,                                     // Número de vértices a desenhar
                        indexes,                                           // índices
                        model);
    }else{
        sceGumDrawArray(GU_TRIANGLES,                       // Primitiva: Desenha uma "tira" de triângulos
                        GU_TEXTURE_32BITF | GU_COLOR_8888| 
                        GU_VERTEX_32BITF | DrawMode, // Formato do vértice
                        indices_count,                      // Número de vértices a desenhar
                        0,                                  // índices
                        model);
    }
}


//*****************************
// Funções de Desenho de MESH *
//*****************************

/*
*@param MODE: GU_TRANSFORM_3D ou GU_TRANSFORM_2D
*/
void draw_mesh(Mesh* mesh, int offset, unsigned int MODE){
    if(mesh->indices && mesh->qnt_indices > 0){
        sceGumDrawArray(GU_TRIANGLES, 
                    GU_INDEX_16BIT | GU_TEXTURE_32BITF | 
                    GU_COLOR_8888 | GU_VERTEX_32BITF | MODE,
                    mesh->qnt_indices, mesh->indices, mesh->vertices
        );
    }else{
        sceGumDrawArray(GU_TRIANGLES, 
                    GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | MODE, 
                    (mesh->num_vertices-offset), NULL, (mesh->vertices+offset)
        );
    }
    
}

/*
@params texture: uma textura para renderizar em 2d na tela
@params TFX_MODE = modo de mistura da placa
*/
void draw_texture(Texture * texture, float x, float y, float w, float h, int TFX_MODE, int swizzle) {
    static Vertex vertices[2];

    vertices[0].u = 0;
    vertices[0].v = h;
    vertices[0].cor = 0xFFFFFFFF;
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    vertices[1].u = w;
    vertices[1].v = 0.0f;
    vertices[1].cor = 0xFFFFFFFF;
    if(x + w < SCREEN_WIDTH){
        vertices[1].x = x + w;
    }else{
        vertices[1].x = SCREEN_WIDTH;
    }

    if(y + h < SCREEN_HEIGHT){
        vertices[1].y = y + h;
    }else{
        vertices[1].y = SCREEN_HEIGHT;
    }
    vertices[1].z = 0.0f;

    sceGuTexMode(GU_PSM_8888, 0, 0, swizzle);
    sceGuTexFunc(TFX_MODE, GU_TCC_RGBA);
    sceGuTexImage(0, texture->width, texture->height, texture->width, texture->data);

    //bind_texture(texture, TFX_MODE, swizzle);

    sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuDisable(GU_TEXTURE_2D);
}

/*
@params texture: uma subtextura para renderizar em 2d na tela
@params TFX_MODE = modo de mistura da placa
*/
void draw_texture(sub_texture* texture, float x, float y, float w, float h, int TFX_MODE) {
    static Vertex v[2];

    v[0].u = texture->x;
    v[0].v = texture->y;
    v[0].cor = 0xFFFFFFFF;
    v[0].x = x;
    v[0].y = y;
    v[0].z = 0.0f;

    v[1].u = texture->x + texture->w;
    v[1].v = texture->y + texture->h;
    v[1].cor = 0xFFFFFFFF;
    v[1].x = x + w;
    v[1].y = y + h;
    v[1].z = 0.0f;

    bind_texture(texture->tex, TFX_MODE);
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, v);
    sceGuDisable(GU_TEXTURE_2D);
}


/*
* Desenha uma textura na tela em em 3D
@param tex: textura
@param w: escala em x
@param h: escala em y
*/
void draw_sprite(Texture* tex, float w, float h){
    static Vertex vertice[4];
    vertice[0] = {0, 0, 0xffffffff, -1.0f - (w/2.0f),-1.0f - (h/2.0f), 1.0f}; // 0
    vertice[1] = {0, 1, 0xffffffff, -1.0f - (w/2.0f), 1.0f + (h/2.0f), 1.0f}; // 1
    vertice[2] = {1, 1, 0xffffffff, 1.0f + (w/2.0f), 1.0f + (h/2.0f), 1.0f}; // 2
    vertice[3] = {1, 0, 0xffffffff, 1.0f + (w/2.0f),-1.0f - (h/2.0f), 1.0f}; // 3

    static u16 indices[6]{
        0, 1, 2, 0, 2, 3
    };

    Mesh m = {.vertices = vertice, .indices = indices, .qnt_indices = 6};
    bind_texture(tex);
    draw_mesh(&m, 0, GU_TRANSFORM_3D);
}

/*
* Desenha uma sub_textura na tela em 3D
@param tex: textura
@param w: escala em x
@param h: escala em y
*/
void draw_sprite(sub_texture* tex, float w, float h){
    static Vertex vertice[4];
    //centrado em 0
    vertice[0] = {0, 0, 0xffffffff, -1.0f - (w/2.0f), -1.0f - (h/2.0f), 1.0f}; // 0
    vertice[1] = {0, 1, 0xffffffff, -1.0f - (w/2.0f),  1.0f + (h/2.0f), 1.0f}; // 1
    vertice[2] = {1, 1, 0xffffffff,  1.0f + (w/2.0f),  1.0f + (h/2.0f), 1.0f}; // 2
    vertice[3] = {1, 0, 0xffffffff,  1.0f + (w/2.0f), -1.0f - (h/2.0f), 1.0f}; // 3

    static u16 indices[6]{
        0, 3, 2, 0, 2, 1
    };
    Mesh m = {.vertices = vertice, .indices = indices, .qnt_indices = 6};
    bind_sub_texture(tex, &m);
    draw_mesh(&m, 0, GU_TRANSFORM_3D);
}

/*
*Renderiza um Quadrado em 2D na tela
@param xc: x center
@param yc: y center
@param w: escala em x
@param h: escala em y
@param cor: cor do quadrado
*/
void draw_square(int xc, int yc, int w, int h, unsigned int cor){
    static Vertex ve[2];

    ve[0].u = 0;
    ve[0].v = 0;
    ve[0].cor = cor;
    ve[0].x = xc - w;
    ve[0].y = yc - h;
    ve[0].z = 0.0f;

    ve[1].u = w;
    ve[1].v = h;
    ve[1].cor = cor;
    ve[1].x = xc + w;
    ve[1].y = yc + h;
    ve[1].z = 0.0f;
   
    sceGuColor(cor);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | 
                   GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, ve);
}


/*
*Renderiza um Quadrado2D em 3D na tela
@param w: escala em x
@param h: escala em y
@param cor: cor do quadrado
*/
void draw_square2D(float w, float h, unsigned int cor, int type){
    static Vertex vertice[4];
    //centrado em 0
    vertice[0] = {0, 0, cor, -1.0f - (w/2.0f), -1.0f - (h/2.0f), 1.0f}; // 0
    vertice[1] = {0, 0, cor, -1.0f - (w/2.0f),  1.0f + (h/2.0f), 1.0f}; // 1
    vertice[2] = {0, 0, cor,  1.0f + (w/2.0f),  1.0f + (h/2.0f), 1.0f}; // 2
    vertice[3] = {0, 0, cor,  1.0f + (w/2.0f), -1.0f - (h/2.0f), 1.0f}; // 3

    static u16 indices[6]{
        0, 3, 2, 0, 2, 1
    };
    //Mesh m = {.vertices = vertice, .indices = indices, .qnt_indices = 6};
   
    sceGuColor(cor);
    sceGuDisable(GU_TEXTURE_2D);
    //draw_mesh(&m, 0, GU_TRANSFORM_3D);
    sceGumDrawArray(GU_TRIANGLES, 
                   GU_INDEX_16BIT | GU_TEXTURE_32BITF | 
                   GU_COLOR_8888 | GU_VERTEX_32BITF | 
                   GU_TRANSFORM_3D, 
                   6, indices, vertice);
}

/*
*Desenha um ponto 3D no mundo
*/
void draw_point(unsigned int cor){
    static Vertex vertice[1];
    //------------------------------------------
    vertice[0].u = 0.0f; vertice[0].v = 0.0f;
    vertice[0].cor = cor;
    vertice[0].x = 0.0f; vertice[0].y = 0.0f; vertice[0].z = 0.0f;
    //----------------
    sceGuColor(cor);
    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_POINTS,
                    GU_COLOR_8888 | GU_TEXTURE_32BITF | 
                    GU_VERTEX_32BITF | GU_TRANSFORM_3D, 
                    1,
                    NULL, // Sem buffer de índices
                    vertice);
}

/*
*Desenha uma linha 3D no mundo
*/
void draw_line3D(vec3 pi, vec3 pf, unsigned int cor){
     static Vertex ve[2];

    ve[0].u = 0;
    ve[0].v = 0;
    ve[0].cor = cor;
    ve[0].x = pi.x;
    ve[0].y = pi.y;
    ve[0].z = pi.z;

    ve[1].u = 1;
    ve[1].v = 1;
    ve[1].cor = cor;
    ve[1].x = pf.x;
    ve[1].y = pf.y;
    ve[1].z = pf.z;
   
    sceGuColor(cor);
    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_LINES, 
                    GU_COLOR_8888 | GU_TEXTURE_32BITF | 
                    GU_VERTEX_32BITF | GU_TRANSFORM_3D, 
                    2, 0, ve);
}

/*
* Desenha um circulo 3D na tela
*/
void draw_Circle3D(float radius, unsigned int color){
    static Vertex vertices[36];
    // Validação básica
    if (radius <= 0.0f) {
        return;
    }

    // GU_TRIANGLE_FAN precisa de: 1 vértice central + N vértices na borda + 1 repetido para fechar
    int num_segments = 34;
    int total_vertices = num_segments + 2;

    // 1. Define o vértice central
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;
    vertices[0].cor = color;
    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 0.0f; // Assumindo Z=0 para 2D/planos

    // 2. Calcula e define os vértices na circunferência
    float angle_step = (2.0f * M_PI) / num_segments;
    for (int i = 0; i <= num_segments; ++i) { // Loop num_segments + 1 vezes para fechar o fã
        float current_angle = i * angle_step;
        int vertex_index = i + 1; // Começa a preencher a partir do índice 1
        //-----------------------------
        vertices[vertex_index].u = 0.0f;
        vertices[vertex_index].v = 0.0f;
        vertices[vertex_index].cor = color;
        vertices[vertex_index].x = radius * cosf(current_angle);
        vertices[vertex_index].y = radius * sinf(current_angle);
        vertices[vertex_index].z = 0.0f;
    }

    // 3. Desenha o fã de triângulos
    // Use GU_TRANSFORM_3D se estiver usando a pipeline de matrizes (recomendado)
    // Use GU_TRANSFORM_2D se xc, yc forem coordenadas de tela e você não usar matrizes
    sceGuColor(color);
    sceGuDisable(GU_TEXTURE_2D);
    sceGumDrawArray(GU_TRIANGLE_FAN,
                    GU_COLOR_8888 | GU_TEXTURE_32BITF | 
                    GU_VERTEX_32BITF | GU_TRANSFORM_3D, 
                    total_vertices,
                    NULL, // Sem buffer de índices
                    vertices);
}
