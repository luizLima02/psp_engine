#pragma once

#ifndef PSP_ENGINE_CAMERA
#define PSP_ENGINE_CAMERA

#include "../../core/core.hpp"
#include "../../core/Linear_Algebra/trans_proj.hpp"


enum Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

//=============
//    Dados
//=============

struct CameraData{
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 WorldUp;
};

struct RotateData{
    float yaw;
    float pitch;
    float zoom;
};

//==============================
//    Declaração das Funcoes
//==============================

CameraData Create_camera(vec3 pos, vec3 up, vec3 front = Vec3(0, 0, -1));

RotateData Create_rotate(float yaw = -90.0f, float pitch = 0.0f, float zoom = 45.0f);

//=================================================

CameraData UpdateCameraVectors2D(CameraData data);

CameraData UpdateCameraVectors3D(CameraData data, RotateData rot_data = Create_rotate());

//=================================================
CameraData ProcessGamePad(CameraData data, Camera_Movement direction,float deltaTime,  float speed = 2.5f);

RotateData ProcessRot(RotateData data, Camera_Movement direction, float deltaTime, float speed = 4.5f);

void SendCamData(CameraData data);

#endif