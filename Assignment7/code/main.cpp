#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main(int argc, char* argv[])
{

    // Change the definition here to change resolution
    Scene scene(1024, 1024);

    Material* red = new Material(DIFFUSE, Vector3f(0.0f));
    red->Kd = Vector3f(0.6f, 0.05f, 0.05f);
    Material* green = new Material(DIFFUSE, Vector3f(0.0f));
    green->Kd = Vector3f(0.15f, 0.45f, 0.1f);
    Material* white = new Material(DIFFUSE, Vector3f(0.0f));
    white->Kd = Vector3f(0.75, 0.75, 0.75);
    Material* reflect = new Material(MIRROR, Vector3f(0.0f));
    reflect->Kd = Vector3f(0.0f, 0.0f, 0.0f);
    reflect->ior = 40.0;
    Material* light = new Material(DIFFUSE, (45.0 * Vector3f(0.9, 0.8, 0.7)));
    light->Kd = Vector3f(0.65f);

    // MeshTriangle front("cornell/pFront.obj", white);
    MeshTriangle floor("cornell/pBack.obj", white);
    MeshTriangle shortbox("cornell/boxSmall.obj", white);
    MeshTriangle tallbox("cornell/boxTall.obj", reflect);
    MeshTriangle left("cornell/pLeft.obj", red);
    MeshTriangle right("cornell/pRight.obj", green);
    MeshTriangle light_("cornell/light.obj", light);

    // scene.Add(&front);
    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene, 16);
    auto stop = std::chrono::system_clock::now();

    int hours = std::chrono::duration_cast<std::chrono::hours>(stop - start).count();

    int minutes = std::chrono::duration_cast<std::chrono::minutes>(stop - start).count();
    if (hours > 0) {
        minutes -= hours * 60;
    }

    int seconds = std::chrono::duration_cast<std::chrono::seconds>(stop - start).count();
    if (minutes > 0) {
        seconds -= hours * 3600 + minutes * 60;
    }

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << hours << " hours\n";
    std::cout << "          : " << minutes << " minutes\n";
    std::cout << "          : " << seconds << " seconds\n";

    return 0;
}