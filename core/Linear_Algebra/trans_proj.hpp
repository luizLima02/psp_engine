#pragma once
#ifndef TRANSFORM_PROJECTION
#define TRANSFORM_PROJECTION

#include"../core/core.hpp"

// =====================
//     TRANSFORMAÇÕES
// =====================

void reset_translate(float x, float y, float z);

void scale(float x, float y, float z);

void rotateXYZ(float angle, float x, float y, float z);

// ======================
//      PERSPECTIVA
// ======================

void init_perspective_2D(float left = (-480.0f/272.0f), float right = (480.0f/272.0f), 
                         float top = 1.0f, float down = 1.0f, 
                         float near = -10.0f, float far = 100.0f);

void init_perspective_2D_Especial(float fovy = 75.0f, float aspect = (16.0f/9.0f), float near = -100.0f, float far = 100.0f);

void init_perspective_3D(float fovy = 75.0f, float aspect = (16.0f/9.0f), float near = 0.1f, float far = 100.0f);


/**
 * @brief Cria uma matriz de modelo para um billboard que gira apenas no eixo Y.
 * @param cameraPosition A posição da câmera no mundo.
 * @param objectPosition A posição do objeto no mundo.
 */
void billboard_to_camera(const vec3 objectPosition,
                         const vec3 cameraPosition);

#endif