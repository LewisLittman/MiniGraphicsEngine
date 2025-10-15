#ifndef LIGHTING_H
#define LIGHTING_H

#include "RayTriangleIntersection.h"
#include "Scene.h"

class Lighting 
{
public:
    static float proximityLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos, float lightStrength);
    static float angleOfIncidenceLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos);
    static float specularLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos, const Scene& scene);
    static float combinedLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos, const Scene& scene);
};

#endif