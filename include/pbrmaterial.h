#pragma once

#include <glm/glm.hpp>

#define ALBEDO_MASK 1;
#define NORMAL_MASK 1 << 1;
#define METALLIC_MASK 1 << 2;
#define ROUGHNESS_MASK 1 << 3;
#define AO_MASK 1 << 4;

class PbrMaterial
{
public:
    PbrMaterial()
    {
        baseColor = glm::vec3();
        metalness = .4f;
        roughness = 0.5f;
        ao = 0.1f;
        opacity = 1.0f;
    }

    glm::vec3 baseColor;
    float metalness;
    float roughness;
    float ao;
    float opacity;
    unsigned int baseColorMap;
    unsigned int metallicMap;
    unsigned int roughnessMap;
    unsigned int aoMap;
    unsigned int normalMap;
};