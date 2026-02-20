#include "camera.hpp"

//========================
//        Criação
//========================

CameraData Create_camera(vec3 pos, vec3 up, vec3 front){
    return CameraData{
        .Position = pos,
        .Front = front,
        .Up = up,
        .WorldUp = up,
    };
}

/*void ChangeCamPos(CameraData data, vec3 pos){
    data.Position = pos;
}*/

RotateData Create_rotate(float yaw, float pitch, float zoom){
    return RotateData{
        .yaw = yaw,
        .pitch = pitch,
        .zoom = zoom
    };
}

//========================
//       Todas Cams
//========================

void SendCamData(CameraData data){
    //carrega a camera
    vec3 front = {data.Position.x + data.Front.x,
                  data.Position.y + data.Front.y,
                  data.Position.z + data.Front.z
                 };
    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    sceGumLookAt(&data.Position, &front, &data.Up);
}

CameraData ProcessGamePad(CameraData data, Camera_Movement direction, float deltaTime, float speed){
    float velocity = speed * deltaTime;
    vec3 Pos = data.Position;
    vec3 Fro = data.Front;
    vec3 Upp = data.Up;
    if(direction == FORWARD){
        AddScaledVector3(Pos, Fro, velocity);
    }
    if(direction == BACKWARD){
        AddScaledVector3(Pos, Fro, -1.0f*velocity);
    }
    //Esquerda ou Direita, calcule vetor Right
    if(direction == LEFT || direction == RIGHT){
        vec3 right;
        gumCrossProduct(&right, &Fro, &data.WorldUp);
        gumNormalize(&right);
        if(direction == LEFT){
            AddScaledVector3(Pos, right, -1.0f*velocity);
        }
        if(direction == RIGHT){
            AddScaledVector3(Pos, right, velocity);
        }
    }
    if(direction == UP){
        AddScaledVector3(Pos, Upp, velocity);
    }
    if(direction == DOWN){
        AddScaledVector3(Pos, Upp, -1.0f*velocity);
    }
    
    return CameraData{
        .Position = Pos,
        .Front = Fro,
        .Up = Upp, 
        .WorldUp = data.WorldUp
    };
}

//========================
//       Camera 2D
//========================

CameraData UpdateCameraVectors2D(CameraData data){
    //=================================
    vec3 Frnt = data.Front;
    vec3 Wrldp = data.WorldUp;
    //=================================
    vec3 right;
    vec3 up;
    gumCrossProduct(&right, &Frnt, &Wrldp);
    gumNormalize(&right);

    gumCrossProduct(&up, &right, &Frnt);
    gumNormalize(&up);

    return CameraData{
        .Position = data.Position,
        .Front = data.Front,
        .Up = up, 
        .WorldUp = data.WorldUp
    };
}

//========================
//        Camera 3D
//========================

CameraData UpdateCameraVectors3D(CameraData data, RotateData rot_data){
    vec3 fro;
    fro.x = cosf(Radians(rot_data.yaw)) * cosf(Radians(rot_data.pitch));
    fro.y = sinf(Radians(rot_data.pitch));
    fro.z = sinf(Radians(rot_data.yaw)) * cosf(Radians(rot_data.pitch));
    gumNormalize(&fro);
   
    vec3 right;
    vec3 up;

    gumCrossProduct(&right, &data.Front, &data.WorldUp);
    gumNormalize(&right);

    gumCrossProduct(&up, &right, &data.Front);
    gumNormalize(&up);

    return CameraData{
        .Position = data.Position,
        .Front = fro,
        .Up = up,
        .WorldUp = data.WorldUp
    };
}


RotateData ProcessRot(RotateData data, Camera_Movement direction, float deltaTime, float speed){
    float pitch = data.pitch;
    float yaw   = data.yaw;
    if(direction == FORWARD){
        pitch -= speed*deltaTime;
    }
    if(direction == BACKWARD){
        pitch += speed*deltaTime;
    }
    if(direction == LEFT){
        yaw -= speed*deltaTime;
    }
    if(direction == RIGHT){
        yaw += speed*deltaTime;
    }
    //constrainPitch
    if (pitch > 89.0f){
        pitch = 89.0f;
    }if (pitch < -89.0f){
        pitch = -89.0f;
    }
    return RotateData{
        .yaw = yaw,
        .pitch = pitch,
        .zoom = data.zoom
    };
}