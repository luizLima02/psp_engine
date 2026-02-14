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

void init_perspective_2D(float left, float right, float top, float down, float near, float far);

void init_perspective_3D(float fovy, float aspect, float near, float far);


/**
 * @brief Cria uma matriz de modelo para um billboard que gira apenas no eixo Y.
 * @param cameraPosition A posição da câmera no mundo.
 * @param objectPosition A posição do objeto no mundo.
 */
void billboard_to_camera(const vec3 objectPosition,
                         const vec3 cameraPosition);

#endif