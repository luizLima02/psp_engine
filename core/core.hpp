#pragma once

#ifndef PSP_ENGINE_CORE
#define PSP_ENGINE_CORE

#include <stdio.h>
#include <stdlib.h> // for malloc() e free()
#include <string.h>  // for strlen(), strcpy() e strcat()
#include <math.h>
#include <memory.h>
#include <malloc.h>
#include <stdbool.h>
#include <string>

//PSPSDK dependences
#include <psptypes.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include <pspgu.h>

//redefinition of functions

#define logF pspDebugScreenPrintf
#define readController sceCtrlReadBufferPositive

//Sprite Height unit
#define SPRITE_HU_SIZE 32.0f
//Sprite Width unit
#define SPRITE_WU_SIZE 32.0f

//============================================================
//
//                          AUX
//
//============================================================

int size_of_string(const char* s);

/**
 * @brief Concatena duas strings (s1 e s2) em uma nova string alocada dinamicamente.
 * @param s1 A primeira string (prefixo).
 * @param s2 A segunda string (sufixo).
 * @return Um ponteiro para a nova string concatenada.
 * Retorna NULL se a alocação de memória falhar.
 * @note A memória retornada por esta função DEVE ser liberada pelo chamador usando free().
 */
const char* concatString(const char* s1, const char* s2);

static inline int maxi(int a, int b){
    if(a > b){return a;}
    else{return b;}
}

static inline int mini(int a, int b){
    if(a < b){return a;}
    else{return b;}
}

static inline float clampF(float value, float min, float max){
    if(value < min){return min;}
    if(value > max){return max;}
    return value;
}

//============================================================
//
//                   PSP CALLBACKS
//
//============================================================

int exit_callback(int arg1, int arg2, void *common);

int callback_thread(SceSize args, void *argp);

int setup_callbacks(void);

//===============
//   GamePad
//===============

#define PSP_CONTROLLER SceCtrlData

/*
* coloca o valor passado no axis de [-1 a 1]
*/
static inline float normaliza_axis(unsigned char ax){
    int axis = ax;
    float valor = ((float)axis - 128.0f)/128.0f;
    return valor;
}

//===============
//   delta time
//===============

extern float delta_time;
extern SceInt64 last_time;

void calc_delta();

//============================================================
//
//                   VETORES - MATRIZES
//
//============================================================

#define vec4 ScePspFVector4
#define vec3 ScePspFVector3
#define vec2 ScePspFVector2

#define mat4 ScePspFMatrix4
#define mat3 ScePspFMatrix3
#define mat2 ScePspFMatrix2


//-------------------------
//   construtores vetores
//-------------------------
/**
 * @brief Create a vec2
 */
vec2 Vec2(float x = 0, float y = 0);
/**
 * @brief Create a vec3
 */
vec3 Vec3(float x = 0, float y = 0, float z = 0);
/**
 * Discarts the w position and return the vec3
 */
vec3 Vec3(vec4 v);

/**
 * @brief Create a vec4
 */
vec4 Vec4(float x = 0, float y = 0, float z = 0, float w = 0);
/**
 * Add a w position and return the vec4
 */
vec4 Vec4(vec3 v, float w = 0 );
//=============================================================
//
//                    funçoes basicas
//
//=============================================================

//--------------------------------------
//                Adição
//--------------------------------------
//vec2
static inline void operator+=(vec2& u, const vec2& v){
    u.x += v.x; 
    u.y += v.y;
}
//vec3
static inline void operator+=(vec3& u, const vec3& v){
    u.x += v.x; 
    u.y += v.y; 
    u.z += v.z;
}
//vec4
static inline void operator+=(vec4& u, const vec4& v){
    u.x += v.x; 
    u.y += v.y; 
    u.z += v.z;
    u.w += v.w;
}

//--------------------------------------
//              Subtração
//--------------------------------------
//vec2
static inline void operator-=(vec2& u, const vec2& v){
    u.x -= v.x; 
    u.y -= v.y; 
}
//vec3
static inline void operator-=(vec3& u, const vec3& v){
    u.x -= v.x; 
    u.y -= v.y; 
    u.z -= v.z;
}
//vec4
static inline void operator-=(vec4& u, const vec4& v){
    u.x -= v.x; 
    u.y -= v.y; 
    u.z -= v.z;
    u.w -= v.w;
}

//--------------------------------------
//              Produto
//--------------------------------------
//vec2
static inline void operator*=(vec2& u, const float &s){
    u.x *= s; 
    u.y *= s; 
}
//vec3
static inline void operator*=(vec3& u, const float &s){
    u.x *= s; 
    u.y *= s; 
    u.z *= s;
}
//vec4
static inline void operator*=(vec4& u, const float &s){ 
    u.x *= s; 
    u.y *= s; 
    u.z *= s;
    u.w *= s;
}

//--------------------------------------
//              Division
//--------------------------------------
//vec2
static inline void operator/=(vec2& u, const float &s){
    u.x /= s; 
    u.y /= s; 
}
//vec3
static inline void operator/=(vec3& u, const float &s){
    u.x /= s; 
    u.y /= s; 
    u.z /= s;
}
//vec4
static inline void operator/=(vec4& u, const float &s){ 
    u.x /= s; 
    u.y /= s; 
    u.z /= s;
    u.w /= s;
}


/*
* Retorna o Cross Product dos vetores Passados
*/
vec3 Cross(vec3 u, vec3 t);

/*
* Retorna o produto escalar dos vetores Passados
*/
float DOT(vec3 t, vec3 outro);

/*
* Retorna o produto escalar dos vetores Passados
*/
float DOT(vec2 t, vec2 outro);

//--------------------------
//          Invert
//--------------------------
/**
 * Inverts the vector
 */
vec2 Invert(vec2 data);
/**
 * Inverts the vector
 */
vec3 Invert(vec3 data);
/**
 * Inverts the vector
 */
vec4 Invert(vec4 data);

//--------------------------------
//             Norma
//--------------------------------

/**
* Get the norm of the vector
* @param v:(vec2) B :vetor para pegar o modulo
* @param u:(vec2) A :vetor para pegar o modulo
*
* Retorn sqrt((A - B)^2)
*/
float Norm(vec2 v, vec2 u = Vec2());

/**
* Get the norm of the vector
* @param v:(vec3) B :vetor para pegar o modulo
* @param u:(vec3) A :vetor para pegar o modulo
*
* Retorna sqrt((A - B)^2)
*/
float Norm(vec3 v, vec3 u= Vec3());
/**
* Get the norm of the vector
* @param v:(vec4) B :vetor para pegar o modulo
* @param u:(vec4) A :vetor para pegar o modulo
*
* Retorna sqrt((A - B)^2)
*/
float Norm(vec4 v, vec4 u = Vec4());

/**
* Normaliza o vetor passado
*/
vec3 Normalize(vec3 t);


//--------------------------------------
//      Especial Functions
//--------------------------------------

//Adiciona o escalar*vetor ao vetor data

static inline void AddScaledVector2(vec2& out, vec2 vec, float scalar){
    out.x += vec.x * scalar;
    out.y += vec.y * scalar;
}

static inline void AddScaledVector3(vec3& out, vec3 vec, float scalar){
    out.x += vec.x * scalar;
    out.y += vec.y * scalar;
    out.z += vec.z * scalar;
}

static inline void AddScaledVector4(vec4& out, vec4 vec, float scalar){
    out.x += vec.x * scalar;
    out.y += vec.y * scalar;
    out.z += vec.z * scalar;
    out.w += vec.w * scalar;
}

//-------------------------
//   construtores matriz
//-------------------------

mat2 Mat2(vec2 cx, vec2 cy);

mat3 Mat3(vec3 cx, vec3 cy, vec3 cz);

mat4 Mat4(vec4 cx, vec4 cy, vec4 cz, vec4 cw);

//============================================================
//                   ANGULOS - QUATERNION
//============================================================

#define QUATERNION_EPS (1e-4)
#define Quat ScePspFQuaternion

Quat Quaternion(vec4 v);

static inline float Radians(float v){ return v * (M_PI / 180.0f); }

Quat Quat_from_axis_angle(ScePspFVector3 axis, float angle_rad);

Quat Quat_multiply(Quat q1, Quat q2);

Quat Quat_slerp(Quat q1, Quat q2, float t);

void Quat_to_matrix(mat4* m, Quat q);

#endif