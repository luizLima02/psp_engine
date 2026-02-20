#include"trans_proj.hpp"

// =====================
//     TRANSFORMAÇÕES
// =====================

void reset_translate(float x, float y, float z){
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    vec3 v = {x, y, z};
    sceGumTranslate(&v);
}

void scale(float x, float y, float z){
    sceGumMatrixMode(GU_MODEL);
    vec3 v = {x, y, z};
    sceGumScale(&v);
}

void rotateXYZ(float angle, float x, float y, float z){
    sceGumMatrixMode(GU_MODEL);
    vec3 v = {x*angle, y*angle, z*angle};
    sceGumRotateXYZ(&v);
}

// ======================
//      PERSPECTIVA
// ======================

void init_perspective_2D(float left, float right, float top, float down, float near, float far){
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(left, right, top, down, near, far);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
}

void init_perspective_2D_Especial(float fovy, float aspect, float near, float far){
    float top = -near* tanf(fovy / 2.0f);
    float down = -top;
    float right = -top * aspect;
    float left = -right;

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(left, right, top, down, near, far);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
}

void init_perspective_3D(float fovy, float aspect, float near, float far){
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(fovy, aspect, near, far);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
}



void billboard_to_camera(const vec3 objectPosition,
                         const vec3 cameraPosition)
{
    // P -> camera
    // E -> player
    // F = P - E
    vec3 F = {cameraPosition.x - objectPosition.x, 
                        cameraPosition.y - objectPosition.y, 
                        cameraPosition.z - objectPosition.z};
    //R = (0, 1, 0) x F
    vec3 R;
    vec3 wUp = {0, 1, 0};
    gumCrossProduct(&R, &wUp, &F);
    //u = F X R
    vec3 U;
    gumCrossProduct(&U, &F, &R);
    gumNormalize(&U);
    gumNormalize(&F);
    gumNormalize(&R);

    mat4 modelMat;
    gumLoadIdentity(&modelMat);
    //Coluna X
    modelMat.x.x = R.x; modelMat.x.y = R.y; modelMat.x.z = R.z; modelMat.x.w = 0.0f;
    //Coluna Y
    modelMat.y.x = U.x; modelMat.y.y = U.y; modelMat.y.z = U.z; modelMat.y.w = 0.0f;
    //Coluna Z
    modelMat.z.x = F.x; modelMat.z.y = F.y; modelMat.z.z = F.z; modelMat.z.w = 0.0f;
    //Coluna W
    modelMat.w.x = objectPosition.x; modelMat.w.y = objectPosition.y; modelMat.w.z = objectPosition.z; modelMat.w.w = 1.0f;
    sceGumMatrixMode(GU_MODEL);
    sceGumLoadMatrix(&modelMat);
}