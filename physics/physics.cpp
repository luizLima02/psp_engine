#include"physics.hpp"

// ============================
//        FUNCTION AABB
// ============================

AABB Create_AABB(vec3 coord, vec3 size){
    return AABB{
        .position = vec4{coord.x, coord.y, coord.z, 1.0f},
        .size = vec4{size.x, size.y, size.z}
    };
}

// ============================
//       FUNCTION SPHERE
// ============================

SPHERE Create_SPHERE(vec3 position, float r){
    return SPHERE{
        .pos_size = vec4{position.x, position.y, position.z, r}
    };
}


// ===================================
//              COLLISION
// ===================================

bool Collides(const SPHERE& s1, const SPHERE& s2){
    vec3 c1 = vec3{s1.pos_size.x, s1.pos_size.y, s1.pos_size.z};
    vec3 c2 = vec3{s2.pos_size.x, s2.pos_size.y, s2.pos_size.z};
    float dist = powf((c1.x - c2.x), 2)+
                 powf((c1.y - c2.y), 2)+
                 powf((c1.z - c2.z), 2);
    return dist < powf(s1.pos_size.w + s2.pos_size.w, 2);
}

bool Collides(const AABB& s1, const AABB& s2){
    vec3 s1minP = minP(s1); vec3 s1maxP = maxP(s1);
    vec3 s2minP = minP(s2); vec3 s2maxP = maxP(s2);
    return !( s1minP.x > s2maxP.x ||
              s1minP.y > s2maxP.y ||
              s1minP.z > s2maxP.z ||
              s1maxP.x < s2minP.x ||
              s1maxP.y < s2minP.y ||
              s1maxP.z < s2minP.z
    );
}

bool Collides(const AABB& s1, const SPHERE& s2){
    vec3 s1minP = minP(s1); vec3 s1maxP = maxP(s1);
    float minX = s1minP.x; float minY = s1minP.y; float minZ = s1minP.z;
    float maxX = s1maxP.x; float maxY = s1maxP.y; float maxZ = s1maxP.z;

    float cx = s2.pos_size.x;
    float cy = s2.pos_size.y;
    float cz = s2.pos_size.z;

    float px = std::max(minX, std::min(cx, maxX));
    float py = std::max(minY, std::min(cy, maxY));
    float pz = std::max(minZ, std::min(cz, maxZ));

    float dx = cx - px;
    float dy = cy - py;
    float dz = cz - pz;

    float d = (dx*dx) + (dy*dy) + (dz*dz);

    return d <= (s2.pos_size.w*s2.pos_size.w);
}

bool Collides(const SPHERE& s1, const AABB& s2){
    return Collides(s2, s1);
}


//=============================
//          Physics
//=============================
//-----------------
//     PARTICLE
//-----------------


void Integrate(Particle* particle, float dt){
    if((!particle) && (dt <= 0) && (particle->inverseMass <= 0)) return;
    
    //work out the acceleration from force.
    vec3 resultAcc;
    resultAcc.x = particle->acceleration.x;
    resultAcc.y = particle->acceleration.y;
    resultAcc.z = particle->acceleration.z;

    resultAcc.x += (particle->forceAccum.x * particle->inverseMass);
    resultAcc.y += (particle->forceAccum.y * particle->inverseMass);
    resultAcc.z += (particle->forceAccum.z * particle->inverseMass);

    // Update linear velocity from the acceleration.
    particle->velocity.x += (resultAcc.x * dt);
    particle->velocity.y += (resultAcc.y * dt);
    particle->velocity.z += (resultAcc.z * dt);

    // Impose drag.
    float v = powf(particle->damping, dt);
    particle->velocity.x *= v;
    particle->velocity.y *= v;
    particle->velocity.z *= v;

    //update linear pos
    AddScaledVector4(particle->position, particle->velocity, dt);

    ClearForce(particle);
}

void updateForce(Particle* p, const ForceGenerator* fg, float duration){
    switch(fg->type)
    {
        case FG_GRAVITY:
        applyGravity(
            p, 
            fg->data.gravity.values
        );
        break;

        case FG_DRAG:
        applyDrag(
            p, 
            fg->data.drag.k1, 
            fg->data.drag.k2
        );
        break;

        case FG_SPRING:
        applySpring(
            p,
            fg->data.spring.otherPos,
            fg->data.spring.springConstant,
            fg->data.spring.restLength
        );
        break;

        case FG_BUOYANCY:
        applyBuoyancy(
            p,
            fg->data.buoyancy.maxDepth,
            fg->data.buoyancy.volume,
            fg->data.buoyancy.waterHeight,
            fg->data.buoyancy.liquidDensity
        );
        break;

        default:
        break;
    }
}

ForceGenerator Create_GravityGenerator(vec4 g){
    ForceGenerator fg;
    fg.data.gravity.values = g;
    fg.type = FG_GRAVITY;
    return fg;
}

ForceGenerator Create_DragGenerator(float k1, float k2){
    ForceGenerator fg;
    fg.data.drag.k1 = k1;
    fg.data.drag.k2 = k2;
    fg.type = FG_DRAG;
    return fg;
}

ForceGenerator Create_SpringGenerator(vec4* other, float sc, float rl){
    ForceGenerator fg;
    fg.data.spring.otherPos = other;
    fg.data.spring.springConstant = sc;
    fg.data.spring.restLength = rl;
    fg.type = FG_SPRING;
    return fg;
}

ForceGenerator Create_BuoyancyGenerator(float maxDepth, float volume, float waterHeight, float liquidDensity){
    ForceGenerator fg;
    fg.data.buoyancy.maxDepth = maxDepth;
    fg.data.buoyancy.volume = volume;
    fg.data.buoyancy.waterHeight = waterHeight;
    fg.data.buoyancy.liquidDensity = liquidDensity;
    fg.type = FG_BUOYANCY;
    return fg;
}

