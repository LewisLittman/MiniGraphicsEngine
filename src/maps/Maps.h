#ifndef MAPS_H
#define MAPS_H

#include <glm/glm.hpp>
#include "RayTriangleIntersection.h"
#include "TextureMap.h"
#include "../scene/Scene.h"

class Maps {
public:
    static uint32_t pixelManager(const RayTriangleIntersection& intersection, const glm::vec3& rayDirection, const Scene& scene);
    static glm::vec3 getTexturePixel(const RayTriangleIntersection& point, const TextureMap& texture);
    static glm::vec3 getPixelNormal(const RayTriangleIntersection& point, const TextureMap& normalMap);
    static uint32_t getEnvMapColour(const glm::vec3& rayDirection, const Scene& scene);
};

#endif