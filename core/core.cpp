#include"core.hpp"

//============================================================
//                          AUXILIARES
//============================================================


int size_of_string(const char* s){
    int size = 0;
    while(s[size] != 0){
        size++;
    }
    return size;
}

const char* concatString(const char* s1, const char* s2) {
    // Trata casos onde uma ou ambas as strings podem ser NULL
    if (s1 == NULL) {
        s1 = "";
    }
    if (s2 == NULL) {
        s2 = "";
    }
    // 1. Calcula o tamanho total necessário para a nova string
    //    tamanho_de_s1 + tamanho_de_s2 + 1 byte para o caractere nulo '\0'
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    size_t total_len = len1 + len2 + 1;

    // 2. Aloca memória na heap com o tamanho calculado
    char* resultado = (char*)malloc(total_len);

    // 3. Verifica se a alocação de memória foi bem-sucedida
    if (resultado == NULL) {
        // Se malloc falhar, imprime um erro e retorna NULL
        perror("Falha ao alocar memória em concatString");
        return NULL;
    }

    // 4. Copia a primeira string para o buffer alocado
    strcpy(resultado, s1);

    // 5. Concatena (anexa) a segunda string ao final do buffer
    strcat(resultado, s2);

    // 6. Retorna o ponteiro para a nova string
    return resultado;
}


//============================================================
//                   PSP CALLBACKS
//============================================================


int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

//delta time
float delta_time = 0.0f;
SceInt64 last_time = 0;

void calc_delta(){
    // --- Cálculo do Delta Time (como mostrado acima) ---
    SceInt64 current_time = sceKernelGetSystemTimeWide();
    if (last_time != 0) {
        delta_time = (float)(current_time - last_time) / 1000000.0f;
    }
    last_time = current_time;
}


//============================================================
//                   VETORES - MATRIZES
//============================================================

//-------------------------
//   construtores vetores
//-------------------------

vec2 Vec2(float x, float y){
    return vec2{.x = x, .y = y};
}

vec3 Vec3(float x , float y , float z ){
   return vec3{.x = x, .y = y, .z = z};
}

vec3 Vec3(vec4 v){
    return vec3{.x = v.x, .y = v.y, .z = v.z};
}

vec4 Vec4(float x , float y , float z , float w ){
    return vec4{.x = x, .y = y, .z = z, .w = w};
}

vec4 Vec4(vec3 v, float w ){
    return vec4{.x = v.x, .y = v.y, .z = v.z, .w = w};
}


//-------------------------
//      funçoes basicas
//-------------------------

/**
 * Returns a Inverts the vector
 */
vec2 Invert(vec2 data){
    return vec2{-data.x, -data.y};
}
/**
 * Returns a Inverts the vector
 */
vec3 Invert(vec3 data){
    return vec3{-data.x, -data.y, -data.z};
}
/**
 * Returns a Inverts the vector
 */
vec4 Invert(vec4 data){
    return vec4{-data.x, -data.y, -data.z, -data.w};
}

//================================
//             Norm
//================================

float Norm(vec2 v, vec2 u){
    return sqrtf(powf((u.x - v.x), 2)+powf((u.y - v.y), 2));
}

float Norm(vec3 v, vec3 u){
    return sqrtf(powf((u.x - v.x), 2)+powf((u.y - v.y), 2)+powf((u.z - v.z), 2));
}

float Norm(vec4 v, vec4 u){
    return sqrtf(powf((u.x - v.x), 2)+powf((u.y - v.y), 2)+powf((u.z - v.z), 2));
}

vec3 Normalize(vec3 t){
    vec3 n = vec3{t.x, t.y, t.z};
    gumNormalize(&n);
    return n;
}

//=============================
//    Especial Functions
//=============================

//Cross product
vec3 Cross(vec3 u, vec3 t){
    vec3 res;
    gumCrossProduct(&res, &u, &t);
    return res;
}

//Dot product
float DOT(vec2 t, vec2 outro){
    return (t.x * outro.x) + (t.y * outro.y);
}

//Dot product
float DOT(vec3 t, vec3 outro){
    return gumDotProduct(&t, &outro);
}

/*
* Retorna o produto escalar dos vetores Passados
*/
float DOT(vec4 t, vec4 outro){
    return (t.x*outro.x) + (t.y*outro.y) + (t.z*outro.z);
}
//-------------------------
//   construtores matriz
//-------------------------

mat2 Mat2(vec2 cx, vec2 cy){
    return mat2{.x = cx, .y = cy};
}

mat3 Mat3(vec3 cx, vec3 cy, vec3 cz){
    return mat3{.x = cx, .y = cy, .z = cz};
}

mat4 Mat4(vec4 cx, vec4 cy, vec4 cz, vec4 cw){
    return mat4{.x = cx, .y = cy, .z = cz, .w = cw};
}


//============================================================
//                   ANGULOS - QUATERNION
//============================================================

Quat Quaternion(vec4 v){
    Quat q = {.x = v.x, 
              .y = v.y, 
              .z = v.z, 
              .w = v.w
            };
    return q;
}

Quat Quat_from_axis_angle(ScePspFVector3 axis, float angle_rad) {
    Quat q;
    // A matemática do quatérnion usa metade do ângulo.
    float half_angle = angle_rad * 0.5f;
    float s = sinf(half_angle);
    float c = cosf(half_angle);

    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    q.w = c;

    return q;
}

Quat Quat_multiply(Quat q1, Quat q2) {
    Quat r;
    r.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    r.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    r.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    r.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return r;
}

Quat Quat_slerp(Quat q1, Quat q2, float t) {
    Quat result;
    double cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
    // if q1=q2 or qa=-q2 then theta = 0 and we can return q1
    if (fabs(cosHalfTheta) >= 1.0) {
        result = q1;
        return result;
    }
    double halfTheta = acos(cosHalfTheta);
    double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
    // If theta = 180 degrees then result is not fully defined
    // We could rotate around any axis normal to q1 or q2
     if (fabs(sinHalfTheta) < QUATERNION_EPS) {
        result.w = (q1.w * 0.5 + q2.w * 0.5);
        result.x = (q1.x * 0.5 + q2.x * 0.5);
        result.y = (q1.y * 0.5 + q2.y * 0.5);
        result.z = (q1.z * 0.5 + q2.z * 0.5);
    }else{
        // Default quaternion calculation
        double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
        double ratioB = sin(t * halfTheta) / sinHalfTheta;
        result.w = (q1.w * ratioA + q2.w * ratioB);
        result.x = (q1.x * ratioA + q2.x * ratioB);
        result.y = (q1.y * ratioA + q2.y * ratioB);
        result.z = (q1.z * ratioA + q2.z * ratioB);
    }
    return result;
}

void Quat_to_matrix(mat4* m, Quat q) {
    float x2 = q.x * 2.0f; float y2 = q.y * 2.0f; float z2 = q.z * 2.0f;
    float xx = q.x * x2;   float xy = q.x * y2;   float xz = q.x * z2;
    float yy = q.y * y2;   float yz = q.y * z2;   float zz = q.z * z2;
    float wx = q.w * x2;   float wy = q.w * y2;   float wz = q.w * z2;

    m->x.x = 1.0f - (yy + zz); m->y.x = xy - wz;        m->z.x = xz + wy;        m->w.x = 0.0f;
    m->x.y = xy + wz;        m->y.y = 1.0f - (xx + zz); m->z.y = yz - wx;        m->w.y = 0.0f;
    m->x.z = xz - wy;        m->y.z = yz + wx;        m->z.z = 1.0f - (xx + yy); m->w.z = 0.0f;
    m->x.w = 0.0f;           m->y.w = 0.0f;           m->z.w = 0.0f;           m->w.w = 1.0f;
}


