#include "Maps.h"

glm::vec3 Maps::pixelManager(const RayTriangleIntersection& intersection, const Scene& scene) {
    if (intersection.intersectedTriangle.texture) {
        const TextureMap& texture = scene.textures.at(intersection.intersectedTriangle.colour.name);
        return getTexturePixel(intersection, texture);
    } else {
        return glm::vec3(intersection.pointColour.red, intersection.pointColour.green, intersection.pointColour.blue);
    }
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



// else if (intersection.intersectedTriangle.normalMap) {
//         const TextureMap& normalMap = scene.textures.at(intersection.intersectedTriangle.colour.name);
//         return getNormalMapPixel(intersection, normalMap);
//     } 