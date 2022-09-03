//
// Created by goksu on 2/25/20.
//
#include "Scene.hpp"

#pragma once
struct hit_payload
{
    float tNear;
    uint32_t index;
    Vector2f uv;
    Object* hit_obj;
};

class Renderer
{
public:
    void Render(const Scene& scene, const int spp = 16);
    bool writeImage(const int width, const int height, const std::vector<Vector3f> framebuffer, const char* path = "binary.ppm");
    Vector3f validColor(const Vector3f pixel_color);
private:
};
