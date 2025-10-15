#ifndef MAPS_H
#define MAPS_H

#include <glm/glm.hpp>
#include "RayTriangleIntersection.h"
#include "TextureMap.h"
#include "Scene.h"

class Maps {
public:
    static glm::vec3 pixelManager(const RayTriangleIntersection& intersection, const Scene& scene);
    static glm::vec3 getTexturePixel(const RayTriangleIntersection& point, const TextureMap& texture);
    static glm::vec3 getNormalMapPixel(const RayTriangleIntersection& point, const TextureMap& normalMap);
};

#endif