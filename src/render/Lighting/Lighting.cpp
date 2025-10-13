#include "Lighting.h"

float Lighting::proximityLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos, float lightStrength) {
    float distance = glm::length(lightPos - point.intersectionPoint);
    float intensity = lightStrength / (4 * PI * distance * distance);
    return glm::clamp(intensity, 0.0f, 1.0f);
}

float Lighting::angleOfIncidenceLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos) {
    glm::vec3 lightRay = glm::normalize(lightPos - point.intersectionPoint);
    float intensity = glm::dot(lightRay, point.intersectedTriangle.normal);
    return glm::clamp(intensity, 0.0f, 1.0f);
}

float Lighting::specularLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos, const Scene& scene) {
    glm::vec3 lightRay = glm::normalize(point.intersectionPoint - lightPos);
    glm::vec3 reflectionRay = glm::normalize(lightRay - 2.0f * point.intersectedTriangle.normal * glm::dot(lightRay, point.intersectedTriangle.normal));
    glm::vec3 viewRay = glm::normalize(scene.camera.position - point.intersectionPoint);
    float intensity = pow(glm::dot(viewRay, reflectionRay), 256.0f);
    return glm::clamp(intensity, 0.0f, 1.0f);
}

float Lighting::combinedLighting(const RayTriangleIntersection& point, const glm::vec3& lightPos, const Scene& scene) {
    float proximityIntensity = proximityLighting(point, lightPos, 10.0f);
    float AoIIntensity = angleOfIncidenceLighting(point, lightPos);
    float specularIntensity = specularLighting(point, lightPos, scene);
    float combinedIntensity = 0.2f * proximityIntensity + 0.7f * AoIIntensity + 0.5f * specularIntensity;
    return glm::clamp(combinedIntensity, 0.2f, 1.0f);
}