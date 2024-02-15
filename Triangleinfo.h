#pragma once

#include "Vector3.h"
#include "Vector2.h"
#include "Color.h"

struct sTriangleInfo {
    Vector3 vertices[3];
    Vector2 uvs[3];
    Color colors[3];
    const Image* image; // Pointer to the image
};
