#include "Lighting.h"
#include "../other/Constants.h"

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

float Lighting::normalMapIntensity(const RayTriangleIntersection& point, const glm::vec3& pointNormal, const glm::vec3& light, const Scene& scene) {
  glm::vec3 AoIlightRay = normalize(glm::vec3(light - point.intersectionPoint));
  float AoIintensity = dot(AoIlightRay, pointNormal);
  if(AoIintensity > 1) AoIintensity = 1;
  if (AoIintensity < 0) AoIintensity = 0;
//   return AoIintensity;

  float distance = length(light - point.intersectionPoint);
  float proxIntensity = 20 / (4 * PI * distance * distance); //20 = light strength
  if (proxIntensity > 1) proxIntensity = 1;
  if (proxIntensity < 0) proxIntensity = 0;

  //Specular lighting for each vertex
  glm::vec3 specLightRay = glm::normalize(glm::vec3(point.intersectionPoint - light));
  glm::vec3 reflectionRay = glm::normalize(specLightRay - 2.0f * pointNormal * glm::dot(specLightRay, pointNormal));
  glm::vec3 viewRay = glm::normalize(scene.camera.position - point.intersectionPoint);
  float specIntensity = pow(dot(viewRay, reflectionRay), 256.0f);
  if (specIntensity > 1) specIntensity = 1;
  if (specIntensity < 0) specIntensity = 0;

  float combinedIntensity = 0.3 * proxIntensity + 0.7 * AoIintensity + 0.4 * specIntensity;
  if (combinedIntensity < 0) combinedIntensity = 0;
  if (combinedIntensity > 1) combinedIntensity = 1;
  return combinedIntensity;
}

float Lighting::gouraud(const RayTriangleIntersection& point, const glm::vec3& light, const Scene& scene) {
  std::vector<float> brightness;
  for (int i = 0; i < point.intersectedTriangle.vertices.size(); i++) {
    //proximity lighting for each vertex
    float distance = length(light - point.intersectedTriangle.vertices[i]);
    float proxIntensity = 20 / (4 * PI * distance * distance); //20 = light strength
    if (proxIntensity > 1) proxIntensity = 1;
    if (proxIntensity < 0) proxIntensity = 0;

    //AoI lighting for each vertex
    glm::vec3 AoIlightRay = normalize(glm::vec3(light - point.intersectedTriangle.vertices[i]));
    float AoIintensity = dot(AoIlightRay, point.intersectedTriangle.vertexNormals[i]);
    if(AoIintensity > 1) AoIintensity = 1;
    if (AoIintensity < 0) AoIintensity = 0;

    //Specular lighting for each vertex
    glm::vec3 specLightRay = normalize(glm::vec3(point.intersectedTriangle.vertices[i] - light));
    glm::vec3 reflectionRay = normalize(specLightRay - 2.0f * point.intersectedTriangle.vertexNormals[i] * dot(specLightRay, point.intersectedTriangle.vertexNormals[i]));
    glm::vec3 viewRay = normalize(scene.camera.position - point.intersectedTriangle.vertices[i]);
    float specIntensity = pow(dot(viewRay, reflectionRay), 256);
    if (specIntensity > 1) specIntensity = 1;
    if (specIntensity < 0) specIntensity = 0;

    float combinedIntensity = 0.2 * proxIntensity + 0.7 * AoIintensity + 0.4 * specIntensity;
    if (combinedIntensity < 0.2) combinedIntensity = 0.2;
    if (combinedIntensity > 1) combinedIntensity = 1;
    brightness.push_back(combinedIntensity);
  }
  float interpolatedCombinedBrightness = (1 - point.u - point.v) * brightness[0] + point.u * brightness[1] + point.v * brightness[2];
  return interpolatedCombinedBrightness;
}

float Lighting::phong(const RayTriangleIntersection& point, const glm::vec3& light, const Scene& scene) {
  glm::vec3 pointNormal = (1 - point.u - point.v) * point.intersectedTriangle.vertexNormals[0] + point.u * point.intersectedTriangle.vertexNormals[1] + point.v * point.intersectedTriangle.vertexNormals[2];
  // pointNormal = normalize(pointNormal);
  //proximity lighting for the point
  float distance = length(light - point.intersectionPoint);
  float proxIntensity = 10 / (4 * PI * distance * distance); //20 = light strength
  if (proxIntensity > 1) proxIntensity = 1;
  if (proxIntensity < 0) proxIntensity = 0;
  //AoI lighting for the point
  glm::vec3 AoIlightRay = normalize(glm::vec3(light - point.intersectionPoint));
  float AoIintensity = dot(AoIlightRay, pointNormal);
  if(AoIintensity > 1) AoIintensity = 1;
  if (AoIintensity < 0) AoIintensity = 0;
  //Specular lighting for each vertex
  glm::vec3 specLightRay = normalize(glm::vec3(point.intersectionPoint- light));
  glm::vec3 reflectionRay = normalize(specLightRay - 2.0f * pointNormal * dot(specLightRay, pointNormal));
  glm::vec3 viewRay = normalize(scene.camera.position - point.intersectionPoint);
  float specIntensity = pow(dot(viewRay, reflectionRay), 256);
  if (specIntensity > 1) specIntensity = 1;
  if (specIntensity < 0) specIntensity = 0;

  float combinedIntensity = 0.3 * proxIntensity + 0.5 * AoIintensity + 0 * specIntensity;
  if (combinedIntensity < 0.2) combinedIntensity = 0.2;
  if (combinedIntensity > 1) combinedIntensity = 1;
  return combinedIntensity;
}