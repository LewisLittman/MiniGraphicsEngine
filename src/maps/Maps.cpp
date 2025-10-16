#include "Maps.h"
#include "../lighting/Lighting.h"
#include "../lighting/ShadingMode.h"

uint32_t Maps::pixelManager(const RayTriangleIntersection& intersection, const glm::vec3& rayDirection, const Scene& scene) {
    if (!intersection.hit) {
        return getEnvMapColour(rayDirection, scene);
    }

    glm::vec3 baseColour;
    if (intersection.intersectedTriangle.texture && !intersection.intersectedTriangle.normalMap) { // textured but no normal map
      const TextureMap& texture = scene.textures.at(intersection.intersectedTriangle.colour.name);
      baseColour = getTexturePixel(intersection, texture);
    } else {
      baseColour = glm::vec3(intersection.pointColour.red, intersection.pointColour.green, intersection.pointColour.blue);
    }

    float lightIntensity = 0.0f;
    switch (intersection.intersectedTriangle.shadingMode) {
        case GOURAUD: {
            lightIntensity = Lighting::gouraud(intersection, scene.lights[0], scene);
            break;
        }
        case PHONG: {
            lightIntensity = Lighting::phong(intersection, scene.lights[0], scene);
            break;
        }
        default: {
            if (intersection.intersectedTriangle.normalMap) { // has normal map
                const TextureMap& normalMap = scene.textures.at(intersection.intersectedTriangle.colour.name);
                glm::vec3 normalMapPixel = getPixelNormal(intersection, normalMap);
                lightIntensity = Lighting::normalMapIntensity(intersection, normalMapPixel, scene.lights[0], scene);
            } else { 
                lightIntensity = Lighting::combinedLighting(intersection, scene.lights[0], scene);
            }
            break;
        }
    }

    return (255 << 24) + (int(intersection.pointColour.red * lightIntensity) << 16) + (int(intersection.pointColour.green * lightIntensity) << 8) + int(intersection.pointColour.blue * lightIntensity);



    // if (intersection.hit) {
    //   if (intersection.intersectedTriangle.texture && !intersection.intersectedTriangle.normalMap) { // textured but no normal map
    //       const TextureMap& texture = scene.textures.at(intersection.intersectedTriangle.colour.name);
    //       glm::vec3 texturePixel = getTexturePixel(intersection, texture);
    //       float lightIntensity = Lighting::combinedLighting(intersection, scene.lights[0], scene);
    //       return (255 << 24) + (int(texturePixel.r * lightIntensity) << 16) + (int(texturePixel.g * lightIntensity) << 8) + int(texturePixel.b * lightIntensity);
    //   } else if (intersection.intersectedTriangle.normalMap) { // has normal map
    //       const TextureMap& normalMap = scene.textures.at(intersection.intersectedTriangle.colour.name);
    //       glm::vec3 normalMapPixel = getPixelNormal(intersection, normalMap);
    //       float lightIntensity = Lighting::normalMapIntensity(intersection, normalMapPixel, scene.lights[0], scene);
    //       return (255 << 24) + (int(intersection.pointColour.red * lightIntensity) << 16) + (int(intersection.pointColour.green * lightIntensity) << 8) + int(intersection.pointColour.blue * lightIntensity);
    //   } else { // no texture or normal map
    //       switch (intersection.intersectedTriangle.shadingMode) {
    //           case GOURAUD: {
    //               float lightIntensity = Lighting::gouraud(intersection, scene.lights[0], scene);
    //               return (255 << 24) + (int(intersection.pointColour.red * lightIntensity) << 16) + (int(intersection.pointColour.green * lightIntensity) << 8) + int(intersection.pointColour.blue * lightIntensity);
    //           }
    //           case PHONG: {
    //               float lightIntensity = Lighting::phong(intersection, scene.lights[0], scene);
    //               return (255 << 24) + (int(intersection.pointColour.red * lightIntensity) << 16) + (int(intersection.pointColour.green * lightIntensity) << 8) + int(intersection.pointColour.blue * lightIntensity);
    //           }
    //           default: {
    //               float lightIntensity = Lighting::combinedLighting(intersection, scene.lights[0], scene);
    //               return (255 << 24) + (int(intersection.pointColour.red * lightIntensity) << 16) + (int(intersection.pointColour.green * lightIntensity) << 8) + int(intersection.pointColour.blue * lightIntensity);
    //           }
    //       }
    //   }
    // } else {
    //   return getEnvMapColour(rayDirection, scene);
    // }  
}

glm::vec3 Maps::getTexturePixel(const RayTriangleIntersection& point, const TextureMap& texture) {
    float x = (1 - point.u - point.v) * point.intersectedTriangle.texturePoints[0].x + point.u * point.intersectedTriangle.texturePoints[1].x + point.v * point.intersectedTriangle.texturePoints[2].x;
    float y = (1 - point.u - point.v) * point.intersectedTriangle.texturePoints[0].y + point.u * point.intersectedTriangle.texturePoints[1].y + point.v * point.intersectedTriangle.texturePoints[2].y;
    x *= texture.width;
    y *= texture.height;

    float texturePixel = round(x) + round(y) * texture.width;
    uint32_t colour = texture.pixels[texturePixel];
    float r = ((colour >> 16) & 0xFF);
    float g = ((colour >> 8)  & 0xFF);
    float b = (colour & 0xFF);

    return glm::vec3(r, g, b);
}    

glm::vec3 Maps::getPixelNormal(const RayTriangleIntersection& point, const TextureMap& normalMap) {
    float x = (1 - point.u - point.v) * point.intersectedTriangle.texturePoints[0].x + point.u * point.intersectedTriangle.texturePoints[1].x + point.v * point.intersectedTriangle.texturePoints[2].x;
    float y = (1 - point.u - point.v) * point.intersectedTriangle.texturePoints[0].y + point.u * point.intersectedTriangle.texturePoints[1].y + point.v * point.intersectedTriangle.texturePoints[2].y;
    x *= normalMap.width;
    y *= normalMap.height;

    float texturePixel = round(x) + round(y) * normalMap.width;
    uint32_t colour = normalMap.pixels[texturePixel];
    float r = ((colour >> 16) & 0xFF);
    float g = ((colour >> 8)  & 0xFF);
    float b = (colour & 0xFF);

    r = (r / 255.0f) * 2.0f - 1.0f;
    g = (g / 255.0f) * 2.0f - 1.0f;
    b = (b / 255.0f) * 2.0f - 1.0f;

    return glm::normalize(glm::vec3(r, g, b));
}

// vec3 colourToNormal(Colour normalMapColour, RayTriangleIntersection point) {
//   float x,y,z;
//   if (point.intersectedTriangle.normal == vec3(0,0,1)) { //if front face of cube want z to be positive, x and y should be good
//     x = 128 - normalMapColour.red;
//     y = normalMapColour.green - 128;
//     z = normalMapColour.blue - 128;
//   } else if (point.intersectedTriangle.normal == vec3(1,0,0)) { //if right face of cube want z of colour -> x, x-> -z, y -> y
//     x = normalMapColour.blue - 128;
//     y = normalMapColour.green - 128;
//     z = normalMapColour.red - 128;
//   } else if (point.intersectedTriangle.normal == vec3(0, 0, -1)) { //if back face of cube inverse x, y is normal, inverse z
//     x = normalMapColour.red - 128;
//     y = normalMapColour.green - 128;
//     z = 128 - normalMapColour.blue;
//   } else if (point.intersectedTriangle.normal == vec3(-1, 0, 0)) {
//     x = 128 - normalMapColour.blue;
//     y = normalMapColour.green - 128;
//     z = 128 - normalMapColour.red;
//   }

//   return normalize(vec3(x, y, z));
// }

uint32_t Maps::getEnvMapColour(const glm::vec3& rayDirection, const Scene& scene) {
    float absX = fabs(rayDirection.x);
    float absY = fabs(rayDirection.y);
    float absZ = fabs(rayDirection.z);

    float u, v;
    std::string face;
    if (absX >= absY && absX >= absZ) {
      if (rayDirection.x > 0) {
        u = rayDirection.z / absX;
        v = -rayDirection.y / absX;
        face = "px";
      } else {
        u = -rayDirection.z / absX;
        v = -rayDirection.y / absX;
        face = "nx";
      }
    } else if (absY >= absX && absY >= absZ) {
      if (rayDirection.y > 0) {
        u = rayDirection.x / absY;
        v = -rayDirection.z / absY;
        face = "py";
      } else {
        u = rayDirection.x / absY;
        v = rayDirection.z / absY;
        face = "ny";
      }
    } else {
      if (rayDirection.z > 0) {
        u = -rayDirection.x / absZ;
        v = -rayDirection.y / absZ;
        face = "nz";
      } else {
        u = rayDirection.x / absZ;
        v = -rayDirection.y / absZ;
        face = "pz";
      }
    }
    
    float xRatio = 0.5f * (u + 1.0f);
    float yRatio = 0.5f * (v + 1.0f);

    const TextureMap& envmap = scene.textures.at(face);

    float x = round(xRatio * (envmap.width - 1));
    float y = round(yRatio * (envmap.height - 1));

    float texturePixel = x + y * envmap.width;
    return envmap.pixels[texturePixel];
}





// float normalMapIntensity(RayTriangleIntersection point, vec3 pointNormal, vec3 light) {
//   vec3 AoIlightRay = normalize(vec3(light - point.intersectionPoint));
//   float AoIintensity = dot(AoIlightRay, pointNormal);
//   if(AoIintensity > 1) AoIintensity = 1;
//   if (AoIintensity < 0) AoIintensity = 0;
//   return AoIintensity;

//   float distance = length(light - point.intersectionPoint);
//   float proxIntensity = 20 / (4 * PI * distance * distance); //20 = light strength
//   if (proxIntensity > 1) proxIntensity = 1;
//   if (proxIntensity < 0) proxIntensity = 0;

//   //Specular lighting for each vertex
//   vec3 specLightRay = normalize(vec3(point.intersectionPoint - light));
//   vec3 reflectionRay = normalize(specLightRay - 2 * pointNormal * dot(specLightRay, pointNormal));
//   vec3 viewRay = normalize(cameraPosition - point.intersectionPoint);
//   float specIntensity = pow(dot(viewRay, reflectionRay), 256);
//   if (specIntensity > 1) specIntensity = 1;
//   if (specIntensity < 0) specIntensity = 0;

//   float combinedIntensity = 0.7 * proxIntensity + 0.5 * AoIintensity + 0.3 * specIntensity;
//   if (combinedIntensity < 0.2) combinedIntensity = 0.2;
//   if (combinedIntensity > 1) combinedIntensity = 1;
// }