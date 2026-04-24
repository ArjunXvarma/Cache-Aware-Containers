#pragma once
#include <cstddef>

namespace cac {

struct ParticleProxy {
    float& x;
    float& y;
    float& z;

    float& vx;
    float& vy;
    float& vz;

    ParticleProxy& operator=(const Particle& p) {
        x = p.x; y = p.y; z = p.z;
        vx = p.vx; vy = p.vy; vz = p.vz;
        return *this;
    }

    // Conversion to Particle (value)
    operator Particle() const {
        return Particle{x, y, z, vx, vy, vz};
    }
};

struct ConstParticleProxy {
    const float& x;
    const float& y;
    const float& z;

    const float& vx;
    const float& vy;
    const float& vz;

    operator Particle() const {
        return Particle{x, y, z, vx, vy, vz};
    }
};

}