#include "WireFrameRenderer.h"
#include "Projection.h"
#include "CanvasPoint.h"
#include "Line.h"

void WireFrameRenderer::render(float focalLength, DrawingWindow& window, const Scene& scene) {
    for (const auto& triangle : scene.triangles) {
        CanvasPoint p0 = projectVertexOntoCanvasPoint(focalLength, triangle.vertices[0], scene.camera, window.height, window.width);
        CanvasPoint p1 = projectVertexOntoCanvasPoint(focalLength, triangle.vertices[1], scene.camera, window.height, window.width);
        CanvasPoint p2 = projectVertexOntoCanvasPoint(focalLength, triangle.vertices[2], scene.camera, window.height, window.width);
        drawLine(p0, p1, triangle.colour, window);
        drawLine(p1, p2, triangle.colour, window);
        drawLine(p2, p0, triangle.colour, window);
    }
}
