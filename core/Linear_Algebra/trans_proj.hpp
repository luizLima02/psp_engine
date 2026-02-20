#pragma once
#ifndef TRANSFORM_PROJECTION
#define TRANSFORM_PROJECTION

#include"../core/core.hpp"

// =====================
//     TRANSFORMAÇÕES
// =====================

/**
 * @brief Reset the Model matriz and translate the Matriz model to (x, y, z)
 * @param x x position to translate
 * @param y y position to translate
 * @param z z position to translate
 */
void reset_translate(float x, float y, float z);

/**
 * @brief Scales the Model matriz with the corresponding values in each axis
 * @param x factor to scale in the x-axis.
 * @param y factor to scale in the y-axis.
 * @param z factor to scale in the z-axis.
 */
void scale(float x = 1, float y = 1, float z = 1);

void rotateXYZ(float angle, float x, float y, float z);

// ======================
//      PERSPECTIVA
// ======================

/**
* @brief send the matriz to use the ortho projection
*/
void init_perspective_2D(float left = (-480.0f/272.0f), float right = (480.0f/272.0f), 
                         float top = 1.0f, float down = 1.0f, 
                         float near = -10.0f, float far = 100.0f);

/**
 * @brief send the matriz to use the ortho projection , converting a perspective projection  to ortho projection 
 */
void init_perspective_2D_Especial(float fovy = 75.0f, float aspect = (16.0f/9.0f), float near = -100.0f, float far = 100.0f);

/**
 * @brief send the matriz to use the perspective projection 
 */
void init_perspective_3D(float fovy = 75.0f, float aspect = (16.0f/9.0f), float near = 0.1f, float far = 100.0f);


/**
 * @brief Make a Billboarding Matriz and send To Model, it rotates only the y-axis
 * @param cameraPosition The world position of the Camera
 * @param objectPosition The object position of the Object
*/
void billboard_to_camera(const vec3 objectPosition,
                         const vec3 cameraPosition);

#endif