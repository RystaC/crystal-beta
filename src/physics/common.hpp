#pragma once

#include <format>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace physics {

struct Segment {
    glm::vec3 start, end;

    auto at(float t) const {
        return start + (end - start) * t;
    }
};

struct Plane {
    glm::vec3 normal;
    float dist;
};

struct Sphere {
    glm::vec3 center;
    float radius;
};

struct AABB {
    glm::vec3 min, max;
};

struct Capsule {
    Segment segment;
    float radius;
};

struct Convex {
    std::vector<glm::vec3> vertices;
};

struct RigidShape {
    glm::vec3 position;
    glm::quat rotate;
};

struct RegidBody {
    glm::mat3 inertia;
    float mass;
    float restitution;
    float friction;
};

struct Constraint {
    glm::vec3 axis;
    float inv;
    float rhs;
    float lower;
    float upper;
    float impulse;
};

struct Joint {
    float bias;
    uint32_t index_a;
    uint32_t index_b;
    glm::vec3 anchor_a;
    glm::vec3 anchor_b;
    Constraint constraint;
};

}