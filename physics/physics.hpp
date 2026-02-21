#pragma once

#ifndef PSP_ENGINE_PHYSICS
#define PSP_ENGINE_PHYSICS

#include "../core/core.hpp"

// ============================
//      STRUCTS COLLISION
// ============================

typedef struct{
    vec4 position;
    vec4 size;
} __attribute__((aligned(16))) AABB;

/**
* @brief stores the radius in the w coordinate
*/
typedef struct{
    vec4 pos_size;
} __attribute__((aligned(16))) SPHERE;


// ============================
//        FUNCTION AABB
// ============================
         
/**
*  @brief Cria um corpo de colisão AABB centrado no ponto (x, y, z) com tamanho (sx, sy, sz).
*  @param coord the coordinate where the box will be centered.
*  @param size the box (HALF) size in each axis.
*  @return Estrutura AABB.
*/
AABB Create_AABB(vec3 coord, vec3 size);

/**
*  @brief Atualiza uma caixa AABB
*  @param data the AABB to be changed
*  @param newPos the new position
*/
static inline void Update_AABB(AABB* data, vec3 newPos){
    data->position.x = newPos.x;
    data->position.y = newPos.y;
    data->position.z = newPos.z;
}

/**
*  @brief Get the minimal point of a AABB box
*  @param data the AABB box to get the point
*  @return a vec3.
*/
static inline vec3 minP(const AABB& data){
    return vec3{data.position.x-data.size.x, 
                data.position.y-data.size.y,
                data.position.z-data.size.z
                };
}

/**
*  @brief Get the maximal point AABB box
*  @param data the AABB box to get the point
*  @return a vec3.
*/
static inline vec3 maxP(const AABB& data){
    return vec3{data.position.x+data.size.x, 
                data.position.y+data.size.y,
                data.position.z+data.size.z
                };
}

// ============================
//       FUNCTION SPHERE
// ============================

/**
*  @brief Create a SPHERE
*  @param position the coordinate where the sphere will be centered.
*  @param r the sphere radius.
*  @return a SPHERE.
*/
SPHERE Create_SPHERE(vec3 position, float r);

/**
*  @brief update a SPHERE
*  @param data the SPHERE to be changed
*  @param newPos the new coordinates.
*/
static inline void Update_SPHERE(SPHERE* data, vec3 newPos){
    data->pos_size.x = newPos.x;
    data->pos_size.y = newPos.y;
    data->pos_size.z = newPos.z;
}

// ===================================
//              COLLISION
// ===================================

//SPHERE x
bool Collides(const SPHERE& s1, const SPHERE& s2);
bool Collides(const SPHERE& s1, const AABB& s2);

//AABB x
bool Collides(const AABB& s1, const AABB& s2);
bool Collides(const AABB& s1, const SPHERE& s2);


//================================================
//                     PHYSICS
//================================================

//=============================
//  STRUCTS PHYSICS SIMULATION
//=============================
//-----------------
//     PARTICLE
//-----------------

typedef struct{
    //position in the 3d space
    vec4 position; //16 Bytes

    //linear velocity
    vec4 velocity; //16 Bytes

    //the particle acceleration
    vec4 acceleration; //16 Bytes

    //the force accum (reseted each frame)
    vec4 forceAccum; //16 Bytes

    //drag to simulate resistence
    float damping; //4 Bytes

    // 1/mass: inverse mass = 8 (static object)
    float inverseMass; //4 Bytes

    //para deixar multiplo de 16
    char padding[8]; //Padding de 8 Bytes
} __attribute__((aligned(16))) Particle;

static inline Particle Create_Particle(vec4 position, float inverseMass, 
                                      float damping, vec4 velocity, vec4 acceleration)
{
    Particle particulas;
    particulas.position = position;
    particulas.inverseMass = inverseMass;
    particulas.damping = damping;
    particulas.velocity = velocity;
    particulas.acceleration = acceleration;
    return particulas;
}

static inline void ClearForce(Particle* particle){
    particle->forceAccum = vec4{0,0,0,0};
}

static inline void AddForce(Particle* particle, vec4 force){
    particle->forceAccum.x += force.x;
    particle->forceAccum.y += force.y;
    particle->forceAccum.z += force.z;
    particle->forceAccum.w += force.w;
}

void Integrate(Particle* particle, float dt);

//----------------------
//    Force Generator
//----------------------
enum ForceType{
    FG_GRAVITY,
    FG_DRAG,
    FG_SPRING,
    FG_BUOYANCY
};

typedef struct{
    union {
        //dados gravidade
        struct{vec4 values;}gravity; //16 Bytes
        //dados gravidade
        struct{float k1, k2; }drag; //8 Bytes
        // Dados para Mola
        struct {
            vec4* otherPos; //4 Bytes
            float springConstant; //4 Bytes
            float restLength; //4 Bytes
        }spring;
        struct{
            /**
            * The maximum submersion depth of the object before
            * it generates its maximum buoyancy force.
            */
            float maxDepth; //4 Bytes
            /**
            * The volume of the object.
            */
            float volume; //4 Byte
            /**
            * The height of the water plane above y=0. The plane will be
            * parallel to the XZ plane.
            */
            float waterHeight; //4 Byte
            /**
            * The density of the liquid. Pure water has a density of
            * 1000 kg per cubic meter.
            */
            float liquidDensity; //4 Byte
        }buoyancy; //16 Bytes
    } data; //16 Bytes
    ForceType type; //4 Bytes
    int padding_bot[3]; //12 Bytes
} __attribute__((aligned(16))) ForceGenerator; //32 Bytes

typedef struct {
    Particle* p; //4 bytes
    ForceGenerator* fg; //4 bytes
} ForceRegistration;

//--------------
//   Functions
//--------------

static inline void applyGravity(Particle* p, const ScePspFVector4& gravity) {
    if (p->inverseMass <= 0.0f) return;
    // f = m * g (Como usamos invMass, f = g / invMass)
    vec4 g = vec4{gravity.x / p->inverseMass,
              gravity.y / p->inverseMass,
              gravity.z / p->inverseMass, 0};
    AddForce(p, g);
}

static inline void applyDrag(Particle* p, float k1, float k2) {
    vec4 v = p->velocity;
    float dragCoeff = Norm(v);
    dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

    vec3 force = Normalize(vec3{v.x, v.y, v.z});
    force *= -dragCoeff;
    AddForce(p, vec4{force.x, force.y, force.z, 0});
}

static inline void applySpring(Particle* p, vec4* other, float sc, float rl, bool Bungee = false){
    vec4 force;
    force.x = p->position.x - other->x;
    force.y = p->position.y - other->y;
    force.z = p->position.z - other->z;
    force.w = 0;

    // 2. Calcular a magnitude da distância
    float magnitude = sqrtf(force.x*force.x + force.y*force.y + force.z*force.z);
    if(magnitude <= rl) return;
    // Proteção contra divisão por zero caso as partículas estejam no mesmo lugar 
    if(magnitude <= 0.0001f && magnitude >= -0.0001f) return;

    // 3. Calcular o coeficiente da força: sprintConstant * (magnitude - restLength)
    float totalForce = 0;
    if(Bungee)
        totalForce = sc * (rl - magnitude);
    else
        totalForce = fabs(magnitude - rl) * sc;

    // 4. Normalizar o vetor força e aplicar a magnitude total
    float invMag = 1.0f / magnitude;
    force.x *= -(invMag * totalForce);
    force.y *= -(invMag * totalForce);
    force.z *= -(invMag * totalForce);
    
    // 5. Adicionar ao acumulador da partícula
    AddForce(p, force);
}

static inline void applyBuoyancy(Particle* p, float maxDepth, float volume, float waterHeight, float liquidDensity){
    //calculate the submersion depth.
    float depth = p->position.y;

    //check if we're out of water.
    if(depth >= waterHeight + maxDepth) return;
    vec4 force = vec4{0,0,0,0};

    //check if we're at maximum depth.
    if(depth <= waterHeight - maxDepth)
    {
        force.y = liquidDensity * volume;
        AddForce(p, force);
        return;
    }
    // Otherwise we are partly submerged.
    force.y = liquidDensity * volume * ((depth - maxDepth - waterHeight) / (2 * maxDepth));
    AddForce(p, force);
}

void updateForce(Particle* p, const ForceGenerator* fg, float duration);

ForceGenerator Create_GravityGenerator(vec4 g);

ForceGenerator Create_DragGenerator(float k1, float k2);

ForceGenerator Create_SpringGenerator(vec4* other, float sc, float rl);

ForceGenerator Create_BuoyancyGenerator(float maxDepth, float volume, 
                                        float waterHeight, float liquidDensity = 1000.0f);

//-----------------------------
//      Physics Colision
//-----------------------------
typedef struct{
    Particle* particle[2]; //8 Bytes
    float restuition; //4 Bytes
    float penetration; //4 bytes
    //16
    vec4 contactNormal; //16
}ParticleContact;


static inline float calculateSeparatingVelocity(ParticleContact* pc){
    vec4 relativeVelocity = pc->particle[0]->velocity;
    if(pc->particle[1]) relativeVelocity -= pc->particle[1]->velocity;
    return DOT(relativeVelocity, pc->contactNormal);
}

static void resolveVelocity(ParticleContact* pc, float dt){
    // Find the velocity in the direction of the contact.
    float separatingVelocity = calculateSeparatingVelocity(pc);

    // Check whether it needs to be resolved.
    if(separatingVelocity > 0)
    {
    // The contact is either separating or stationary- there’s
    // no impulse required.
        return;
    }
    float newSepVelocity = -separatingVelocity * pc->restuition;

    // Check the velocity build-up due to acceleration only.
    vec4 accCausedVelocity = pc->particle[0]->acceleration;
    if(pc->particle[1]) accCausedVelocity -= pc->particle[1]->acceleration;
    vec4 contactNormalD = vec4{pc->contactNormal.x*dt, 
                               pc->contactNormal.y*dt,
                               pc->contactNormal.z*dt,
                               0
                            };
    float accCausedSepVelocity = DOT(accCausedVelocity, contactNormalD);
    // If we’ve got a closing velocity due to acceleration build-up,
    // remove it from the new separating velocity.
    if(accCausedSepVelocity < 0)
    {
        newSepVelocity += pc->restuition * accCausedSepVelocity;
        // Make sure we haven’t removed more than was
        // there to remove.
        if (newSepVelocity < 0) newSepVelocity = 0;
    }

    float deltaVelocity = newSepVelocity - separatingVelocity;
    // We apply the change in velocity to each object in proportion to
    // its inverse mass (i.e., those with lower inverse mass [higher
    // actual mass] get less change in velocity).
    float totalInverseMass = pc->particle[0]->inverseMass;
    if (pc->particle[1]) totalInverseMass += pc->particle[1]->inverseMass;

    // If all particles have infinite mass, then impulses have no effect.
    if (totalInverseMass <= 0) return;

    // Calculate the impulse to apply.
    float impulse = deltaVelocity / totalInverseMass;

    // Find the amount of impulse per unit of inverse mass.
    vec4 impulsePerIMass;
    impulsePerIMass.x = pc->contactNormal.x * impulse;
    impulsePerIMass.y = pc->contactNormal.y * impulse;
    impulsePerIMass.z = pc->contactNormal.z * impulse;
    impulsePerIMass.w = 0;
    // Apply impulses: they are applied in the direction of the contact,
    // and are proportional to the inverse mass.
    pc->particle[0]->velocity.x += impulsePerIMass.x * pc->particle[0]->inverseMass;
    pc->particle[0]->velocity.y += impulsePerIMass.y * pc->particle[0]->inverseMass;
    pc->particle[0]->velocity.z += impulsePerIMass.z * pc->particle[0]->inverseMass;
    if(pc->particle[1])
    {
        pc->particle[1]->velocity.x += impulsePerIMass.x * -pc->particle[1]->inverseMass;
        pc->particle[1]->velocity.y += impulsePerIMass.y * -pc->particle[1]->inverseMass;
        pc->particle[1]->velocity.z += impulsePerIMass.z * -pc->particle[1]->inverseMass;
    }
}

static void resolveInterpenetration(ParticleContact* pc, float dt){
    // If we don’t have any penetration, skip this step.
    if(pc->penetration <= 0) return;

    // The movement of each object is based on its inverse mass, so
    // total that.
    float totalInverseMass = pc->particle[0]->inverseMass;
    if(pc->particle[1]) totalInverseMass += pc->particle[1]->inverseMass;

    // If all particles have infinite mass, then we do nothing.
    if(totalInverseMass <= 0) return;

    // Find the amount of penetration resolution per unit of inverse mass.
    vec4 movePerIMass;
    movePerIMass.x = pc->contactNormal.x * (-pc->penetration / totalInverseMass);
    movePerIMass.y = pc->contactNormal.y * (-pc->penetration / totalInverseMass);
    movePerIMass.z = pc->contactNormal.z * (-pc->penetration / totalInverseMass);
    movePerIMass.w = 0;
    // Apply the penetration resolution.
    pc->particle[0]->position.x += movePerIMass.x * pc->particle[0]->inverseMass;
    pc->particle[0]->position.y += movePerIMass.y * pc->particle[0]->inverseMass;
    pc->particle[0]->position.z += movePerIMass.z * pc->particle[0]->inverseMass;
    if(pc->particle[1])
    {
        pc->particle[1]->position.x += movePerIMass.x * pc->particle[1]->inverseMass;
        pc->particle[1]->position.y += movePerIMass.y * pc->particle[1]->inverseMass;
        pc->particle[1]->position.z += movePerIMass.z * pc->particle[1]->inverseMass;
    }
}

static inline void resolve(ParticleContact* pc, float dt){
    resolveVelocity(pc, dt);
    resolveInterpenetration(pc, dt);
}


typedef struct{
    unsigned iterations; //4 Bytes
    unsigned iterationsUsed; //4 Bytes
    void resolveContacts(ParticleContact *contactArray, unsigned numContacts,float dt);
}ParticleContactResolver;

ParticleContactResolver Create_ParticleContactResolver(unsigned iterations);


//-----------------
//  ParticleLink
//-----------------
enum LinkType{
    ParticleCable,
    ParticleRod
};

typedef struct{
    union{
        struct{float maxLenght, restitution;}cable;//8 bytes
        struct{float lenght;}rod; //4 bytes
    }data; //8 Bytes
    Particle* particle[2]; //8 Bytes
    //16 Bytes
    LinkType type; // 4 Bytes
    char padding[12]; // 12 Bytes
}ParticleLink;

static inline float currentLenght(ParticleLink pl){
    vec3 relativePos;
    relativePos.x = pl.particle[0]->position.x - pl.particle[1]->position.x;
    relativePos.y = pl.particle[0]->position.y - pl.particle[1]->position.y;
    relativePos.z = pl.particle[0]->position.z - pl.particle[1]->position.z;
    return Norm(relativePos);
}

static unsigned fillCable(ParticleLink pl, ParticleContact *contact, unsigned limit){
    float lenght = currentLenght(pl);
    // Check whether we’re overextended.
    if(lenght < pl.data.cable.maxLenght){
        return 0;
    }

    // Otherwise return the contact.
    contact->particle[0] = pl.particle[0];
    contact->particle[1] = pl.particle[1];
    // Calculate the normal.
    vec3 normal = vec3{pl.particle[1]->position.x - pl.particle[0]->position.x,
                       pl.particle[1]->position.y - pl.particle[0]->position.y,
                       pl.particle[1]->position.z - pl.particle[0]->position.z,
    };
    normal = Normalize(normal);
    contact->contactNormal.x = normal.x;
    contact->contactNormal.y = normal.y;
    contact->contactNormal.z = normal.z; contact->contactNormal.w = 0;

    contact->penetration = lenght - pl.data.cable.maxLenght;
    contact->restuition = pl.data.cable.restitution;

    return 1;
}

static unsigned fillRod(ParticleLink pl, ParticleContact *contact, unsigned limit){
    float currentLen = currentLenght(pl);
    // Check whether we’re overextended.
    if (currentLen == pl.data.rod.lenght)
    {
        return 0;
    }
    // Otherwise return the contact.
    contact->particle[0] = pl.particle[0];
    contact->particle[1] = pl.particle[1];
    // Calculate the normal.
    vec3 normal = vec3{pl.particle[1]->position.x - pl.particle[0]->position.x,
                       pl.particle[1]->position.y - pl.particle[0]->position.y,
                       pl.particle[1]->position.z - pl.particle[0]->position.z,
    };
    normal = Normalize(normal);
    // The contact normal depends on whether we’re extending
    // or compressing.
    if(currentLen > pl.data.rod.lenght){
        contact->contactNormal.x = normal.x;
        contact->contactNormal.y = normal.y;
        contact->contactNormal.z = normal.z; contact->contactNormal.w = 0;
        contact->penetration = currentLen - pl.data.rod.lenght;
    }else{
        contact->contactNormal.x = -1*normal.x;
        contact->contactNormal.y = -1*normal.y;
        contact->contactNormal.z = -1*normal.z; contact->contactNormal.w = 0;
        contact->penetration = pl.data.rod.lenght - currentLen;
    }
    contact->restuition = 0;
    return 1;
}

/**
* Fills the given contact structure with the contact needed
* to keep the cable from overextending.
*/
static unsigned fillContact(ParticleLink pl, ParticleContact *contact, unsigned limit)
{
    switch (pl.type)
    {
    case ParticleCable:
        return fillCable(pl, contact, limit);
        break;
    case ParticleRod:
        return fillRod(pl, contact, limit);
        break;
    default:
        break;
    }
    return 0;
}


#endif