//
// Created by goksu on 2/25/20.
//

#include <omp.h>
#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene, const int spp)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(0, 5, -18);
    // int pixel_count = 0;

    // change the spp value to change sample ammount
    std::cout << "SPP: " << spp << "\n";

    for (int j = 0; j < scene.height; ++j) {
        // OpenMP multi-thread
        #pragma omp parallel for
        for (int i = 0; i < scene.width; ++i) {
            int pixel_count = j * scene.width + i;
            // generate primary ray direction
            for (int k = 0; k < spp; k++){
                
                // MSAA emit direction ratio is k divide spp
                float x = (2 * (i + k / (float) spp) / (float) scene.width - 1) * scale * imageAspectRatio;
                float y = (1 - 2 * (j + k / (float) spp) / (float) scene.height) * scale;

                Vector3f direction = normalize(Vector3f(-x, y, 1));
                Vector3f pixel_color = scene.castRay(Ray(eye_pos, direction), 0) / spp;
                // if color is NaN return zero
                pixel_color = Renderer::validColor(pixel_color);
                framebuffer[pixel_count] += pixel_color;
            }
        }
        UpdateProgress(j / (float)scene.height);
    }
    UpdateProgress(1.f);

    Renderer::writeImage(scene.width, scene.height, framebuffer);
}


bool Renderer::writeImage(const int width, const int height, const std::vector<Vector3f> framebuffer, const char* path)
{
    // save framebuffer to file
    FILE* fp = fopen(path, "w");
    fprintf(fp, "P3\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height; i++) {
        static unsigned int color[3];
        color[0] = (unsigned int)(255 * clamp(0, 1, std::pow(framebuffer[i].x, 0.35)));
        color[1] = (unsigned int)(255 * clamp(0, 1, std::pow(framebuffer[i].y, 0.35)));
        color[2] = (unsigned int)(255 * clamp(0, 1, std::pow(framebuffer[i].z, 0.35)));
        fprintf(fp, "%ud %ud %ud\n", color[0], color[1], color[2]);
    }
    fclose(fp);
    return true;
}


Vector3f Renderer::validColor(const Vector3f pixel_color)
{
    if (pixel_color.x != pixel_color.x || pixel_color.y != pixel_color.y || pixel_color.z != pixel_color.z) {
        return Vector3f(0, 0, 0);
    }
    else {
        return pixel_color;
    }
}